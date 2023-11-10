#include <runtime/objects/Int.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {

std::string TypeInt::__str__(std::shared_ptr<Tuple> args) {
    if (args->values.size() != 1) {
        throw std::runtime_error("Expected 1 argument.");
    } 

    auto& obj = args->values[0];

    if (Int* int_obj = dynamic_cast<Int*>(obj.get())) {
        return std::to_string(int_obj->value);
    }

    return obj->type->__str__(args);
}

}