#include <runtime/objects/Object.hpp>
#include <runtime/objects/Type.hpp>

namespace kod {
    
std::string Object::to_string() const { 
    throw std::runtime_error("Can't convert to string from " + type->type_name + "."); 
}

}