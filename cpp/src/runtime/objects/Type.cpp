#include <runtime/objects/Type.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {


std::shared_ptr<Object> Type::__new__(std::shared_ptr<Tuple> args) {
    // throw type error
    if (args->values.size() != 1) {
        throw std::runtime_error(type_name + "() takes 1 argument.");
    }

    return std::make_shared<Object>(args->values[0]->type);
}

std::string Type::__str__(std::shared_ptr<Tuple> args) {
    if (args->values.size() != 1) {
        throw std::runtime_error("Expected 1 argument.");
    } 

    auto& obj = args->values[0];

    if (Type* type_obj = dynamic_cast<Type*>(obj.get())) {
        return "<class '" + type_obj->type_name +"'>";
    }

    return obj->type.__str__(args);
}

std::string TypeInt::__str__(std::shared_ptr<Tuple> args) {
    if (args->values.size() != 1) {
        throw std::runtime_error("Expected 1 argument.");
    } 

    auto& obj = args->values[0];

    if (Int* int_obj = dynamic_cast<Int*>(obj.get())) {
        return std::to_string(int_obj->value);
    }

    return obj->type.__str__(args);
}

}