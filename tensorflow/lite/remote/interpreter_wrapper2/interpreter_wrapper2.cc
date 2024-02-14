/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "tensorflow/lite/remote/interpreter_wrapper2/interpreter_wrapper2.h"

#include <stdarg.h>

#include <cstring>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "absl/memory/memory.h"
#include "absl/strings/str_format.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/core/api/error_reporter.h"
#include "tensorflow/lite/core/api/op_resolver.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/internal/compatibility.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/kernels/register_ref.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/mutable_op_resolver.h"
#include "tensorflow/lite/python/interpreter_wrapper/numpy.h"
#include "tensorflow/lite/python/interpreter_wrapper/python_error_reporter.h"
#include "tensorflow/lite/python/interpreter_wrapper/python_utils.h"
#include "tensorflow/lite/shared_library.h"
#include "tensorflow/lite/string_util.h"
#include "tensorflow/lite/util.h"

#include <iostream>
#include <experimental/filesystem>
#include <dlfcn.h>
#include <fs_utils.h>
#include <memstreambuf.h>
#include <common.h>

#include "tflite_messages.h"

namespace tflite {
namespace interpreter_wrapper {

namespace {

using python_utils::PyDecrefDeleter;

PyObject* PyArrayFromFloatVector(const float* data, npy_intp size) {
  void* pydata = malloc(size * sizeof(float));
  if (data != nullptr) {
    memcpy(pydata, data, size * sizeof(float));
  }
  PyObject* obj = PyArray_SimpleNewFromData(1, &size, NPY_FLOAT32, pydata);
  PyArray_ENABLEFLAGS(reinterpret_cast<PyArrayObject*>(obj), NPY_ARRAY_OWNDATA);
  return obj;
}

PyObject* PyArrayFromIntVector(const int* data, npy_intp size) {
  void* pydata = malloc(size * sizeof(int));
  if (data != nullptr) {
    memcpy(pydata, data, size * sizeof(int));
  }
  PyObject* obj = PyArray_SimpleNewFromData(1, &size, NPY_INT32, pydata);
  PyArray_ENABLEFLAGS(reinterpret_cast<PyArrayObject*>(obj), NPY_ARRAY_OWNDATA);
  return obj;
}

PyObject* PyTupleFromQuantizationParam(const TfLiteQuantizationParams& param) {
  PyObject* result = PyTuple_New(2);
  PyTuple_SET_ITEM(result, 0, PyFloat_FromDouble(param.scale));
  PyTuple_SET_ITEM(result, 1, PyLong_FromLong(param.zero_point));
  return result;
}

PyObject* PyDictFromSparsityParam(const TfLiteSparsity& param) {
  PyObject* result = PyDict_New();
  PyDict_SetItemString(result, "traversal_order",
                       PyArrayFromIntVector(param.traversal_order->data,
                                            param.traversal_order->size));
  PyDict_SetItemString(
      result, "block_map",
      PyArrayFromIntVector(param.block_map->data, param.block_map->size));
  PyObject* dim_metadata = PyList_New(param.dim_metadata_size);
  for (int i = 0; i < param.dim_metadata_size; i++) {
    PyObject* dim_metadata_i = PyDict_New();
    if (param.dim_metadata[i].format == kTfLiteDimDense) {
      PyDict_SetItemString(dim_metadata_i, "format", PyLong_FromSize_t(0));
      PyDict_SetItemString(dim_metadata_i, "dense_size",
                           PyLong_FromSize_t(param.dim_metadata[i].dense_size));
    } else {
      PyDict_SetItemString(dim_metadata_i, "format", PyLong_FromSize_t(1));
      const auto* array_segments = param.dim_metadata[i].array_segments;
      const auto* array_indices = param.dim_metadata[i].array_indices;
      PyDict_SetItemString(
          dim_metadata_i, "array_segments",
          PyArrayFromIntVector(array_segments->data, array_segments->size));
      PyDict_SetItemString(
          dim_metadata_i, "array_indices",
          PyArrayFromIntVector(array_indices->data, array_indices->size));
    }
    PyList_SetItem(dim_metadata, i, dim_metadata_i);
  }
  PyDict_SetItemString(result, "dim_metadata", dim_metadata);
  return result;
}
}  // namespace
/*
 * We create both local and remote interpreters
 * So, destroy both
 *
 * The destructor has nothing to return, and as such
 * the remote will also not return any valid error state
 * in resp.status()
 *
 * We still try...catch for transport errors like
 * <id not found on remote>, or <send/recv error> etc
 */
InterpreterWrapper2::~InterpreterWrapper2() {
  delete lwrap;
  try{
    roundtrip(tflite_delete_wrapper, proxy.id);
  } catch(std::exception &e) {
  }
}

PyObject* InterpreterWrapper2::AllocateTensors(int subgraph_index) {
  if(local_exec)
    return lwrap->AllocateTensors(subgraph_index);
  try {
    roundtrip(tflite_allocate_tensors, proxy.id, subgraph_index);
    if(resp.status())
      throw resp.message();

    Py_RETURN_NONE;
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_RuntimeError, e.c_str());
  }
  return nullptr;
}

PyObject* InterpreterWrapper2::Invoke(int subgraph_index) {
  if(local_exec)
    return lwrap->Invoke(subgraph_index);
  try {
    roundtrip(tflite_invoke, proxy.id, subgraph_index);
    if(resp.status())
      throw resp.message();

    Py_RETURN_NONE;
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_RuntimeError, e.c_str());
  }
  return nullptr;
}

static constexpr int kBuiltinOpResolver = 1;
static constexpr int kBuiltinRefOpResolver = 2;
static constexpr int kBuiltinOpResolverWithoutDefaultDelegates = 3;

PyObject* InterpreterWrapper2::InputIndices() const {
  if(local_exec)
    return lwrap->InputIndices();
  try {
    roundtrip(tflite_input_indices, proxy.id);
    if(resp.status())
      throw resp.message();

    PyObject* np_array = PyArrayFromIntVector(resp.indices().data(), resp.indices().size());
    return PyArray_Return(reinterpret_cast<PyArrayObject*>(np_array));
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

PyObject* InterpreterWrapper2::OutputIndices() const {
  if(local_exec)
    return lwrap->OutputIndices();
  try {
    roundtrip(tflite_output_indices, proxy.id);
    if(resp.status())
      throw resp.message();

    PyObject* np_array = PyArrayFromIntVector(resp.indices().data(), resp.indices().size());
    return PyArray_Return(reinterpret_cast<PyArrayObject*>(np_array));
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

/*
 * Check the new shape array for validation, like it should have
 * one dimension, something like shape = (4,), and should have
 * dtype = "int" etc
 *
 * Taken from interpreter_wrapper.cc
 */
PyObject* InterpreterWrapper2::ResizeInputTensorImpl(int i, PyObject* value) {
  std::unique_ptr<PyObject, PyDecrefDeleter> array_safe(
      PyArray_FromAny(value, nullptr, 0, 0, NPY_ARRAY_CARRAY, nullptr));
  if (!array_safe) {
    PyErr_SetString(PyExc_ValueError,
                    "Failed to convert numpy value into readable tensor.");
    return nullptr;
  }

  PyArrayObject* array = reinterpret_cast<PyArrayObject*>(array_safe.get());

  if (PyArray_NDIM(array) != 1) {
    PyErr_Format(PyExc_ValueError, "Shape should be 1D instead of %d.",
                 PyArray_NDIM(array));
    return nullptr;
  }

  if (PyArray_TYPE(array) != NPY_INT32) {
    PyErr_Format(PyExc_ValueError, "Shape must be type int32 (was %d).",
                 PyArray_TYPE(array));
    return nullptr;
  }

  PyArray_ENABLEFLAGS(reinterpret_cast<PyArrayObject*>(array),
                      NPY_ARRAY_OWNDATA);
  return PyArray_Return(reinterpret_cast<PyArrayObject*>(array));
}

PyObject* InterpreterWrapper2::ResizeInputTensor(int i, PyObject* value,
                                                bool strict,
                                                int subgraph_index) {
  if(local_exec)
    return lwrap->ResizeInputTensor(i, value, subgraph_index, strict);
  
  PyArrayObject* array =
      reinterpret_cast<PyArrayObject*>(ResizeInputTensorImpl(i, value));
  if (array == nullptr) {
    return nullptr;
  }
  std::vector<int32_t> dims(PyArray_SHAPE(array)[0]);
  memcpy(dims.data(), PyArray_BYTES(array), dims.size() * sizeof(int32_t));

  try {
    roundtrip(tflite_resize_input_tensor, proxy.id, i, dims, strict ? 1 : 0, subgraph_index);
    if(resp.status())
      throw resp.message();

    Py_RETURN_NONE;
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_RuntimeError, e.c_str());
  }
  return nullptr;
}

/*
 * This returns an int, and never returns an error
 */
int InterpreterWrapper2::NumTensors(int subgraph_index) const {
  if(local_exec)
    return lwrap->NumTensors(subgraph_index);
  try {
    roundtrip(tflite_num_tensors, proxy.id);
    return resp.count();
  } catch(std::exception &e) {
    return 0;
  }
}

/*
 * This returns a string, and never returns an error
 */
std::string InterpreterWrapper2::TensorName(int i, int subgraph_index) const {
  if(local_exec)
    return lwrap->TensorName(i, subgraph_index);
  try {
    roundtrip(tflite_tensor_name, proxy.id, i);
    return resp.name();
  } catch(std::exception &e) {
    return "";
  }
}

/*
 * Get TfLiteType from remote and convert to Numpy Type class 
 */
PyObject* InterpreterWrapper2::TensorType(int i, int subgraph_index) const {
  if(local_exec)
    return lwrap->TensorType(i, subgraph_index);
  try {
    roundtrip(tflite_tensor_type, proxy.id, i);
    if(resp.status())
      throw resp.message();

    int code = python_utils::TfLiteTypeToPyArrayType(static_cast<TfLiteType>(resp.tensor_type()));
    if (code == -1)
      throw std::string("Invalid tflite type code ") + std::to_string(code);

    return PyArray_TypeObjectFromType(code);
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

PyObject* InterpreterWrapper2::TensorSize(int i, int subgraph_index) const {
  if(local_exec)
    return lwrap->TensorSize(i, subgraph_index);
  try {
    roundtrip(tflite_tensor_size, proxy.id, i);
    if(resp.status())
      throw resp.message();

    PyObject* np_array = PyArrayFromIntVector(resp.tensor_size().data(), resp.tensor_size().size());
    return PyArray_Return(reinterpret_cast<PyArrayObject*>(np_array));
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

PyObject* InterpreterWrapper2::TensorSizeSignature(int i, int subgraph_index) const {
  if(local_exec)
    return lwrap->TensorSizeSignature(i, subgraph_index);
  try {
    roundtrip(tflite_tensor_size_signature, proxy.id, i);
    if(resp.status())
      throw resp.message();

    PyObject* np_array = PyArrayFromIntVector(resp.tensor_size().data(), resp.tensor_size().size());
    return PyArray_Return(reinterpret_cast<PyArrayObject*>(np_array));
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

/*
 * Use <PyDictFromSparsityParam> from interpreter_wrapper.cc
 * and therefore convert out response to a TfLiteSparsity struct
 */
PyObject* InterpreterWrapper2::TensorSparsityParameters(int i, int subgraph_index) const {
  if(local_exec)
    return lwrap->TensorSparsityParameters(i, subgraph_index);
  try {
    roundtrip(tflite_tensor_sparsity_parameters, proxy.id, i);
    if(resp.status())
      throw resp.message();

    if(!resp.has_sparsity())
      return PyDict_New();
  
    auto vec2IntArray = [](std::vector<int> v) {
      TfLiteIntArray *ret = static_cast<TfLiteIntArray *>(malloc(sizeof(int) * v.size() + sizeof(int)));
      ret->size = v.size();
      std::copy(v.begin(), v.end(), &ret->data[0]);
      return std::unique_ptr<TfLiteIntArray, decltype(&free)>(ret, free);
    };
    std::unique_ptr<TfLiteIntArray, decltype(&free)> traversal_order = vec2IntArray(resp.traversal_order());
    std::unique_ptr<TfLiteIntArray, decltype(&free)> block_map = vec2IntArray(resp.block_map());
    std::unique_ptr<TfLiteDimensionMetadata[]> dim_metadata = std::make_unique<TfLiteDimensionMetadata[]>(resp.dim_metadata().size());
    std::vector<std::unique_ptr<TfLiteIntArray, decltype(&free)>> dummy_arrseg, dummy_arrind;

    TfLiteSparsity sparsity;
    sparsity.traversal_order = traversal_order.get();
    sparsity.block_map = block_map.get();
    sparsity.dim_metadata_size = resp.dim_metadata().size();
    sparsity.dim_metadata = dim_metadata.get();
    for(unsigned int i = 0; i < resp.dim_metadata().size(); i++) {
      auto elem = resp.dim_metadata()[i];
      sparsity.dim_metadata[i].format = static_cast<TfLiteDimensionType>(std::get<0>(elem));
      sparsity.dim_metadata[i].dense_size = std::get<1>(elem);
      
      // a small trick to get all of these inside loop and still
      // letting them stay on scope for unique_ptr to work
      std::unique_ptr<TfLiteIntArray, decltype(&free)> aseg = vec2IntArray(std::get<2>(elem));
      dummy_arrseg.push_back(std::move(aseg));
      sparsity.dim_metadata[i].array_segments = aseg.get();

      // same as above
      std::unique_ptr<TfLiteIntArray, decltype(&free)> aind = vec2IntArray(std::get<3>(elem));
      dummy_arrind.push_back(std::move(aind));
      sparsity.dim_metadata[i].array_indices = aind.get();
    }

    return PyDictFromSparsityParam(sparsity);
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

PyObject* InterpreterWrapper2::TensorQuantization(int i, int subgraph_index) const {
  if(local_exec)
    return lwrap->TensorQuantization(i, subgraph_index);
  try {
    roundtrip(tflite_tensor_quantization, proxy.id, i);
    if(resp.status())
      throw resp.message();

    TfLiteQuantizationParams params{resp.scale(), resp.zp()};
    return PyTupleFromQuantizationParam(params);
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

PyObject* InterpreterWrapper2::TensorQuantizationParameters(int i, int subgraph_index) const {
  if(local_exec)
    return lwrap->TensorQuantizationParameters(i, subgraph_index);
  try {
    roundtrip(tflite_tensor_quantization_parameters, proxy.id, i);
    if(resp.status())
      throw resp.message();

    PyObject* scales_array = PyArrayFromFloatVector(resp.scales().data(), resp.scales().size());
    PyObject* zero_points_array = PyArrayFromIntVector(resp.zps().data(), resp.zps().size());

    PyObject* result = PyTuple_New(3);
    PyTuple_SET_ITEM(result, 0, scales_array);
    PyTuple_SET_ITEM(result, 1, zero_points_array);
    PyTuple_SET_ITEM(result, 2, PyLong_FromLong(resp.dim()));
    return result;
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

/*
 * TODO: See what needs to be done for kTfLiteString
 * currenly, the server will send an error if tensor->type
 * is kTfLiteString
 */
PyObject* InterpreterWrapper2::SetTensor(int i, PyObject* value, int subgraph_index) {
  if(local_exec)
    return lwrap->SetTensor(i, value, subgraph_index);
  std::unique_ptr<PyObject, PyDecrefDeleter> array_safe(
      PyArray_FromAny(value, nullptr, 0, 0, NPY_ARRAY_CARRAY, nullptr));
  if (!array_safe) {
    PyErr_SetString(PyExc_ValueError,
                    "Failed to convert value into readable tensor.");
    return nullptr;
  }

  PyArrayObject* array = reinterpret_cast<PyArrayObject*>(array_safe.get());
  TfLiteType type = python_utils::TfLiteTypeFromPyArray(array);
  std::vector<int64_t> dims = std::vector<int64_t>(PyArray_SHAPE(array), PyArray_SHAPE(array) + PyArray_NDIM(array));
  try {
      roundtrip(tflite_set_tensor, proxy.id, i, type, dims, std::vector<uint8_t>(reinterpret_cast<uint8_t *>(PyArray_DATA(array)),
                  reinterpret_cast<uint8_t *>(PyArray_DATA(array)) + PyArray_NBYTES(array)),subgraph_index);
      if(resp.status())
          throw resp.message();
      Py_RETURN_NONE;
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

/*
 * This returns an int, and never returns an error
 */
int InterpreterWrapper2::NumNodes() const {
  if(local_exec)
    return lwrap->NumNodes();
  try {
    roundtrip(tflite_num_nodes, proxy.id);
    return resp.count();
  } catch(std::exception &e) {
    return 0;
  }
}

PyObject* InterpreterWrapper2::NodeInputs(int i) const {
  if(local_exec)
    return lwrap->NodeInputs(i);
  try {
    roundtrip(tflite_node_inputs, proxy.id, i);
    if(resp.status())
      throw resp.message();

    PyObject* inputs = PyArrayFromIntVector(resp.inputs().data(), resp.inputs().size());
    return inputs;
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

PyObject* InterpreterWrapper2::NodeOutputs(int i) const {
  if(local_exec)
    return lwrap->NodeOutputs(i);
  try {
    roundtrip(tflite_node_outputs, proxy.id, i);
    if(resp.status())
      throw resp.message();

    PyObject* outputs = PyArrayFromIntVector(resp.outputs().data(), resp.outputs().size());
    return outputs;
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

/*
 * This returns a string, and never returns an error
 */
std::string InterpreterWrapper2::NodeName(int i) const {
  if(local_exec)
    return lwrap->NodeName(i);
  try {
    roundtrip(tflite_node_name, proxy.id, i);
    return resp.name();
  } catch(std::exception &e) {
    return "";
  }
}

/*
 * TODO: See what needs to be done for kTfLiteString
 * currenly, the server will send an error if tensor->type
 * is kTfLiteString
 */
PyObject* InterpreterWrapper2::GetTensor(int i, int subgraph_index) const {
  if(local_exec)
    return lwrap->GetTensor(i,subgraph_index);
  try {
    roundtrip(tflite_get_tensor, proxy.id, i, subgraph_index);
    if(resp.status())
      throw resp.message();

    if(python_utils::TfLiteTypeToPyArrayType(static_cast<TfLiteType>(resp.data_type())) == -1)
      throw "Unknown tensor type.";

    void* data = malloc(resp.data().size());
    if (!data)
      throw "Malloc to copy tensor failed.";
    memcpy(data, resp.data().data(), resp.data().size());

    PyObject* np_array;
    np_array = PyArray_SimpleNewFromData(resp.data_shape().size(), (npy_intp*)(resp.data_shape().data()),
            python_utils::TfLiteTypeToPyArrayType(static_cast<TfLiteType>(resp.data_type())), data);
      
    PyArray_ENABLEFLAGS(reinterpret_cast<PyArrayObject*>(np_array),
                        NPY_ARRAY_OWNDATA);
    return PyArray_Return(reinterpret_cast<PyArrayObject*>(np_array));
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_RuntimeError, e.c_str());
  }
  return nullptr;
}

InterpreterWrapper2* InterpreterWrapper2::CreateWrapperCPPFromFile(
    const char* model_path, int op_resolver_id, const std::vector<std::string>& registerers_by_name,
    const std::vector<std::function<void(uintptr_t)>>& registerers_by_func,
    std::string* error_msg, bool preserve_all_tensors) {
  if (registerers_by_name.size() || registerers_by_func.size()) {
    *error_msg = "custom_op_registerers can not be used on this setup";
    return nullptr;
  }
  InterpreterWrapper *wrap = InterpreterWrapper::CreateWrapperCPPFromFile(model_path, op_resolver_id, registerers_by_name,
    registerers_by_func, error_msg, preserve_all_tensors);
  if(!wrap)
    return nullptr;

  try {
    std::string canonical_path = std::experimental::filesystem::canonical(model_path);
    send_file(canonical_path.c_str());
    
    roundtrip(tflite_create_wrapper_from_file, canonical_path);
    if(resp.status())
      throw resp.message();

    return new InterpreterWrapper2(wrap, resp.id());
  } catch(std::exception &e) {
    *error_msg = "internal error";
  } catch(std::string &e) {
    *error_msg = e;
  }
  return nullptr;
}

InterpreterWrapper2* InterpreterWrapper2::CreateWrapperCPPFromFile(
    const char* model_path, int op_resolver_id, const std::vector<std::string>& registerers,
    std::string* error_msg, bool preserve_all_tensors) {
  return CreateWrapperCPPFromFile(model_path, op_resolver_id, registerers, {}, error_msg, preserve_all_tensors);
}

InterpreterWrapper2* InterpreterWrapper2::CreateWrapperCPPFromBuffer(
    PyObject* data, int op_resolver_id, const std::vector<std::string>& registerers_by_name,
    const std::vector<std::function<void(uintptr_t)>>& registerers_by_func,
    std::string* error_msg, bool preserve_all_tensors) {
  if (registerers_by_name.size() || registerers_by_func.size()) {
    *error_msg = "custom_op_registerers can not be used on this setup";
    return nullptr;
  }
  InterpreterWrapper *wrap = InterpreterWrapper::CreateWrapperCPPFromBuffer(data, op_resolver_id, registerers_by_name,
    registerers_by_func, error_msg, preserve_all_tensors);
  if(!wrap)
    return nullptr;

  try {
    uint8_t* buffer = nullptr;
    Py_ssize_t length;
    if (python_utils::ConvertFromPyString(data, reinterpret_cast<char **>(&buffer), &length) == -1)
      throw "ConvertFromPyString failed";

    /* TODO: progress bar */
    roundtrip(tflite_create_wrapper_from_buffer, std::vector<uint8_t>(buffer, buffer + length));
    if(resp.status())
      throw resp.message();

    return new InterpreterWrapper2(wrap, resp.id());
  } catch(std::exception &e) {
    *error_msg = "internal error";
  } catch(std::string &e) {
    *error_msg = e;
  }
  return nullptr;
}

InterpreterWrapper2* InterpreterWrapper2::CreateWrapperCPPFromBuffer(
    PyObject* data, int op_resolver_id, const std::vector<std::string>& registerers,
    std::string* error_msg, bool preserve_all_tensors) {
  return CreateWrapperCPPFromBuffer(data, op_resolver_id, registerers, {}, error_msg, preserve_all_tensors);
}

PyObject* InterpreterWrapper2::ResetVariableTensors() {
  if(local_exec)
    return lwrap->ResetVariableTensors();
  try {
    roundtrip(tflite_reset_variable_tensors, proxy.id);
    if(resp.status())
      throw resp.message();

    Py_RETURN_NONE;
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_RuntimeError, e.c_str());
  }
  return nullptr;
}

/*
 * TODO: I have a trick for this, but would like
 * to see if anyone wants to use this API on cloud
 * interface
 */
PyObject* InterpreterWrapper2::tensor(PyObject* base_object, int i, int subgraph_index) {
  PyErr_Format(PyExc_NotImplementedError, "tensor() cannot be called in this setup");
  return nullptr;
}

/* Unless the user changes the code in interpreter.py, it calls __init__ as
 * __init():
 *     w = createWrapper(...)
 *     w.SetNumThreads(...)
 *     w.ModifyGraphWithDelegate(...)
 *
 * So this API is a bit special, as it gets called before the delegate
 * is decided and we dont know whether the corresponding function on
 * local or remote interpreter be called.
 *
 * What we do know is that the <local_exec> variable is false at this moment
 * and in future if the "compile" delegate is loaded, it will run single-threaded
 *
 * Well, no harm there, but then lets call both local and remote so that the
 * "compiler" delegate also runs multi-threaded if the user wants so
 */
PyObject* InterpreterWrapper2::SetNumThreads(int num_threads) {
  PyObject *lret = lwrap->SetNumThreads(num_threads);
  if(!lret)
    return lret;

  try {
    roundtrip(tflite_set_num_threads, proxy.id, num_threads);
    if(resp.status())
      throw resp.message();

    Py_RETURN_NONE;
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

/* We try to extract the name of the library that was used
 * as delegate
 *
 * We will then use this info to validate whether the user
 * has used a TI provided delegate or not, and whether
 * it is the compile delegate or the exec delegate
 *
 * If it is the compile delegate, we stay within the
 * container and use the (delegate *) directly
 *
 * If the exec delegate, it is not the real exec delegate :)
 * It is a cooked up delegate that has already loaded the real
 * delegate on the target and obtained its pointer in data_
 *
 * So we pass the pointer back to the target where the server
 * loads the delegate by calling ModifyGraphWithDelegate()
 */
PyObject* InterpreterWrapper2::ModifyGraphWithDelegate(
    TfLiteDelegate* delegate) {
  Dl_info info;
  std::string libname;
  int res;

  res = dladdr(reinterpret_cast<void *>(delegate->Prepare), &info);
  if(!res || !info.dli_fname)
    goto error;
  libname = std::string(basename(info.dli_fname));

  if(libname == "tidl_model_import_tflite.so") {
    local_exec = true;
    return lwrap->ModifyGraphWithDelegate(delegate);
  } else if(libname == "libtidl_tfl_delegate.so") {
      try {
          roundtrip(tflite_load_exec_delegate, proxy.id, reinterpret_cast<uint64_t>(delegate->data_));
          if(resp.status())
              throw resp.message();

          Py_RETURN_NONE;
      } catch(std::exception &e) {
          PyErr_Format(PyExc_RuntimeError, "internal error");
      } catch(std::string &e) {
          PyErr_Format(PyExc_ValueError, e.c_str());
      }
      return nullptr;
  }

error:
  PyErr_Format(PyExc_NotImplementedError, "only prebuilt delegates can be used on this setup");
  return nullptr;
}

/* TODO: lets see how people use this API to take a call */
Interpreter* InterpreterWrapper2::interpreter() {
  PyErr_Format(PyExc_NotImplementedError, "interpreter() cannot be called on this setup");
  return nullptr;
}

PyObject* InterpreterWrapper2::get_TI_benchmark_data(int subgraph_index)  {
  if(local_exec)
    return lwrap->get_TI_benchmark_data(subgraph_index);
  try {
      roundtrip(tflite_get_ti_benchmark_data, proxy.id, subgraph_index);
      if(resp.status())
          throw resp.message();

      PyObject* result = PyDict_New();
      for (auto e : resp.benchmark_data())
          PyDict_SetItemString(result, e.first.c_str(), PyLong_FromUnsignedLongLong(e.second));
      return result;
  } catch(std::exception &e) {
    PyErr_Format(PyExc_RuntimeError, "internal error");
  } catch(std::string &e) {
    PyErr_Format(PyExc_ValueError, e.c_str());
  }
  return nullptr;
}

}  // namespace interpreter_wrapper
}  // namespace tflite
