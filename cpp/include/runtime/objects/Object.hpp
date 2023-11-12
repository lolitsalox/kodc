#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>

namespace kod {

struct Type;
struct Dict;

struct Object {
    std::shared_ptr<Type> type;
    std::shared_ptr<Dict> attributes;

    Object() = default;
    Object(std::shared_ptr<Type> type, std::shared_ptr<Dict> attributes = {});
    virtual ~Object() = default;

    virtual std::string to_string() const; 

    virtual bool operator==(const std::shared_ptr<Object> other) const {
        throw std::runtime_error("Unimplemented operator==");
    }        
    
};

}