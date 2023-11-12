#pragma once
#include <runtime/objects/Type.hpp>

namespace kod {

struct TypeInt : public Type {

    TypeInt();
    ~TypeInt() = default;

    // std::shared_ptr<Object> __new__(std::shared_ptr<Tuple> args) override;
    std::string __str__(std::shared_ptr<Object> obj) override;
    int64_t __int__(std::shared_ptr<Object> obj);
    bool __bool__(std::shared_ptr<Object> obj);

    std::shared_ptr<Object> __new__(std::shared_ptr<Tuple> args);

    std::shared_ptr<Object> __add__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) override;
    std::shared_ptr<Object> __sub__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) override;
    std::shared_ptr<Object> __mul__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) override;
    std::shared_ptr<Object> __lt__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) override;
    std::shared_ptr<Object> __gt__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) override;
    std::shared_ptr<Object> __eq__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) override;
};

static std::shared_ptr<TypeInt> kod_type_int = std::make_shared<TypeInt>();


struct Int : public Object {
    
    Int(int64_t value) : Object(kod_type_int, kod_type_int->attributes), value(value) {};
    ~Int() = default;

    int64_t value;

};

}