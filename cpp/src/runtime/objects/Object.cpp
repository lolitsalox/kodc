#include <runtime/objects/Object.hpp>
#include <runtime/objects/Type.hpp>

namespace kod {
    
Object::Object(std::shared_ptr<Type> type, std::shared_ptr<Dict> attributes)
    : type(type), attributes(attributes) {

        if (attributes && type) {
            attributes = type->attributes;
        }
    }

std::string Object::to_string() const { 
    throw std::runtime_error("Can't convert to string from " + type->type_name + "."); 
}

}