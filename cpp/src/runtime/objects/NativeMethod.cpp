#include <runtime/objects/NativeMethod.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {

std::shared_ptr<Object> TypeNativeMethod::__call__(VM* vm, std::shared_ptr<Object> obj, std::shared_ptr<Tuple> args) {
    if (NativeMethod* meth_obj = dynamic_cast<NativeMethod*>(obj.get())) {
        return meth_obj->meth(vm, args->values[0], args);
    }

    return obj->type->__call__(vm, obj, args);
}

std::string TypeNativeMethod::__str__(std::shared_ptr<Object> obj) {
    if (NativeMethod* meth_obj = dynamic_cast<NativeMethod*>(obj.get())) {
        return "native " + obj->type->type_name +" method <" + meth_obj->name + ">";
    }

    return obj->type->__str__(obj);
}

}