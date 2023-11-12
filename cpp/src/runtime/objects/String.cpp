#include <runtime/objects/String.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {

std::string TypeString::__str__(std::shared_ptr<Object> obj) {
    if (String* str_obj = dynamic_cast<String*>(obj.get())) {
        return str_obj->value;
    }

    return obj->type->__str__(obj);
}

int64_t TypeString::__int__(std::shared_ptr<Object> obj) {
    if (String* str_obj = dynamic_cast<String*>(obj.get())) {
        return std::stoll(str_obj->value); // support "0x.." and "0b.."
    }

    return obj->type->__int__(obj);
}

bool TypeString::__bool__(std::shared_ptr<Object> obj) {
    if (String* str_obj = dynamic_cast<String*>(obj.get())) {
        return !str_obj->value.empty();
    }

    return obj->type->__bool__(obj);
}

}