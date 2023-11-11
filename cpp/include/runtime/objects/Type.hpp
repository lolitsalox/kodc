#pragma once
#include <runtime/objects/Object.hpp>

namespace kod {

struct Tuple;
struct Type;
static std::shared_ptr<Type> kod_type_type = std::make_shared<Type>();
struct Type : public Object {
    std::string type_name = "type";
    
    Type() = default;
    
    Type(std::string type_name, std::shared_ptr<Type> type = kod_type_type) 
        : Object(type), type_name(std::move(type_name)) {};
    
    virtual ~Type() = default;

    virtual int64_t to_int() const {
        throw std::runtime_error("Can't convert to int from <" + type_name + ">.");
    }

    virtual std::shared_ptr<Object> __new__(std::shared_ptr<Tuple> args);
    virtual std::string __str__(std::shared_ptr<Object> obj);

    virtual std::shared_ptr<Object> __add__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        throw std::runtime_error("<" + type_name + "> doesn't have an __add__ method.");
    }

    virtual int64_t to_int(std::shared_ptr<Object> obj) {
        throw std::runtime_error("Cannot convert " + obj->type->type_name + " to " + type_name);
    }


};

struct TypeNull : public Type {
    TypeNull() : Type("null_type") {};
    ~TypeNull() = default;
    std::string __str__(std::shared_ptr<Object> obj) override {
        return "null";
    }
};

static std::shared_ptr<TypeNull> kod_type_null = std::make_shared<TypeNull>();

struct Null : public Object {
    Null() : Object(kod_type_null) {}
    ~Null() = default;
};

}