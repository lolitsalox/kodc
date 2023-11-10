#pragma once
#include <runtime/objects/Object.hpp>

namespace kod {

struct Tuple;
struct Type : public Object {
    
    std::string type_name = "type";
    
    Type() : Object(*this) {};
    
    Type(Type& type, std::string type_name) 
        : Object(type), type_name(std::move(type_name)) {};
    
    virtual ~Type() = default;

    virtual int64_t to_int() const {
        throw std::runtime_error("Can't convert to int from <" + type_name + ">.");
    }

    virtual std::shared_ptr<Object> __new__(std::shared_ptr<Tuple> args);
    virtual std::string __str__(std::shared_ptr<Tuple> args);


};

static Type kod_type_type; // kod_type_class

struct TypeInt : public Type {

    TypeInt() : Type(kod_type_type, "int") {};
    ~TypeInt() = default;

    // std::string __new__(std::shared_ptr<Tuple> args) override;
    std::string __str__(std::shared_ptr<Tuple> args) override;

};

static TypeInt kod_type_int;

struct Int : public Object {
    
    Int(int64_t value) : Object(kod_type_int), value(value) {};
    ~Int() = default;

    int64_t value;

};

struct TypeNull : public Type {
    TypeNull() : Type(kod_type_type, "null_type") {};
    ~TypeNull() = default;
};

static TypeNull kod_type_null;

struct Null : public Object {
    Null() : Object(kod_type_null) {}
    ~Null() = default;
};

}