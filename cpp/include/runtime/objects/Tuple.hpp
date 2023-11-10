#pragma once
#include <runtime/runtime.hpp>
#include <runtime/objects/Type.hpp>

namespace kod {

struct TypeTuple : public Type {

    TypeTuple() : Type(kod_type_type, "tuple") {};
    ~TypeTuple() = default;
};
static TypeTuple kod_type_tuple;

struct Tuple : public Object {

    std::vector<std::shared_ptr<Object>> values;
    
    Tuple() : Object(kod_type_tuple) {}
    Tuple(std::vector<Constant> values) : Object(kod_type_tuple) {
        for (auto& value : values) {
            this->values.push_back(constant_to_object(value));
        }
    }
    Tuple(std::vector<std::shared_ptr<Object>> values = {}) : Object(kod_type_tuple), values(std::move(values)) {}


    ~Tuple() = default;
};


}