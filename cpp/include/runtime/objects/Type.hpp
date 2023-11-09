#pragma once
#include <runtime/objects/Object.hpp>

namespace kod {
    
struct Type : public Object {
    
    std::string type_name = "type";
    
    Type() : Object(*this) {};
    
    Type(Type& type, std::string type_name) 
        : Object(type), type_name(std::move(type_name)) {};
    
    virtual ~Type() = default;


};

static Type kod_type_type; // kod_type_class

struct TypeInt : public Type {

    TypeInt() : Type(kod_type_type, "int") {};
    ~TypeInt() = default;

};

static TypeInt kod_type_int;

struct Int : public Object {
    
    Int(int64_t value) : Object(kod_type_int), value(value) {};
    ~Int() = default;

    int64_t value;

};

}