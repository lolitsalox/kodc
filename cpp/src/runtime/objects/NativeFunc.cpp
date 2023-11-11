#include <runtime/objects/NativeFunc.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {

std::shared_ptr<Object> TypeNativeFunc::__call__(VM* vm, std::shared_ptr<Object> obj, std::shared_ptr<Tuple> args) {
    if (NativeFunc* func_obj = dynamic_cast<NativeFunc*>(obj.get())) {
        return func_obj->func(vm, args);
    }

    return obj->type->__call__(vm, obj, args);
}

std::string TypeNativeFunc::__str__(std::shared_ptr<Object> obj) {
    if (NativeFunc* func_obj = dynamic_cast<NativeFunc*>(obj.get())) {
        return "native func <" + func_obj->name + ">";
    }

    return obj->type->__str__(obj);
}

}