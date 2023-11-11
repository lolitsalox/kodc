#include <runtime/objects/Type.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {


std::shared_ptr<Object> Type::__new__(std::shared_ptr<Tuple> args) {
    // throw type error
    if (args->values.size() != 1) {
        throw std::runtime_error(type_name + "() takes 1 argument.");
    }

    // return std::make_shared<Object>(args->values[0]->type);

    throw std::runtime_error("<" + type_name + "> doesn't have a __new__ method.");
}

std::string Type::__str__(std::shared_ptr<Object> obj) {
    if (Type* type_obj = dynamic_cast<Type*>(obj.get())) {
        return "<class '" + type_obj->type_name +"'>";
    }

    throw std::runtime_error("Cannot convert " + obj->type->type_name + " to " + type_name);
}

}