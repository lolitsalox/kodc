#include <runtime/objects/String.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {

std::string TypeString::__str__(std::shared_ptr<Tuple> args) {
    if (args->values.size() != 1) {
        throw std::runtime_error("Expected 1 argument.");
    } 

    auto& obj = args->values[0];

    if (String* str_obj = dynamic_cast<String*>(obj.get())) {
        return str_obj->value;
    }

    return obj->type->__str__(args);
}

}