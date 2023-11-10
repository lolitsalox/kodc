#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace kod {

struct Object;
struct Type;
typedef std::unordered_map<std::string, std::shared_ptr<Object>> ObjectMap;

struct Object {
    ObjectMap attributes;
    Type& type;

    Object(Object&& other) = default;
    Object(Type& type) : type(type) {};
    virtual ~Object() = default;

    virtual std::string to_string() const { throw std::runtime_error("Can't convert to string from object."); }
};


}