#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>

namespace kod {

struct Type;

struct Object {
    std::shared_ptr<Type> type;

    Object() = default;
    Object(std::shared_ptr<Type> type) : type(type) {};
    virtual ~Object() = default;

    virtual std::string to_string() const; 

    virtual bool operator==(const std::shared_ptr<Object> other) const {
        throw std::runtime_error("Unimplemented operator==");
    }        
    
};

}