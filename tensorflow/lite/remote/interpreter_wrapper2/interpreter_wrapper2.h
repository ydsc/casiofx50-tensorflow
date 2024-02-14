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
#ifndef TENSORFLOW_LITE_PYTHON_INTERPRETER_WRAPPER_INTERPRETER_WRAPPER2_H_
#define TENSORFLOW_LITE_PYTHON_INTERPRETER_WRAPPER_INTERPRETER_WRAPPER2_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

// Place `<locale>` before <Python.h> to avoid build failures in macOS.
#include <locale>

// The empty line above is on purpose as otherwise clang-format will
// automatically move <Python.h> before <locale>.
#include <Python.h>

#include "tensorflow/lite/python/interpreter_wrapper/interpreter_wrapper.h"

struct TfLiteDelegate;

// We forward declare TFLite classes here to avoid exposing them to SWIG.
namespace tflite {
namespace interpreter_wrapper {

class proxy_model {
public:
    proxy_model(uint32_t _id): id(_id) {}

    uint32_t id;
    std::unique_ptr<const char *[]> annotations;
    std::unique_ptr<uint64_t[]> values;
};

class InterpreterWrapper2 {
 public:

  // SWIG caller takes ownership of pointer.
  static InterpreterWrapper2* CreateWrapperCPPFromFile(
      const char* model_path, int op_resolver_id,
      const std::vector<std::string>& registerers, std::string* error_msg,
      bool preserve_all_tensors);
  static InterpreterWrapper2* CreateWrapperCPPFromFile(
      const char* model_path, int op_resolver_id,
      const std::vector<std::string>& registerers_by_name,
      const std::vector<std::function<void(uintptr_t)>>& registerers_by_func,
      std::string* error_msg, bool preserve_all_tensors);

  // SWIG caller takes ownership of pointer.
  static InterpreterWrapper2* CreateWrapperCPPFromBuffer(
      PyObject* data, int op_resolver_id,
      const std::vector<std::string>& registerers, std::string* error_msg,
      bool preserve_all_tensors);
  static InterpreterWrapper2* CreateWrapperCPPFromBuffer(
      PyObject* data, int op_resolver_id,
      const std::vector<std::string>& registerers_by_name,
      const std::vector<std::function<void(uintptr_t)>>& registerers_by_func,
      std::string* error_msg, bool preserve_all_tensors);

  ~InterpreterWrapper2();
  PyObject* AllocateTensors(int subgraph_index);
  PyObject* Invoke(int subgraph_index);

  PyObject* InputIndices() const;
  PyObject* OutputIndices() const;
  PyObject* ResizeInputTensor(int i, PyObject* value, bool strict,
                              int subgraph_index);

  int NumTensors(int subgraph_index) const;
  std::string TensorName(int i, int subgraph_index) const;
  PyObject* TensorType(int i, int subgraph_index) const;
  PyObject* TensorSize(int i, int subgraph_index) const;
  PyObject* TensorSizeSignature(int i, int subgraph_index) const;
  PyObject* TensorSparsityParameters(int i, int subgraph_index) const;
  // Deprecated in favor of TensorQuantizationScales, below.
  PyObject* TensorQuantization(int i, int subgraph_index) const;
  PyObject* TensorQuantizationParameters(int i, int subgraph_index) const;
  PyObject* SetTensor(int i, PyObject* value, int subgraph_index);
  PyObject* GetTensor(int i,int subgraph_index) const;
  PyObject* ResetVariableTensors();

  int NumNodes() const;
  std::string NodeName(int i) const;
  PyObject* NodeInputs(int i) const;
  PyObject* NodeOutputs(int i) const;

  // Returns a reference to tensor index i as a numpy array. The base_object
  // should be the interpreter object providing the memory.
  PyObject* tensor(PyObject* base_object, int i, int subgraph_index);

  PyObject* SetNumThreads(int num_threads);

  // Adds a delegate to the interpreter.
  PyObject* ModifyGraphWithDelegate(TfLiteDelegate* delegate);

  // Experimental and subject to change.
  //
  // Returns a pointer to the underlying interpreter.
  Interpreter* interpreter();

  PyObject* get_TI_benchmark_data(int subgraph_index);

private:
  // Helper function to resize an input tensor.
  PyObject* ResizeInputTensorImpl(int i, PyObject* value);

  InterpreterWrapper2(InterpreterWrapper *wrap, uint32_t id) : lwrap(wrap), proxy(id), local_exec(false) {}
  InterpreterWrapper2();
  InterpreterWrapper2(const InterpreterWrapper2& rhs);

  InterpreterWrapper *lwrap;
  proxy_model proxy;
  bool local_exec;
};

}  // namespace interpreter_wrapper
}  // namespace tflite

#endif  // TENSORFLOW_LITE_PYTHON_INTERPRETER_WRAPPER_INTERPRETER_WRAPPER_H_
