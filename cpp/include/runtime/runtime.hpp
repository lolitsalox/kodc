#pragma once

#include <parser/parser.hpp>
#include <unordered_map>

namespace kod {

enum Type {
    T_NULL = 0,
    T_INT = 1,
    T_FLOAT = 2,
    T_BOOL = 3,
    T_STRING = 4,
    T_TUPLE = 5,
};

struct Object {
    Object() = default;
    virtual ~Object() = default;
    virtual std::string to_string() const { throw std::runtime_error("Can't convert to string from object."); }
    virtual int64_t to_int() const { throw std::runtime_error("Can't convert to int from " + to_string() + "."); }
    virtual double to_float() const { throw std::runtime_error("Can't convert to float from " + to_string() + "."); }
    virtual bool to_bool() const { throw std::runtime_error("Can't convert to bool from " + to_string() + "."); }
};

typedef std::unordered_map<std::string, std::shared_ptr<Object>> ObjectMap;

struct ObjectNull : public Object {
    ObjectNull() = default;
    ~ObjectNull() = default;
    std::string to_string() const override { return "null"; }
};

struct ObjectInt : public Object {
    int64_t value = 0;
    ObjectInt() = default;
    ~ObjectInt() = default;
    ObjectInt(int64_t value) : value(value) {}
    std::string to_string() const override { return std::to_string(value); }
    int64_t to_int() const override { return value; }
};

struct ObjectFloat : public Object {
    double value = 0;
    ObjectFloat() = default;
    ~ObjectFloat() = default;
    ObjectFloat(double value) : value(value) {}
    std::string to_string() const override { return std::to_string(value); }
    double to_float() const override { return value; }
};

struct ObjectBool : public Object {
    bool value = false;
    ObjectBool() = default;
    ~ObjectBool() = default;
    ObjectBool(bool value) : value(value) {}
    std::string to_string() const override { return value ? "true" : "false"; }
    bool to_bool() const override { return value; }
};

struct ObjectString : public Object {
    std::string value;
    ObjectString() = default;
    ~ObjectString() = default;
    ObjectString(std::string value) : value(std::move(value)) {}
    std::string to_string() const override { return value; }
};

struct ObjectCode : public Object {
    Code value;
    ObjectCode() = default;
    ~ObjectCode() = default;
    ObjectCode(Code value) : value(value) {}
    std::string to_string() const override { return value.to_string(); }
};

std::shared_ptr<Object> constant_to_object(Constant const& constant);

struct ObjectTuple : public Object {
    std::vector<std::shared_ptr<Object>> value;
    ObjectTuple() = default;
    ~ObjectTuple() = default;
    ObjectTuple(std::vector<Constant> value) {
        for (auto& constant : value) {
            this->value.push_back(constant_to_object(constant));
        }
    }
    std::string to_string() const override { return ""; }
};

struct CallFrame {
    Code code;
    ObjectMap locals;
    size_t ip = 0;

    CallFrame() = default;
    CallFrame(Code code, ObjectMap locals = {}) : code(code), locals(std::move(locals)) {}
    ~CallFrame() = default;

};

struct VM {
    CompiledModule& module;
    std::vector<CallFrame> call_stack;
    std::vector<std::shared_ptr<Object>> object_stack;
    std::vector<std::shared_ptr<Object>> constant_objects;
    ObjectMap globals;

    bool repl = false;

    VM(CompiledModule& module, bool repl = false) : module(module), repl(repl) {
        update_constants();
    }
    ~VM() = default;

    void update_constants() {
        constant_objects.clear();
        for (auto& constant : module.constant_pool) {
            constant_objects.push_back(constant_to_object(constant));
        }

        if (repl && !call_stack.empty()) {
            call_stack[0].code = module.entry;
        }
    }
    std::optional<std::shared_ptr<Object>> run();

    std::optional<std::shared_ptr<Object>> search_name(const std::string& name);
    std::optional<std::shared_ptr<Object>> search_global_name(const std::string& name);
};

}