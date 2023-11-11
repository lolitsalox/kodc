#pragma once
#include <runtime/objects/Type.hpp>

namespace kod {

struct TypeInt : public Type {

    TypeInt() : Type("int") {};
    ~TypeInt() = default;

    // std::string __new__(std::shared_ptr<Tuple> args) override;
    std::string __str__(std::shared_ptr<Object> obj) override;
    std::shared_ptr<Object> __add__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) override;
    int64_t to_int(std::shared_ptr<Object> obj);
};

static std::shared_ptr<TypeInt> kod_type_int = std::make_shared<TypeInt>();


struct Int : public Object {
    
    Int(int64_t value) : Object(kod_type_int), value(value) {};
    ~Int() = default;

    int64_t value;

};

}