#include <runtime/objects/Int.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {

std::string TypeInt::__str__(std::shared_ptr<Object> obj) {
    if (Int* int_obj = dynamic_cast<Int*>(obj.get())) {
        return std::to_string(int_obj->value);
    }

    return obj->type->__str__(obj);
}

int64_t TypeInt::__int__(std::shared_ptr<Object> obj) {
    if (Int* int_obj = dynamic_cast<Int*>(obj.get())) {
        return int_obj->value;
    }

    return obj->type->__int__(obj);
}

bool TypeInt::__bool__(std::shared_ptr<Object> obj) {
    if (Int* int_obj = dynamic_cast<Int*>(obj.get())) {
        return int_obj->value != 0;
    }

    return obj->type->__bool__(obj);
}

std::shared_ptr<Object> TypeInt::__add__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
    if (dynamic_cast<TypeInt*>(left->type.get()) && 
        dynamic_cast<TypeInt*>(right->type.get())) {
        return std::make_shared<Int>(left->type->__int__(left) + right->type->__int__(right));
    }

    throw std::runtime_error("Cannot add " + left->type->type_name + " and " + right->type->type_name);
}

std::shared_ptr<Object> TypeInt::__sub__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
    if (dynamic_cast<TypeInt*>(left->type.get()) && 
        dynamic_cast<TypeInt*>(right->type.get())) {
        return std::make_shared<Int>(left->type->__int__(left) - right->type->__int__(right));
    }

    throw std::runtime_error("Cannot sub " + left->type->type_name + " and " + right->type->type_name);
}

}
