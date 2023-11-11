#pragma once
#include <runtime/objects/Type.hpp>

namespace kod {

struct TypeNativeFunc : public Type {

    TypeNativeFunc() : Type("native_func") {};
    ~TypeNativeFunc() = default;

    // std::string __new__(std::shared_ptr<Tuple> args) override;
    std::string __str__(std::shared_ptr<Object> obj) override;
    std::shared_ptr<Object> __call__(VM* vm, std::shared_ptr<Object> obj, std::shared_ptr<Tuple> args) override;

};

static std::shared_ptr<TypeNativeFunc> kod_type_native_func = std::make_shared<TypeNativeFunc>();
typedef std::shared_ptr<Object> (*native_func)(VM* vm, std::shared_ptr<Tuple> args);

struct NativeFunc : public Object {
    
    NativeFunc(native_func func, std::string name) : Object(kod_type_native_func), func(func), name(name) {};
    ~NativeFunc() = default;

    native_func func;
    std::string name;

};

}