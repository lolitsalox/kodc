#pragma once
#include <runtime/runtime.hpp>
#include <runtime/objects/Type.hpp>

namespace kod {

struct TypeTuple : public Type {

    TypeTuple() : Type("tuple") {};
    ~TypeTuple() = default;

    std::string __str__(std::shared_ptr<Object> obj) override;
};

static std::shared_ptr<TypeTuple> kod_type_tuple = std::make_shared<TypeTuple>();

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