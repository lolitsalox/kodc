#pragma once
#include <runtime/objects/Object.hpp>

namespace kod {

struct Tuple;
struct Type;
struct VM;
static std::shared_ptr<Type> kod_type_type = std::make_shared<Type>();
struct Type : public Object {
    std::string type_name = "type";
    
    Type() = default;
    
    Type(std::string type_name, std::shared_ptr<Type> type = kod_type_type) 
        : Object(type), type_name(std::move(type_name)) {
        };
    
    virtual ~Type() = default;

    virtual std::shared_ptr<Object> __new__(std::shared_ptr<Tuple> args);

    virtual std::shared_ptr<Object> __call__(VM* vm, std::shared_ptr<Object> obj, std::shared_ptr<Tuple> args) {
        if (Type* t = dynamic_cast<Type*>(obj.get())) {
            return t->__new__(args);
        }

        throw std::runtime_error("<" + type_name + "> doesn't have a __call__ method.");
    }

    virtual std::shared_ptr<Object> __add__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        // make this a pure virtual
        throw std::runtime_error("<" + type_name + "> doesn't have an __add__ method.");
    }

    virtual std::shared_ptr<Object> __sub__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        throw std::runtime_error("<" + type_name + "> doesn't have an __sub__ method.");
    }
    
    virtual std::shared_ptr<Object> __mul__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        throw std::runtime_error("<" + type_name + "> doesn't have an __mul__ method.");
    }

    virtual std::shared_ptr<Object> __lt__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        throw std::runtime_error("<" + type_name + "> doesn't have an __lt__ method.");
    }

    virtual std::shared_ptr<Object> __gt__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        throw std::runtime_error("<" + type_name + "> doesn't have an __gt__ method.");
    }

    virtual std::shared_ptr<Object> __eq__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        throw std::runtime_error("<" + type_name + "> doesn't have an __eq__ method.");
    }

    virtual std::string __str__(std::shared_ptr<Object> obj);

    virtual int64_t __int__(std::shared_ptr<Object> obj) {
        throw std::runtime_error("Cannot convert " + obj->type->type_name + " to int");
    }

    virtual bool __bool__(std::shared_ptr<Object> obj) {
        throw std::runtime_error("Cannot convert " + obj->type->type_name + " to bool");
    }


};

struct TypeNull : public Type {
    TypeNull() : Type("null_type") {};
    ~TypeNull() = default;
    std::string __str__(std::shared_ptr<Object> obj) override {
        return "null";
    }

    int64_t __int__(std::shared_ptr<Object> obj) override {
        return 0;
    }
};

static std::shared_ptr<TypeNull> kod_type_null = std::make_shared<TypeNull>();

struct Null : public Object {
    Null() : Object(kod_type_null) {}
    ~Null() = default;
};

}