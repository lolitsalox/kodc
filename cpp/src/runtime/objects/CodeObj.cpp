#include <runtime/objects/CodeObj.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {

std::shared_ptr<Object> TypeCode::__call__(VM* vm, std::shared_ptr<Object> obj, std::shared_ptr<Tuple> args) {
    if (CodeObj* code_obj = dynamic_cast<CodeObj*>(obj.get())) {

        for (size_t i = 0; i < args->values.size(); i++) {
            const std::string& arg_name = code_obj->code.params[i];
            code_obj->locals[arg_name] = args->values[i];
        }

        return obj;
    }

    return obj->type->__call__(vm, obj, args);
}

std::string TypeCode::__str__(std::shared_ptr<Object> obj) {
    if (CodeObj* code_obj = dynamic_cast<CodeObj*>(obj.get())) {
        return "code object <" + code_obj->code.name + ">";
    }

    return obj->type->__str__(obj);
}

}