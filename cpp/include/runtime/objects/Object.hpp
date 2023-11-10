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
    std::shared_ptr<Type> type;

    Object() = default;
    Object(std::shared_ptr<Type> type) : type(type) {};
    virtual ~Object() = default;

    virtual std::string to_string() const; 
};


}