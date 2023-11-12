#pragma once
#include <runtime/runtime.hpp>
#include <runtime/objects/Type.hpp>
#include <runtime/objects/Dict.hpp>

namespace kod {

struct TypeCode : public Type {

    TypeCode() : Type("code") {};
    ~TypeCode() = default;

    // std::string __new__(std::shared_ptr<Tuple> args) override;
    std::string __str__(std::shared_ptr<Object> obj) override;
    std::shared_ptr<Object> __call__(VM* vm, std::shared_ptr<Object> obj, std::shared_ptr<Tuple> args) override;

};

static std::shared_ptr<TypeCode> kod_type_code = std::make_shared<TypeCode>();
typedef std::shared_ptr<Object> (*native_func)(VM* vm, std::shared_ptr<Tuple> args);

struct CodeObj : public Object {
    
    CodeObj(Code code) : Object(kod_type_code), code(code) {};
    ~CodeObj() = default;

    Code code;
    Dict locals = {};

};

}