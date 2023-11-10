#pragma once
#include <runtime/objects/Type.hpp>

namespace kod {

struct TypeInt : public Type {

    TypeInt() : Type("int") {};
    ~TypeInt() = default;

    // std::string __new__(std::shared_ptr<Tuple> args) override;
    std::string __str__(std::shared_ptr<Tuple> args) override;

};

static std::shared_ptr<TypeInt> kod_type_int = std::make_shared<TypeInt>();


struct Int : public Object {
    
    Int(int64_t value) : Object(kod_type_int), value(value) {};
    ~Int() = default;

    int64_t value;

};

}