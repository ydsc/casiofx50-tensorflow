/*
 * Auto-generated header file. Do not modify.
 * Make changes to interpreter_wrapper2/tflite_messages.yaml and run
 *
 * # /home/a0133185/ti/GIT_cloud_build_ta/cloud_build_ta/test/ti-inference-engine/parse_messages.py interpreter_wrapper2/tflite_messages.yaml
 *
 * and save the output to interpreter_wrapper2/tflite_messages.h
 */

#ifndef __TFLITE_MESSAGES_H__
#define __TFLITE_MESSAGES_H__

#include <memstreambuf.h>
#include <op_registry.h>
#include <packet.h>

class tflite_create_wrapper_from_file_resp : public packet {
public:
    ~tflite_create_wrapper_from_file_resp() {};
    explicit tflite_create_wrapper_from_file_resp() {};
    explicit tflite_create_wrapper_from_file_resp(const int32_t& status, const std::string& message, const uint32_t& id) :
        m_status(status), 
        m_message(message), 
        m_id(id) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const uint32_t& id() { return m_id; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write<uint32_t>(output, m_id);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_id = packet::read<uint32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    uint32_t m_id;
    friend class tflite_create_wrapper_from_file_req;
};
class tflite_create_wrapper_from_file_req : public packet {
public:
    ~tflite_create_wrapper_from_file_req() {};
    explicit tflite_create_wrapper_from_file_req() {};
    explicit tflite_create_wrapper_from_file_req(const std::string& model_path) :
        m_model_path(model_path) {}
    const std::string& model_path() { return m_model_path; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write_string(output, m_model_path);
    }
    virtual void read_from(std::istream& input) override {
        m_model_path = packet::read_string(input);
    }
private:
    std::string m_model_path;
};

class tflite_create_wrapper_from_buffer_resp : public packet {
public:
    ~tflite_create_wrapper_from_buffer_resp() {};
    explicit tflite_create_wrapper_from_buffer_resp() {};
    explicit tflite_create_wrapper_from_buffer_resp(const int32_t& status, const std::string& message, const uint32_t& id) :
        m_status(status), 
        m_message(message), 
        m_id(id) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const uint32_t& id() { return m_id; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write<uint32_t>(output, m_id);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_id = packet::read<uint32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    uint32_t m_id;
    friend class tflite_create_wrapper_from_buffer_req;
};
class tflite_create_wrapper_from_buffer_req : public packet {
public:
    ~tflite_create_wrapper_from_buffer_req() {};
    explicit tflite_create_wrapper_from_buffer_req() {};
    explicit tflite_create_wrapper_from_buffer_req(const std::vector<uint8_t>& model_data) :
        m_model_data(model_data) {}
    const std::vector<uint8_t>& model_data() { return m_model_data; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write_vector<uint8_t>(output, m_model_data);
    }
    virtual void read_from(std::istream& input) override {
        m_model_data = packet::read_vector<uint8_t>(input);
    }
private:
    std::vector<uint8_t> m_model_data;
};

class tflite_delete_wrapper_resp : public packet {
public:
    ~tflite_delete_wrapper_resp() {};
    explicit tflite_delete_wrapper_resp() {};
    explicit tflite_delete_wrapper_resp(const int32_t& status) :
        m_status(status) {}
    const int32_t& status() { return m_status; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
    }
private:
    int32_t m_status;
    friend class tflite_delete_wrapper_req;
};
class tflite_delete_wrapper_req : public packet {
public:
    ~tflite_delete_wrapper_req() {};
    explicit tflite_delete_wrapper_req() {};
    explicit tflite_delete_wrapper_req(const uint32_t& id) :
        m_id(id) {}
    const uint32_t& id() { return m_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
};

class tflite_num_tensors_resp : public packet {
public:
    ~tflite_num_tensors_resp() {};
    explicit tflite_num_tensors_resp() {};
    explicit tflite_num_tensors_resp(const int32_t& count) :
        m_count(count) {}
    const int32_t& count() { return m_count; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_count);
    }
    virtual void read_from(std::istream& input) override {
        m_count = packet::read<int32_t>(input);
    }
private:
    int32_t m_count;
    friend class tflite_num_tensors_req;
};
class tflite_num_tensors_req : public packet {
public:
    ~tflite_num_tensors_req() {};
    explicit tflite_num_tensors_req() {};
    explicit tflite_num_tensors_req(const uint32_t& id) :
        m_id(id) {}
    const uint32_t& id() { return m_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
};

class tflite_tensor_name_resp : public packet {
public:
    ~tflite_tensor_name_resp() {};
    explicit tflite_tensor_name_resp() {};
    explicit tflite_tensor_name_resp(const std::string& name) :
        m_name(name) {}
    const std::string& name() { return m_name; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write_string(output, m_name);
    }
    virtual void read_from(std::istream& input) override {
        m_name = packet::read_string(input);
    }
private:
    std::string m_name;
    friend class tflite_tensor_name_req;
};
class tflite_tensor_name_req : public packet {
public:
    ~tflite_tensor_name_req() {};
    explicit tflite_tensor_name_req() {};
    explicit tflite_tensor_name_req(const uint32_t& id, const int32_t& tensor_id) :
        m_id(id), 
        m_tensor_id(tensor_id) {}
    const uint32_t& id() { return m_id; }
    const int32_t& tensor_id() { return m_tensor_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_tensor_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_tensor_id = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_tensor_id;
};

class tflite_tensor_type_resp : public packet {
public:
    ~tflite_tensor_type_resp() {};
    explicit tflite_tensor_type_resp() {};
    explicit tflite_tensor_type_resp(const int32_t& status, const std::string& message, const uint32_t& tensor_type) :
        m_status(status), 
        m_message(message), 
        m_tensor_type(tensor_type) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const uint32_t& tensor_type() { return m_tensor_type; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write<uint32_t>(output, m_tensor_type);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_tensor_type = packet::read<uint32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    uint32_t m_tensor_type;
    friend class tflite_tensor_type_req;
};
class tflite_tensor_type_req : public packet {
public:
    ~tflite_tensor_type_req() {};
    explicit tflite_tensor_type_req() {};
    explicit tflite_tensor_type_req(const uint32_t& id, const int32_t& tensor_id) :
        m_id(id), 
        m_tensor_id(tensor_id) {}
    const uint32_t& id() { return m_id; }
    const int32_t& tensor_id() { return m_tensor_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_tensor_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_tensor_id = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_tensor_id;
};

class tflite_tensor_size_resp : public packet {
public:
    ~tflite_tensor_size_resp() {};
    explicit tflite_tensor_size_resp() {};
    explicit tflite_tensor_size_resp(const int32_t& status, const std::string& message, const std::vector<int32_t>& tensor_size) :
        m_status(status), 
        m_message(message), 
        m_tensor_size(tensor_size) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const std::vector<int32_t>& tensor_size() { return m_tensor_size; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write_vector<int32_t>(output, m_tensor_size);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_tensor_size = packet::read_vector<int32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    std::vector<int32_t> m_tensor_size;
    friend class tflite_tensor_size_req;
};
class tflite_tensor_size_req : public packet {
public:
    ~tflite_tensor_size_req() {};
    explicit tflite_tensor_size_req() {};
    explicit tflite_tensor_size_req(const uint32_t& id, const int32_t& tensor_id) :
        m_id(id), 
        m_tensor_id(tensor_id) {}
    const uint32_t& id() { return m_id; }
    const int32_t& tensor_id() { return m_tensor_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_tensor_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_tensor_id = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_tensor_id;
};

class tflite_tensor_size_signature_resp : public packet {
public:
    ~tflite_tensor_size_signature_resp() {};
    explicit tflite_tensor_size_signature_resp() {};
    explicit tflite_tensor_size_signature_resp(const int32_t& status, const std::string& message, const std::vector<int32_t>& tensor_size) :
        m_status(status), 
        m_message(message), 
        m_tensor_size(tensor_size) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const std::vector<int32_t>& tensor_size() { return m_tensor_size; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write_vector<int32_t>(output, m_tensor_size);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_tensor_size = packet::read_vector<int32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    std::vector<int32_t> m_tensor_size;
    friend class tflite_tensor_size_signature_req;
};
class tflite_tensor_size_signature_req : public packet {
public:
    ~tflite_tensor_size_signature_req() {};
    explicit tflite_tensor_size_signature_req() {};
    explicit tflite_tensor_size_signature_req(const uint32_t& id, const int32_t& tensor_id) :
        m_id(id), 
        m_tensor_id(tensor_id) {}
    const uint32_t& id() { return m_id; }
    const int32_t& tensor_id() { return m_tensor_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_tensor_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_tensor_id = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_tensor_id;
};

class tflite_tensor_quantization_resp : public packet {
public:
    ~tflite_tensor_quantization_resp() {};
    explicit tflite_tensor_quantization_resp() {};
    explicit tflite_tensor_quantization_resp(const int32_t& status, const std::string& message, const float& scale, const int32_t& zp) :
        m_status(status), 
        m_message(message), 
        m_scale(scale), 
        m_zp(zp) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const float& scale() { return m_scale; }
    const int32_t& zp() { return m_zp; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write<float>(output, m_scale);
        packet::write<int32_t>(output, m_zp);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_scale = packet::read<float>(input);
        m_zp = packet::read<int32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    float m_scale;
    int32_t m_zp;
    friend class tflite_tensor_quantization_req;
};
class tflite_tensor_quantization_req : public packet {
public:
    ~tflite_tensor_quantization_req() {};
    explicit tflite_tensor_quantization_req() {};
    explicit tflite_tensor_quantization_req(const uint32_t& id, const int32_t& tensor_id) :
        m_id(id), 
        m_tensor_id(tensor_id) {}
    const uint32_t& id() { return m_id; }
    const int32_t& tensor_id() { return m_tensor_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_tensor_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_tensor_id = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_tensor_id;
};

class tflite_tensor_quantization_parameters_resp : public packet {
public:
    ~tflite_tensor_quantization_parameters_resp() {};
    explicit tflite_tensor_quantization_parameters_resp() {};
    explicit tflite_tensor_quantization_parameters_resp(const int32_t& status, const std::string& message, const std::vector<float>& scales, const std::vector<int32_t>& zps, const int32_t& dim) :
        m_status(status), 
        m_message(message), 
        m_scales(scales), 
        m_zps(zps), 
        m_dim(dim) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const std::vector<float>& scales() { return m_scales; }
    const std::vector<int32_t>& zps() { return m_zps; }
    const int32_t& dim() { return m_dim; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write_vector<float>(output, m_scales);
        packet::write_vector<int32_t>(output, m_zps);
        packet::write<int32_t>(output, m_dim);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_scales = packet::read_vector<float>(input);
        m_zps = packet::read_vector<int32_t>(input);
        m_dim = packet::read<int32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    std::vector<float> m_scales;
    std::vector<int32_t> m_zps;
    int32_t m_dim;
    friend class tflite_tensor_quantization_parameters_req;
};
class tflite_tensor_quantization_parameters_req : public packet {
public:
    ~tflite_tensor_quantization_parameters_req() {};
    explicit tflite_tensor_quantization_parameters_req() {};
    explicit tflite_tensor_quantization_parameters_req(const uint32_t& id, const int32_t& tensor_id) :
        m_id(id), 
        m_tensor_id(tensor_id) {}
    const uint32_t& id() { return m_id; }
    const int32_t& tensor_id() { return m_tensor_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_tensor_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_tensor_id = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_tensor_id;
};

class tflite_tensor_sparsity_parameters_resp : public packet {
public:
    ~tflite_tensor_sparsity_parameters_resp() {};
    explicit tflite_tensor_sparsity_parameters_resp() {};
    explicit tflite_tensor_sparsity_parameters_resp(const int32_t& status, const std::string& message, const uint32_t& has_sparsity, const std::vector<int32_t>& traversal_order, const std::vector<int32_t>& block_map, const std::vector<std::tuple<uint32_t, int32_t, std::vector<int32_t>, std::vector<int32_t>>>& dim_metadata) :
        m_status(status), 
        m_message(message), 
        m_has_sparsity(has_sparsity), 
        m_traversal_order(traversal_order), 
        m_block_map(block_map), 
        m_dim_metadata(dim_metadata) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const uint32_t& has_sparsity() { return m_has_sparsity; }
    const std::vector<int32_t>& traversal_order() { return m_traversal_order; }
    const std::vector<int32_t>& block_map() { return m_block_map; }
    const std::vector<std::tuple<uint32_t, int32_t, std::vector<int32_t>, std::vector<int32_t>>>& dim_metadata() { return m_dim_metadata; }
protected:
    virtual void write_to(std::ostream& output) const override;
    virtual void read_from(std::istream& input) override;
private:
    int32_t m_status;
    std::string m_message;
    uint32_t m_has_sparsity;
    std::vector<int32_t> m_traversal_order;
    std::vector<int32_t> m_block_map;
    std::vector<std::tuple<uint32_t, int32_t, std::vector<int32_t>, std::vector<int32_t>>> m_dim_metadata;
    friend class tflite_tensor_sparsity_parameters_req;
};
class tflite_tensor_sparsity_parameters_req : public packet {
public:
    ~tflite_tensor_sparsity_parameters_req() {};
    explicit tflite_tensor_sparsity_parameters_req() {};
    explicit tflite_tensor_sparsity_parameters_req(const uint32_t& id, const int32_t& tensor_id) :
        m_id(id), 
        m_tensor_id(tensor_id) {}
    const uint32_t& id() { return m_id; }
    const int32_t& tensor_id() { return m_tensor_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_tensor_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_tensor_id = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_tensor_id;
};

class tflite_num_nodes_resp : public packet {
public:
    ~tflite_num_nodes_resp() {};
    explicit tflite_num_nodes_resp() {};
    explicit tflite_num_nodes_resp(const int32_t& count) :
        m_count(count) {}
    const int32_t& count() { return m_count; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_count);
    }
    virtual void read_from(std::istream& input) override {
        m_count = packet::read<int32_t>(input);
    }
private:
    int32_t m_count;
    friend class tflite_num_nodes_req;
};
class tflite_num_nodes_req : public packet {
public:
    ~tflite_num_nodes_req() {};
    explicit tflite_num_nodes_req() {};
    explicit tflite_num_nodes_req(const uint32_t& id) :
        m_id(id) {}
    const uint32_t& id() { return m_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
};

class tflite_node_name_resp : public packet {
public:
    ~tflite_node_name_resp() {};
    explicit tflite_node_name_resp() {};
    explicit tflite_node_name_resp(const std::string& name) :
        m_name(name) {}
    const std::string& name() { return m_name; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write_string(output, m_name);
    }
    virtual void read_from(std::istream& input) override {
        m_name = packet::read_string(input);
    }
private:
    std::string m_name;
    friend class tflite_node_name_req;
};
class tflite_node_name_req : public packet {
public:
    ~tflite_node_name_req() {};
    explicit tflite_node_name_req() {};
    explicit tflite_node_name_req(const uint32_t& id, const int32_t& node_id) :
        m_id(id), 
        m_node_id(node_id) {}
    const uint32_t& id() { return m_id; }
    const int32_t& node_id() { return m_node_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_node_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_node_id = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_node_id;
};

class tflite_node_inputs_resp : public packet {
public:
    ~tflite_node_inputs_resp() {};
    explicit tflite_node_inputs_resp() {};
    explicit tflite_node_inputs_resp(const int32_t& status, const std::string& message, const std::vector<int32_t>& inputs) :
        m_status(status), 
        m_message(message), 
        m_inputs(inputs) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const std::vector<int32_t>& inputs() { return m_inputs; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write_vector<int32_t>(output, m_inputs);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_inputs = packet::read_vector<int32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    std::vector<int32_t> m_inputs;
    friend class tflite_node_inputs_req;
};
class tflite_node_inputs_req : public packet {
public:
    ~tflite_node_inputs_req() {};
    explicit tflite_node_inputs_req() {};
    explicit tflite_node_inputs_req(const uint32_t& id, const int32_t& node_id) :
        m_id(id), 
        m_node_id(node_id) {}
    const uint32_t& id() { return m_id; }
    const int32_t& node_id() { return m_node_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_node_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_node_id = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_node_id;
};

class tflite_node_outputs_resp : public packet {
public:
    ~tflite_node_outputs_resp() {};
    explicit tflite_node_outputs_resp() {};
    explicit tflite_node_outputs_resp(const int32_t& status, const std::string& message, const std::vector<int32_t>& outputs) :
        m_status(status), 
        m_message(message), 
        m_outputs(outputs) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const std::vector<int32_t>& outputs() { return m_outputs; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write_vector<int32_t>(output, m_outputs);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_outputs = packet::read_vector<int32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    std::vector<int32_t> m_outputs;
    friend class tflite_node_outputs_req;
};
class tflite_node_outputs_req : public packet {
public:
    ~tflite_node_outputs_req() {};
    explicit tflite_node_outputs_req() {};
    explicit tflite_node_outputs_req(const uint32_t& id, const int32_t& node_id) :
        m_id(id), 
        m_node_id(node_id) {}
    const uint32_t& id() { return m_id; }
    const int32_t& node_id() { return m_node_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_node_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_node_id = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_node_id;
};

class tflite_allocate_tensors_resp : public packet {
public:
    ~tflite_allocate_tensors_resp() {};
    explicit tflite_allocate_tensors_resp() {};
    explicit tflite_allocate_tensors_resp(const int32_t& status, const std::string& message) :
        m_status(status), 
        m_message(message) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    friend class tflite_allocate_tensors_req;
};
class tflite_allocate_tensors_req : public packet {
public:
    ~tflite_allocate_tensors_req() {};
    explicit tflite_allocate_tensors_req() {};
    explicit tflite_allocate_tensors_req(const uint32_t& id, const uint32_t& subgraph_index) :
        m_id(id), 
        m_subgraph_index(subgraph_index) {}
    const uint32_t& id() { return m_id; }
    const uint32_t& subgraph_index() { return m_subgraph_index; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<uint32_t>(output, m_subgraph_index);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_subgraph_index = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
    uint32_t m_subgraph_index;
};

class tflite_input_indices_resp : public packet {
public:
    ~tflite_input_indices_resp() {};
    explicit tflite_input_indices_resp() {};
    explicit tflite_input_indices_resp(const int32_t& status, const std::string& message, const std::vector<int32_t>& indices) :
        m_status(status), 
        m_message(message), 
        m_indices(indices) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const std::vector<int32_t>& indices() { return m_indices; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write_vector<int32_t>(output, m_indices);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_indices = packet::read_vector<int32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    std::vector<int32_t> m_indices;
    friend class tflite_input_indices_req;
};
class tflite_input_indices_req : public packet {
public:
    ~tflite_input_indices_req() {};
    explicit tflite_input_indices_req() {};
    explicit tflite_input_indices_req(const uint32_t& id) :
        m_id(id) {}
    const uint32_t& id() { return m_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
};

class tflite_output_indices_resp : public packet {
public:
    ~tflite_output_indices_resp() {};
    explicit tflite_output_indices_resp() {};
    explicit tflite_output_indices_resp(const int32_t& status, const std::string& message, const std::vector<int32_t>& indices) :
        m_status(status), 
        m_message(message), 
        m_indices(indices) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const std::vector<int32_t>& indices() { return m_indices; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write_vector<int32_t>(output, m_indices);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_indices = packet::read_vector<int32_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    std::vector<int32_t> m_indices;
    friend class tflite_output_indices_req;
};
class tflite_output_indices_req : public packet {
public:
    ~tflite_output_indices_req() {};
    explicit tflite_output_indices_req() {};
    explicit tflite_output_indices_req(const uint32_t& id) :
        m_id(id) {}
    const uint32_t& id() { return m_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
};

class tflite_reset_variable_tensors_resp : public packet {
public:
    ~tflite_reset_variable_tensors_resp() {};
    explicit tflite_reset_variable_tensors_resp() {};
    explicit tflite_reset_variable_tensors_resp(const int32_t& status, const std::string& message) :
        m_status(status), 
        m_message(message) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    friend class tflite_reset_variable_tensors_req;
};
class tflite_reset_variable_tensors_req : public packet {
public:
    ~tflite_reset_variable_tensors_req() {};
    explicit tflite_reset_variable_tensors_req() {};
    explicit tflite_reset_variable_tensors_req(const uint32_t& id) :
        m_id(id) {}
    const uint32_t& id() { return m_id; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
};

class tflite_resize_input_tensor_resp : public packet {
public:
    ~tflite_resize_input_tensor_resp() {};
    explicit tflite_resize_input_tensor_resp() {};
    explicit tflite_resize_input_tensor_resp(const int32_t& status, const std::string& message) :
        m_status(status), 
        m_message(message) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    friend class tflite_resize_input_tensor_req;
};
class tflite_resize_input_tensor_req : public packet {
public:
    ~tflite_resize_input_tensor_req() {};
    explicit tflite_resize_input_tensor_req() {};
    explicit tflite_resize_input_tensor_req(const uint32_t& id, const int32_t& tensor_id, const std::vector<int32_t>& dims, const uint32_t& strict, const uint32_t& subgraph_index) :
        m_id(id), 
        m_tensor_id(tensor_id), 
        m_dims(dims), 
        m_strict(strict), 
        m_subgraph_index(subgraph_index) {}
    const uint32_t& id() { return m_id; }
    const int32_t& tensor_id() { return m_tensor_id; }
    const std::vector<int32_t>& dims() { return m_dims; }
    const uint32_t& strict() { return m_strict; }
    const uint32_t& subgraph_index() { return m_subgraph_index; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_tensor_id);
        packet::write_vector<int32_t>(output, m_dims);
        packet::write<uint32_t>(output, m_strict);
        packet::write<uint32_t>(output, m_subgraph_index);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_tensor_id = packet::read<int32_t>(input);
        m_dims = packet::read_vector<int32_t>(input);
        m_strict = packet::read<uint32_t>(input);
        m_subgraph_index = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_tensor_id;
    std::vector<int32_t> m_dims;
    uint32_t m_strict;
    uint32_t m_subgraph_index;
};

class tflite_set_num_threads_resp : public packet {
public:
    ~tflite_set_num_threads_resp() {};
    explicit tflite_set_num_threads_resp() {};
    explicit tflite_set_num_threads_resp(const int32_t& status, const std::string& message) :
        m_status(status), 
        m_message(message) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    friend class tflite_set_num_threads_req;
};
class tflite_set_num_threads_req : public packet {
public:
    ~tflite_set_num_threads_req() {};
    explicit tflite_set_num_threads_req() {};
    explicit tflite_set_num_threads_req(const uint32_t& id, const int32_t& count) :
        m_id(id), 
        m_count(count) {}
    const uint32_t& id() { return m_id; }
    const int32_t& count() { return m_count; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_count);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_count = packet::read<int32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_count;
};

class tflite_invoke_resp : public packet {
public:
    ~tflite_invoke_resp() {};
    explicit tflite_invoke_resp() {};
    explicit tflite_invoke_resp(const int32_t& status, const std::string& message) :
        m_status(status), 
        m_message(message) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    friend class tflite_invoke_req;
};
class tflite_invoke_req : public packet {
public:
    ~tflite_invoke_req() {};
    explicit tflite_invoke_req() {};
    explicit tflite_invoke_req(const uint32_t& id, const uint32_t& subgraph_index) :
        m_id(id), 
        m_subgraph_index(subgraph_index) {}
    const uint32_t& id() { return m_id; }
    const uint32_t& subgraph_index() { return m_subgraph_index; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<uint32_t>(output, m_subgraph_index);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_subgraph_index = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
    uint32_t m_subgraph_index;
};

class tflite_set_tensor_resp : public packet {
public:
    ~tflite_set_tensor_resp() {};
    explicit tflite_set_tensor_resp() {};
    explicit tflite_set_tensor_resp(const int32_t& status, const std::string& message) :
        m_status(status), 
        m_message(message) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    friend class tflite_set_tensor_req;
};
class tflite_set_tensor_req : public packet {
public:
    ~tflite_set_tensor_req() {};
    explicit tflite_set_tensor_req() {};
    explicit tflite_set_tensor_req(const uint32_t& id, const int32_t& tensor_id, const uint32_t& data_type, const std::vector<int64_t>& data_shape, const std::vector<uint8_t>& data, const uint32_t& subgraph_index) :
        m_id(id), 
        m_tensor_id(tensor_id), 
        m_data_type(data_type), 
        m_data_shape(data_shape), 
        m_data(data), 
        m_subgraph_index(subgraph_index) {}
    const uint32_t& id() { return m_id; }
    const int32_t& tensor_id() { return m_tensor_id; }
    const uint32_t& data_type() { return m_data_type; }
    const std::vector<int64_t>& data_shape() { return m_data_shape; }
    const std::vector<uint8_t>& data() { return m_data; }
    const uint32_t& subgraph_index() { return m_subgraph_index; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_tensor_id);
        packet::write<uint32_t>(output, m_data_type);
        packet::write_vector<int64_t>(output, m_data_shape);
        packet::write_vector<uint8_t>(output, m_data);
        packet::write<uint32_t>(output, m_subgraph_index);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_tensor_id = packet::read<int32_t>(input);
        m_data_type = packet::read<uint32_t>(input);
        m_data_shape = packet::read_vector<int64_t>(input);
        m_data = packet::read_vector<uint8_t>(input);
        m_subgraph_index = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_tensor_id;
    uint32_t m_data_type;
    std::vector<int64_t> m_data_shape;
    std::vector<uint8_t> m_data;
    uint32_t m_subgraph_index;
};

class tflite_get_tensor_resp : public packet {
public:
    ~tflite_get_tensor_resp() {};
    explicit tflite_get_tensor_resp() {};
    explicit tflite_get_tensor_resp(const int32_t& status, const std::string& message, const uint32_t& data_type, const std::vector<int64_t>& data_shape, const std::vector<uint8_t>& data) :
        m_status(status), 
        m_message(message), 
        m_data_type(data_type), 
        m_data_shape(data_shape), 
        m_data(data) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const uint32_t& data_type() { return m_data_type; }
    const std::vector<int64_t>& data_shape() { return m_data_shape; }
    const std::vector<uint8_t>& data() { return m_data; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
        packet::write<uint32_t>(output, m_data_type);
        packet::write_vector<int64_t>(output, m_data_shape);
        packet::write_vector<uint8_t>(output, m_data);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
        m_data_type = packet::read<uint32_t>(input);
        m_data_shape = packet::read_vector<int64_t>(input);
        m_data = packet::read_vector<uint8_t>(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    uint32_t m_data_type;
    std::vector<int64_t> m_data_shape;
    std::vector<uint8_t> m_data;
    friend class tflite_get_tensor_req;
};
class tflite_get_tensor_req : public packet {
public:
    ~tflite_get_tensor_req() {};
    explicit tflite_get_tensor_req() {};
    explicit tflite_get_tensor_req(const uint32_t& id, const int32_t& tensor_id, const uint32_t& subgraph_index) :
        m_id(id), 
        m_tensor_id(tensor_id), 
        m_subgraph_index(subgraph_index) {}
    const uint32_t& id() { return m_id; }
    const int32_t& tensor_id() { return m_tensor_id; }
    const uint32_t& subgraph_index() { return m_subgraph_index; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<int32_t>(output, m_tensor_id);
        packet::write<uint32_t>(output, m_subgraph_index);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_tensor_id = packet::read<int32_t>(input);
        m_subgraph_index = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
    int32_t m_tensor_id;
    uint32_t m_subgraph_index;
};

class tflite_get_ti_benchmark_data_resp : public packet {
public:
    ~tflite_get_ti_benchmark_data_resp() {};
    explicit tflite_get_ti_benchmark_data_resp() {};
    explicit tflite_get_ti_benchmark_data_resp(const int32_t& status, const std::string& message, const std::vector<std::pair<std::string, uint64_t>>& benchmark_data) :
        m_status(status), 
        m_message(message), 
        m_benchmark_data(benchmark_data) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
    const std::vector<std::pair<std::string, uint64_t>>& benchmark_data() { return m_benchmark_data; }
protected:
    virtual void write_to(std::ostream& output) const override;
    virtual void read_from(std::istream& input) override;
private:
    int32_t m_status;
    std::string m_message;
    std::vector<std::pair<std::string, uint64_t>> m_benchmark_data;
    friend class tflite_get_ti_benchmark_data_req;
};
class tflite_get_ti_benchmark_data_req : public packet {
public:
    ~tflite_get_ti_benchmark_data_req() {};
    explicit tflite_get_ti_benchmark_data_req() {};
    explicit tflite_get_ti_benchmark_data_req(const uint32_t& id, const uint32_t& subgraph_index) :
        m_id(id), 
        m_subgraph_index(subgraph_index) {}
    const uint32_t& id() { return m_id; }
    const uint32_t& subgraph_index() { return m_subgraph_index; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<uint32_t>(output, m_subgraph_index);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_subgraph_index = packet::read<uint32_t>(input);
    }
private:
    uint32_t m_id;
    uint32_t m_subgraph_index;
};

class tflite_load_exec_delegate_resp : public packet {
public:
    ~tflite_load_exec_delegate_resp() {};
    explicit tflite_load_exec_delegate_resp() {};
    explicit tflite_load_exec_delegate_resp(const int32_t& status, const std::string& message) :
        m_status(status), 
        m_message(message) {}
    const int32_t& status() { return m_status; }
    const std::string& message() { return m_message; }
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<int32_t>(output, m_status);
        packet::write_string(output, m_message);
    }
    virtual void read_from(std::istream& input) override {
        m_status = packet::read<int32_t>(input);
        m_message = packet::read_string(input);
    }
private:
    int32_t m_status;
    std::string m_message;
    friend class tflite_load_exec_delegate_req;
};
class tflite_load_exec_delegate_req : public packet {
public:
    ~tflite_load_exec_delegate_req() {};
    explicit tflite_load_exec_delegate_req() {};
    explicit tflite_load_exec_delegate_req(const uint32_t& id, const uint64_t& delegate_ptr) :
        m_id(id), 
        m_delegate_ptr(delegate_ptr) {}
    const uint32_t& id() { return m_id; }
    const uint64_t& delegate_ptr() { return m_delegate_ptr; }
    virtual std::unique_ptr<packet> handle() override;
    static const uint32_t transport_id;
protected:
    virtual void write_to(std::ostream& output) const override {
        packet::write<uint32_t>(output, m_id);
        packet::write<uint64_t>(output, m_delegate_ptr);
    }
    virtual void read_from(std::istream& input) override {
        m_id = packet::read<uint32_t>(input);
        m_delegate_ptr = packet::read<uint64_t>(input);
    }
private:
    uint32_t m_id;
    uint64_t m_delegate_ptr;
};


#endif
