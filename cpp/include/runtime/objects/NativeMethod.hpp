#pragma once
#include <runtime/objects/Type.hpp>

namespace kod {

struct TypeNativeMethod : public Type {

    TypeNativeMethod() : Type("native_method") {};
    ~TypeNativeMethod() = default;

    std::string __str__(std::shared_ptr<Object> obj) override;
    std::shared_ptr<Object> __call__(VM* vm, std::shared_ptr<Object> obj, std::shared_ptr<Tuple> args) override;

};

static std::shared_ptr<TypeNativeMethod> kod_type_native_method = std::make_shared<TypeNativeMethod>();
typedef std::shared_ptr<Object> (*native_meth)(VM* vm, std::shared_ptr<Object> self, std::shared_ptr<Tuple> args);

struct NativeMethod : public Object {
    
    NativeMethod(native_meth meth, std::string name, std::string doc) : Object(kod_type_native_method), meth(meth), name(name), doc(doc) {};
    ~NativeMethod() = default;

    native_meth meth;
    std::string name, doc;

};

}