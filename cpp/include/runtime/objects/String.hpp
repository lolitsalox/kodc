#pragma once
#include <runtime/objects/Type.hpp>

namespace kod {

struct TypeString : public Type {

    TypeString() : Type("str") {};
    ~TypeString() = default;

    // std::string __new__(std::shared_ptr<Tuple> args) override;
    std::string __str__(std::shared_ptr<Object> obj) override;
    int64_t __int__(std::shared_ptr<Object> obj) override;

};

static std::shared_ptr<TypeString> kod_type_string = std::make_shared<TypeString>();


struct String : public Object {
    
    String(std::string value) : Object(kod_type_string), value(value) {};
    ~String() = default;

    std::string value;

};

}