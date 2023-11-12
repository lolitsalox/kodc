#pragma once
#include <runtime/objects/Type.hpp>
#include <iostream>

namespace kod {

struct TypeDict : public Type {

    TypeDict() : Type("dict") {};
    ~TypeDict() = default;

    // std::string __new__(std::shared_ptr<Tuple> args) override;
    std::string __str__(std::shared_ptr<Object> obj) override;

};

static std::shared_ptr<TypeDict> kod_type_dict = std::make_shared<TypeDict>();

struct SharedPtrHash {
    std::size_t operator()(const std::shared_ptr<Object>& ptr) const {
        return 0;
    }
};

// Equality operator for shared_ptr<Object>
struct SharedPtrEqual {
    bool operator()(const std::shared_ptr<Object>& lhs, const std::shared_ptr<Object>& rhs) const {
        return *lhs == rhs;
    }
};

typedef std::unordered_map<std::shared_ptr<Object>, std::shared_ptr<Object>, SharedPtrHash, SharedPtrEqual> ObjectDict;

struct Dict : public Object {
    
    Dict(ObjectDict dict = {}) : Object(kod_type_dict), dict(dict) {};
    ~Dict() = default;

    std::shared_ptr<Object>& operator[](std::string const& key);
    std::shared_ptr<Object>& operator[](std::shared_ptr<Object> key);

    ObjectDict dict;

};

}