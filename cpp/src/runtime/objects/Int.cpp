#include <runtime/objects/Int.hpp>
#include <runtime/objects/Tuple.hpp>
#include <iostream>

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

std::shared_ptr<Object> TypeInt::__new__(std::shared_ptr<Tuple> args) {
    if (args->values.size() > 0) {
        return std::make_shared<Int>(args->values[0]->type->__int__(args->values[0]));
    }

    return std::make_shared<Int>(0);
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

std::shared_ptr<Object> TypeInt::__mul__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
    if (dynamic_cast<TypeInt*>(left->type.get()) && 
        dynamic_cast<TypeInt*>(right->type.get())) {
        return std::make_shared<Int>(left->type->__int__(left) * right->type->__int__(right));
    }

    throw std::runtime_error("Cannot mul " + left->type->type_name + " and " + right->type->type_name);
}

std::shared_ptr<Object> TypeInt::__lt__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
    if (dynamic_cast<TypeInt*>(left->type.get()) && 
        dynamic_cast<TypeInt*>(right->type.get())) {
        return std::make_shared<Int>(left->type->__int__(left) < right->type->__int__(right));
    }

    throw std::runtime_error("Cannot lt " + left->type->type_name + " and " + right->type->type_name);
}

std::shared_ptr<Object> TypeInt::__gt__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
    if (dynamic_cast<TypeInt*>(left->type.get()) && 
        dynamic_cast<TypeInt*>(right->type.get())) {
        return std::make_shared<Int>(left->type->__int__(left) > right->type->__int__(right));
    }

    throw std::runtime_error("Cannot gt " + left->type->type_name + " and " + right->type->type_name);
}

std::shared_ptr<Object> TypeInt::__eq__(std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
    if (dynamic_cast<TypeInt*>(left->type.get()) && 
        dynamic_cast<TypeInt*>(right->type.get())) {
        return std::make_shared<Int>(left->type->__int__(left) == right->type->__int__(right));
    }

    throw std::runtime_error("Cannot eq " + left->type->type_name + " and " + right->type->type_name);
}

}
