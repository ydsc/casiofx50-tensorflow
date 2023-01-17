#include <set>
#include <map>
#include <sstream>
#include <string>
#include <memory>
#include "absl/memory/memory.h"
#include "tensorflow/lite/stateful_error_reporter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/util.h"
//#include "tensorflow/lite/python/interpreter_wrapper/interpreter_wrapper.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model.h"

#include "tflite_messages.h"

#define TFLITE_TENSOR_BOUNDS_CHECK(i) \
  if ((i) >= wrap->interpreter_->tensors_size() || (i) < 0) { \
    p->m_message = std::string("Invalid tensor index ") + \
        std::to_string((i)) + \
        " exceeds max tensor index " + \
        std::to_string(wrap->interpreter_->tensors_size()); \
    p->m_status = -1; \
    return p; \
  }

#define TFLITE_NODES_BOUNDS_CHECK(i) \
  if ((i) >= wrap->interpreter_->nodes_size() || (i) < 0) { \
    p->m_message = "Invalid node index"; \
    p->m_status = -1; \
    return p; \
  }

#define TFLITE_CHECK(x) \
  if ((x) != kTfLiteOk) { \
    p->m_message = wrap->error_reporter_->message(); \
    p->m_status = -1; \
    return p; \
  }

#define TFLITE_SUBGRAPH_TENSOR_BOUNDS_CHECK(i, subgraph_index)             \
  if (i >= wrap->interpreter_->subgraph(subgraph_index)->tensors_size() || i < 0) { \
    p->m_message = std::string("Invalid tensor index ") + \
        std::to_string((i)) + \
        " exceeds max tensor index " + \
        std::to_string(wrap->interpreter_->subgraph(subgraph_index)->tensors_size()); \
    p->m_status = -1; \
    return p; \
  }

#define TFLITE_SUBGRAPH_BOUNDS_CHECK(i)                                   \
  if (i >= wrap->interpreter_->subgraphs_size() || i < 0) {                        \
    p->m_message = std::string("Invalid subgraph index ") + \
        std::to_string((i)) + \
        " exceeds max subgraph index " + \
        std::to_string(wrap->interpreter_->subgraphs_size()); \
    p->m_status = -1; \
    return p; \
  }

template <class T>
class idr {
public:
    int add(T item);
    T remove(int id);
    T find(int id);
private:
    std::set<int> m_allocated_ids;
    std::set<int> m_free_ids;
    std::map<int, T> m_models;
};

template <class T>
int idr<T>::add(T item)
{
    int id;

    if(!m_free_ids.empty()) {
        std::set<int>::iterator it = m_free_ids.begin();
        id = *it;
        m_free_ids.erase(it);
    } else if(!m_allocated_ids.empty()) {
        std::set<int>::reverse_iterator it = m_allocated_ids.rbegin();
        id = *it + 1;
    } else
        id = 0;

    if(m_models.find(id) != m_models.end() ||
            m_allocated_ids.find(id) != m_allocated_ids.end() ||
            m_free_ids.find(id) != m_free_ids.end()) {
        ERROR("Exception: idr allocation failure for id = %d", id); 
    }

    m_allocated_ids.insert(id);
    m_models[id] = item;
    return id;
}

template <class T>
T idr<T>::remove(int id)
{
    T item;

    if(m_models.find(id) == m_models.end() ||
            m_allocated_ids.find(id) == m_allocated_ids.end() ||
            m_free_ids.find(id) != m_free_ids.end()) {
        ERROR("Exception: failed to remove id = %d, not allocated", id); 
    }

    item = m_models[id];
    m_models.erase(m_models.find(id));
    m_allocated_ids.erase(m_allocated_ids.find(id));
    m_free_ids.insert(id);

    return item;
}

template <class T>
T idr<T>::find(int id)
{
    T item;

    if(m_models.find(id) == m_models.end() ||
            m_allocated_ids.find(id) == m_allocated_ids.end() ||
            m_free_ids.find(id) != m_free_ids.end()) {
        ERROR("Exception: could not find id = %d", id); 
    }

    item = m_models[id];

    return item;
}


class IEErrorReporter : public tflite::StatefulErrorReporter {
 public:
  IEErrorReporter() {}

  // Report an error message
  int Report(const char* format, va_list args) override {
    int formatted = vsnprintf(NULL, 0, format, args);
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(formatted + 1);
    vsnprintf(buf.get(), formatted + 1, format, args);
    buffer_ << buf.get();
    return formatted;
  }

  // Gets the last error message and clears the buffer.
  std::string message() override {
    std::string value = buffer_.str();
    buffer_.clear();
    return value;
  }

 private:
  std::stringstream buffer_;
};

class IEWrapper {
public:
  IEWrapper(
  std::unique_ptr<tflite::FlatBufferModel> model,
  std::unique_ptr<tflite::Interpreter> interpreter,
  std::unique_ptr<tflite::ops::builtin::BuiltinOpResolver> resolver,
  std::unique_ptr<IEErrorReporter> error_reporter)
  : model_(std::move(model)),
    error_reporter_(std::move(error_reporter)),
    resolver_(std::move(resolver)),
    interpreter_(std::move(interpreter)) {}

  const std::unique_ptr<tflite::FlatBufferModel> model_;
  const std::unique_ptr<IEErrorReporter> error_reporter_;
  const std::unique_ptr<tflite::ops::builtin::BuiltinOpResolver> resolver_;
  const std::unique_ptr<tflite::Interpreter> interpreter_;
};

idr<IEWrapper *> model_idr;

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_create_wrapper_from_file);
  auto resolver = absl::make_unique<tflite::ops::builtin::BuiltinOpResolver>();
  std::unique_ptr<tflite::Interpreter> interpreter;
  std::unique_ptr<IEErrorReporter> error_reporter(new IEErrorReporter);
  std::unique_ptr<tflite::FlatBufferModel> model =
      tflite::FlatBufferModel::BuildFromFile((std::string(BASE) + m_model_path).c_str(),
                                                        error_reporter.get());
  if(!model) {
    p->m_status = -1;
    p->m_message = error_reporter->message();
    return p;
  }

  if (tflite::InterpreterBuilder(
          *model, *resolver)(&interpreter) != kTfLiteOk) {
    p->m_status = -1;
    p->m_message = error_reporter->message();
    return p;
  }

  p->m_id = model_idr.add(new IEWrapper(std::move(model), std::move(interpreter),
              std::move(resolver), std::move(error_reporter)));
  p->m_status = 0;
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_create_wrapper_from_buffer);
  auto resolver = absl::make_unique<tflite::ops::builtin::BuiltinOpResolver>();
  std::unique_ptr<tflite::Interpreter> interpreter;
  std::unique_ptr<IEErrorReporter> error_reporter(new IEErrorReporter);
  std::unique_ptr<tflite::FlatBufferModel> model =
      tflite::FlatBufferModel::BuildFromBuffer(reinterpret_cast<const char *>(m_model_data.data()), m_model_data.size(),
                                                          error_reporter.get());
  if(!model) {
    p->m_status = -1;
    p->m_message = error_reporter->message();
    return p;
  }

  if (tflite::InterpreterBuilder(
          *model, *resolver)(&interpreter) != kTfLiteOk) {
    p->m_status = -1;
    p->m_message = error_reporter->message();
    return p;
  }

  p->m_id = model_idr.add(new IEWrapper(std::move(model), std::move(interpreter),
              std::move(resolver), std::move(error_reporter)));
  p->m_status = 0;
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_delete_wrapper);
    auto wrap = model_idr.remove(m_id);
    delete wrap;

    p->m_status = 0;
    return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_num_tensors);
    auto wrap = model_idr.find(m_id);

    p->m_count = wrap->interpreter_->tensors_size();
    return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_tensor_name);
  auto wrap = model_idr.find(m_id);

  if (m_tensor_id >= wrap->interpreter_->tensors_size() || m_tensor_id < 0)
    return p;

  const TfLiteTensor* tensor = wrap->interpreter_->tensor(m_tensor_id);
  p->m_name = tensor->name ? tensor->name : "";
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_tensor_type);
  auto wrap = model_idr.find(m_id);
  TFLITE_TENSOR_BOUNDS_CHECK(m_tensor_id);

  const TfLiteTensor* tensor = wrap->interpreter_->tensor(m_tensor_id);
  if (tensor->type == kTfLiteNoType) {
    p->m_message = "Tensor with no type found.";
    p->m_status = -1;
    return p;
  }

  p->m_status = 0;
  p->m_tensor_type = tensor->type;
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_tensor_size);
  auto wrap = model_idr.find(m_id);
  TFLITE_TENSOR_BOUNDS_CHECK(m_tensor_id);

  const TfLiteTensor* tensor = wrap->interpreter_->tensor(m_tensor_id);
  if (tensor->dims == nullptr) {
    p->m_message = "Tensor with no shape found.";
    p->m_status = -1;
    return p;
  }
  p->m_status = 0;
  p->m_tensor_size = std::vector<int32_t>(tensor->dims->data, tensor->dims->data + tensor->dims->size);
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_tensor_size_signature);
  auto wrap = model_idr.find(m_id);
  TFLITE_TENSOR_BOUNDS_CHECK(m_tensor_id);

  const TfLiteTensor* tensor = wrap->interpreter_->tensor(m_tensor_id);
  const int32_t* size_signature_data = nullptr;
  int32_t size_signature_size = 0;
  if (tensor->dims_signature != nullptr && tensor->dims_signature->size != 0) {
    size_signature_data = tensor->dims_signature->data;
    size_signature_size = tensor->dims_signature->size;
  } else {
    size_signature_data = tensor->dims->data;
    size_signature_size = tensor->dims->size;
  }
  p->m_status = 0;
  p->m_tensor_size = std::vector<int32_t>(size_signature_data, size_signature_data + size_signature_size);
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_tensor_quantization);
  auto wrap = model_idr.find(m_id);
  TFLITE_TENSOR_BOUNDS_CHECK(m_tensor_id);

  const TfLiteTensor* tensor = wrap->interpreter_->tensor(m_tensor_id);
  p->m_status = 0;
  p->m_scale = tensor->params.scale;
  p->m_zp = tensor->params.zero_point;
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_tensor_quantization_parameters);
  auto wrap = model_idr.find(m_id);
  TFLITE_TENSOR_BOUNDS_CHECK(m_tensor_id);

  const TfLiteTensor* tensor = wrap->interpreter_->tensor(m_tensor_id);
  const TfLiteQuantization quantization = tensor->quantization;
  float* scales_data = nullptr;
  int32_t* zero_points_data = nullptr;
  int32_t scales_size = 0;
  int32_t zero_points_size = 0;
  int32_t quantized_dimension = 0;
  if (quantization.type == kTfLiteAffineQuantization) {
    const TfLiteAffineQuantization* q_params =
        reinterpret_cast<const TfLiteAffineQuantization*>(quantization.params);
    if (q_params->scale) {
      scales_data = q_params->scale->data;
      scales_size = q_params->scale->size;
    }
    if (q_params->zero_point) {
      zero_points_data = q_params->zero_point->data;
      zero_points_size = q_params->zero_point->size;
    }
    quantized_dimension = q_params->quantized_dimension;
  }

  p->m_status = 0;
  p->m_scales = std::vector<float>(scales_data, scales_data + scales_size);
  p->m_zps = std::vector<int32_t>(zero_points_data, zero_points_data + zero_points_size);
  p->m_dim = quantized_dimension;
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_tensor_sparsity_parameters);
  auto wrap = model_idr.find(m_id);
  TFLITE_TENSOR_BOUNDS_CHECK(m_tensor_id);

  const TfLiteTensor* tensor = wrap->interpreter_->tensor(m_tensor_id);
  p->m_status = 0;
  if (tensor->sparsity == nullptr) {
    p->m_has_sparsity = 0;
    return p;
  }

  p->m_traversal_order = std::vector<int32_t>(tensor->sparsity->traversal_order->data,
          tensor->sparsity->traversal_order->data + tensor->sparsity->traversal_order->size); 
  p->m_block_map = std::vector<int32_t>(tensor->sparsity->block_map->data,
          tensor->sparsity->block_map->data + tensor->sparsity->block_map->size); 
  for (int i = 0; i < tensor->sparsity->dim_metadata_size; i++) {
    if (tensor->sparsity->dim_metadata[i].format == kTfLiteDimDense) {
      p->m_dim_metadata.push_back(
              std::make_tuple<uint32_t, int32_t, std::vector<int32_t>, std::vector<int32_t>>(
                  0, std::move(tensor->sparsity->dim_metadata[i].dense_size), {}, {}));
    } else {
      p->m_dim_metadata.push_back(
              std::make_tuple<uint32_t, int32_t, std::vector<int32_t>, std::vector<int32_t>>(
                  1, 0,
                  std::vector<int32_t>(tensor->sparsity->dim_metadata[i].array_segments->data,
                      tensor->sparsity->dim_metadata[i].array_segments->data + tensor->sparsity->dim_metadata[i].array_segments->size),
                  std::vector<int32_t>(tensor->sparsity->dim_metadata[i].array_indices->data,
                      tensor->sparsity->dim_metadata[i].array_indices->data + tensor->sparsity->dim_metadata[i].array_indices->size)));
    }
  }
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_num_nodes);
    auto wrap = model_idr.find(m_id);

    p->m_count = wrap->interpreter_->nodes_size();
    return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_node_name);
  auto wrap = model_idr.find(m_id);
  if (m_node_id >= wrap->interpreter_->nodes_size() || m_node_id < 0)
    return p;

  const TfLiteRegistration* node_registration =
      &(wrap->interpreter_->node_and_registration(m_node_id)->second);
  int32_t op_code = node_registration->builtin_code;
  std::string op_name;
  if (op_code == tflite::BuiltinOperator_CUSTOM) {
    const char* custom_name = node_registration->custom_name;
    op_name = custom_name ? custom_name : "UnknownCustomOp";
  } else {
    op_name = tflite::EnumNamesBuiltinOperator()[op_code];
  }
  p->m_name = op_name;
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_node_inputs);
  auto wrap = model_idr.find(m_id);
  TFLITE_NODES_BOUNDS_CHECK(m_node_id);

  const TfLiteNode* node = &(wrap->interpreter_->node_and_registration(m_node_id)->first);
  p->m_inputs = std::vector<int32_t>(node->inputs->data, node->inputs->data + node->inputs->size);
  p->m_status = 0;
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_node_outputs);
  auto wrap = model_idr.find(m_id);
  TFLITE_NODES_BOUNDS_CHECK(m_node_id);

  const TfLiteNode* node = &(wrap->interpreter_->node_and_registration(m_node_id)->first);
  p->m_outputs = std::vector<int32_t>(node->outputs->data, node->outputs->data + node->outputs->size);
  p->m_status = 0;
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_allocate_tensors);
    auto wrap = model_idr.find(m_id);
    static constexpr int kUndeterminedSubgraphIndex = -1;
    if (m_subgraph_index == kUndeterminedSubgraphIndex) {
      TFLITE_CHECK(wrap->interpreter_->AllocateTensors())
    } else {
      TFLITE_SUBGRAPH_BOUNDS_CHECK(m_subgraph_index);
      TFLITE_CHECK(wrap->interpreter_->subgraph(m_subgraph_index)->AllocateTensors());
  }
    p->m_status = 0;
    return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_input_indices);
    auto wrap = model_idr.find(m_id);

    p->m_status = 0;
    p->m_indices = wrap->interpreter_->inputs();
    return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_output_indices);
    auto wrap = model_idr.find(m_id);

    p->m_status = 0;
    p->m_indices = wrap->interpreter_->outputs();
    return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_reset_variable_tensors);
    auto wrap = model_idr.find(m_id);

    TFLITE_CHECK(wrap->interpreter_->ResetVariableTensors());
    p->m_status = 0;
    return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_resize_input_tensor);
  auto wrap = model_idr.find(m_id);

  if (m_strict) {
    TFLITE_CHECK(wrap->interpreter_->subgraph(m_subgraph_index)->ResizeInputTensorStrict(m_tensor_id, m_dims));
  } else {
    TFLITE_CHECK(wrap->interpreter_->subgraph(m_subgraph_index)->ResizeInputTensor(m_tensor_id, m_dims));
  }

  p->m_status = 0;
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_set_num_threads);
    auto wrap = model_idr.find(m_id);

    wrap->interpreter_->SetNumThreads(m_count);
    p->m_status = 0;
    return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_invoke);
    auto wrap = model_idr.find(m_id);

    TFLITE_CHECK(wrap->interpreter_->subgraph(m_subgraph_index)->Invoke());
    p->m_status = 0;
    return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_set_tensor);
  auto wrap = model_idr.find(m_id);
  TFLITE_TENSOR_BOUNDS_CHECK(m_tensor_id);

  TfLiteTensor* tensor = wrap->interpreter_->subgraph(m_subgraph_index)->tensor(m_tensor_id);  
  if (m_data_type != tensor->type) {
    p->m_message = std::string("Cannot set tensor: Got value of type ") +
        TfLiteTypeGetName(static_cast<TfLiteType>(m_data_type)) +
        " but expected type " +
        TfLiteTypeGetName(tensor->type) +
        " for input " +
        std::to_string(m_tensor_id) +
        ", name: " +
        tensor->name + " ";
    p->m_status = -1;
    return p;
  }

  if (m_data_shape.size() != tensor->dims->size) {
    p->m_message = std::string("Cannot set tensor: Dimension mismatch. Got ") +
        std::to_string(m_data_shape.size()) +
        " but expected " +
        std::to_string(tensor->dims->size) +
        " for input " +
        std::to_string(m_tensor_id) +
        ".";
    p->m_status = -1;
    return p;
  }

  for (int j = 0; j < m_data_shape.size(); j++) {
    if (tensor->dims->data[j] != m_data_shape[j]) {
      p->m_message = std::string("Cannot set tensor: Dimension mismatch. Got ") +
          std::to_string(m_data_shape[j]) +
          " but expected " +
          std::to_string(tensor->dims->data[j]) +
          " for dimension " +
          std::to_string(j) +
          " of input " +
          std::to_string(m_tensor_id) +
          ".";
      p->m_status = -1;
      return p;
    }
  }
  if (tensor->type != kTfLiteString) {
    if (tensor->data.raw == nullptr) {
      p->m_message = "Cannot set tensor: Tensor is unallocated. Try calling allocate_tensors() first";
      p->m_status = -1;
      return p;
    }

    if (m_data.size() != tensor->bytes) {
      p->m_message = "numpy array had " + std::to_string(m_data.size()) +
          " bytes but expected " + std::to_string(tensor->bytes) + " bytes.";
      p->m_status = -1;
      return p;
    }
    memcpy(tensor->data.raw, m_data.data(), m_data.size());
    p->m_status = 0;
    return p;
  } else {
    p->m_message = "kTfLiteString not yet supported on this setup";
    p->m_status = -1;
    return p;
  }
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_get_tensor);
  auto wrap = model_idr.find(m_id);
  TFLITE_TENSOR_BOUNDS_CHECK(m_tensor_id);

  TfLiteTensor* tensor = wrap->interpreter_->subgraph(m_subgraph_index)->tensor(m_tensor_id);  
  if (tensor->bytes == 0) {
    p->m_message = "Invalid tensor size.";
    p->m_status = -1;
    return p;
  }

  if (!tensor->data.raw) {
    p->m_message = "Tensor data is null. Run allocate_tensors() first";
    p->m_status = -1;
    return p;
  }

  if (tensor->type == kTfLiteString) {
    p->m_message = "kTfLiteString not yet supported on this setup";
    p->m_status = -1;
    return p;
  }

  size_t size_of_type;
  if(tensor->sparsity != nullptr && tflite::GetSizeOfType(nullptr, tensor->type, &size_of_type) != kTfLiteOk) {
    p->m_message = "Unknown tensor type.";
    p->m_status = -1;
    return p;
  }

  p->m_status = 0;
  p->m_data_type = tensor->type;
  if(tensor->sparsity == nullptr) {
    p->m_data_shape = std::vector<int64_t>(tensor->dims->size);
    for(auto i = 0; i < tensor->dims->size; i++)
      p->m_data_shape[i] = tensor->dims->data[i];
  } else {
    p->m_data_shape = std::vector<int64_t>{tensor->bytes / size_of_type};
  }
  p->m_data = std::vector<uint8_t>(tensor->data.raw, tensor->data.raw + tensor->bytes);
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_get_ti_benchmark_data);
  auto wrap = model_idr.find(m_id);

  p->m_benchmark_data = wrap->interpreter_->subgraph(m_subgraph_index)->get_TI_benchmark_data();
  p->m_status = 0;
  return p;
}

DECLARE_MSG(TFLITE_SECTION + __COUNTER__, tflite_load_exec_delegate);
  auto wrap = model_idr.find(m_id);

  TFLITE_CHECK(wrap->interpreter_->ModifyGraphWithDelegate(reinterpret_cast<TfLiteDelegate *>(m_delegate_ptr)));

  p->m_status = 0;
  return p;
}

/*
 * tflite_tensor_sparsity_parameters response packet has a complex structure
 * e.g., vector<string> which I could not figure out how to automate
 * serialization / deserialization
 *
 * the yaml parser lets these classes have user-defined overrides
 * to read_from() / write_to() if __SKIP_SERIALIZE__ is added to the
 * yaml list
 */
void tflite_tensor_sparsity_parameters_resp::write_to(std::ostream& output) const {
    packet::write<int32_t>(output, m_status);
    packet::write_string(output, m_message);
    packet::write<uint32_t>(output, m_has_sparsity);
    packet::write_vector<int32_t>(output, m_traversal_order);
    packet::write_vector<int32_t>(output, m_block_map);
    packet::write<uint32_t>(output, m_dim_metadata.size());
    for(auto it : m_dim_metadata) {
        packet::write<uint32_t>(output, std::get<0>(it));
        packet::write<int32_t>(output, std::get<1>(it));
        packet::write_vector<int32_t>(output, std::get<2>(it));
        packet::write_vector<int32_t>(output, std::get<3>(it));
    }
}
void tflite_tensor_sparsity_parameters_resp::read_from(std::istream& input) {
    m_status = packet::read<int32_t>(input);
    m_message = packet::read_string(input);
    m_has_sparsity = packet::read<uint32_t>(input);
    m_traversal_order = packet::read_vector<int32_t>(input);
    m_block_map = packet::read_vector<int32_t>(input);
    m_dim_metadata = std::vector<std::tuple<uint32_t, int32_t, std::vector<int32_t>, std::vector<int32_t>>>(packet::read<uint32_t>(input));
    for(auto& it : m_dim_metadata) {
        auto format = packet::read<uint32_t>(input);
        auto dense_size = packet::read<int32_t>(input);
        auto arr_seg = packet::read_vector<int32_t>(input);
        auto arr_idx = packet::read_vector<int32_t>(input);
        it = std::make_tuple(format, dense_size, arr_seg, arr_idx);
    }
}

/*
 * tflite_get_ti_benchmark_data response packet has a complex structure
 * e.g., vector<string> which I could not figure out how to automate
 * serialization / deserialization
 *
 * the yaml parser lets these classes have user-defined overrides
 * to read_from() / write_to() if __SKIP_SERIALIZE__ is added to the
 * yaml list
 */
void tflite_get_ti_benchmark_data_resp::write_to(std::ostream& output) const {
    packet::write<int32_t>(output, m_status);
    packet::write_string(output, m_message);
    packet::write<uint32_t>(output, m_benchmark_data.size());
    for(auto it : m_benchmark_data) {
        packet::write_string(output, it.first);
        packet::write<uint64_t>(output, it.second);
    }
}
void tflite_get_ti_benchmark_data_resp::read_from(std::istream& input) {
    m_status = packet::read<int32_t>(input);
    m_message = packet::read_string(input);
    m_benchmark_data = std::vector<std::pair<std::string, uint64_t>>(packet::read<uint32_t>(input));
    for(auto& it : m_benchmark_data) {
        it.first = packet::read_string(input);
        it.second = packet::read<uint64_t>(input);
    }
}
