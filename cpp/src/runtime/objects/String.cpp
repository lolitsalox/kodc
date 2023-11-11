#include <runtime/objects/String.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {

std::string TypeString::__str__(std::shared_ptr<Object> obj) {
    if (String* str_obj = dynamic_cast<String*>(obj.get())) {
        return str_obj->value;
    }

    return obj->type->__str__(obj);
}

}