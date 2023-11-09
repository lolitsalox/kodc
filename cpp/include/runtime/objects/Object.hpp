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

    // enum Kind {
    //     OBJECT_INT,
    //     OBJECT_FLOAT,
    //     OBJECT_STRING,
    //     OBJECT_FUNC,
    //     OBJECT_METHOD,
    //     OBJECT_NATIVE_FUNC,
    //     OBJECT_NATIVE_METHOD,
    //     OBJECT_TYPE,
    //     OBJECT_CLASS,
    //     OBJECT_NULL,
    //     OBJECT_BOOL,
    //     OBJECT_TUPLE,
    // };

    ObjectMap attributes;
    Type& type;

    Object(Type& type) : type(type) {};
    virtual ~Object() = default;

    virtual std::string to_string() const { throw std::runtime_error("Can't convert to string from object."); }
    // virtual std::string to_string() const { throw std::runtime_error("Can't convert to string from object."); }
    // virtual int64_t to_int() const { throw std::runtime_error("Can't convert to int from " + to_string() + "."); }
    // virtual double to_float() const { throw std::runtime_error("Can't convert to float from " + to_string() + "."); }
    // virtual bool to_bool() const { throw std::runtime_error("Can't convert to bool from " + to_string() + "."); }

    // virtual std::string type_name() const = 0;
    
};


}