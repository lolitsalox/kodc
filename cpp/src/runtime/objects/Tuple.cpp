#include <runtime/objects/Tuple.hpp>
#include <runtime/objects/Type.hpp>

namespace kod {

std::string TypeTuple::__str__(std::shared_ptr<Object> obj) {
    if (Tuple* tuple_obj = dynamic_cast<Tuple*>(obj.get())) {
        std::string result = "(";

        for (size_t i = 0; i < tuple_obj->values.size(); ++i) {
            auto& elem = tuple_obj->values[i];
            result += elem->type->__str__(elem) + (i == tuple_obj->values.size() - 1 ? "" : ", ");
        }

        return result + (tuple_obj->values.size() == 1 ? ",)" : ")");
    }

    return obj->type->__str__(obj);

    }
}