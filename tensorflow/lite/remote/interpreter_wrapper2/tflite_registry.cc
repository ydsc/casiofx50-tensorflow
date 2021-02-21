#include <op_registry.h>
#include <packet.h>

#include "tflite_messages.h"

REGISTER_MSG(tflite_create_wrapper_from_file);
REGISTER_MSG(tflite_create_wrapper_from_buffer);
REGISTER_MSG(tflite_delete_wrapper);
REGISTER_MSG(tflite_num_tensors);
REGISTER_MSG(tflite_tensor_name);
REGISTER_MSG(tflite_tensor_type);
REGISTER_MSG(tflite_tensor_size);
REGISTER_MSG(tflite_tensor_size_signature);
REGISTER_MSG(tflite_tensor_quantization);
REGISTER_MSG(tflite_tensor_quantization_parameters);
REGISTER_MSG(tflite_tensor_sparsity_parameters);
REGISTER_MSG(tflite_num_nodes);
REGISTER_MSG(tflite_node_name);
REGISTER_MSG(tflite_node_inputs);
REGISTER_MSG(tflite_node_outputs);
REGISTER_MSG(tflite_allocate_tensors);
REGISTER_MSG(tflite_input_indices);
REGISTER_MSG(tflite_output_indices);
REGISTER_MSG(tflite_reset_variable_tensors);
REGISTER_MSG(tflite_resize_input_tensor);
REGISTER_MSG(tflite_set_num_threads);
REGISTER_MSG(tflite_invoke);
REGISTER_MSG(tflite_set_tensor);
REGISTER_MSG(tflite_get_tensor);
REGISTER_MSG(tflite_get_ti_benchmark_data);
REGISTER_MSG(tflite_load_exec_delegate);
