#include <runtime/objects/Int.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {

std::string TypeInt::__str__(std::shared_ptr<Object> obj) {
    if (Int* int_obj = dynamic_cast<Int*>(obj.get())) {
        return std::to_string(int_obj->value);
    }

    return obj->type->__str__(obj);
}

int64_t TypeInt::to_int(std::shared_ptr<Object> obj) {
    if (Int* int_obj = dynamic_cast<Int*>(obj.get())) {
        return int_obj->value;
    }

    throw std::runtime_error("Cannot convert " + obj->type->type_name + " to " + type_name);
}

std::shared_ptr<Object> TypeInt::__add__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
    if (left->type == kod_type_int && right->type == kod_type_int) {
        return std::make_shared<Int>(left->type->to_int(left) + right->type->to_int(right));
    }

    throw std::runtime_error("Cannot add " + left->type->type_name + " and " + right->type->type_name);
}

}
