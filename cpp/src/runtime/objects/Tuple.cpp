#include <runtime/objects/Tuple.hpp>
#include <runtime/objects/Type.hpp>

namespace kod {

std::string TypeTuple::__str__(std::shared_ptr<Tuple> args) {
    if (args->values.size() != 1) {
        throw std::runtime_error(type_name + "() takes 1 argument.");
    }

    auto& obj = args->values[0];
    
    if (Tuple* tuple_obj = dynamic_cast<Tuple*>(obj.get())) {
        std::string result = "(";

        for (size_t i = 0; i < tuple_obj->values.size(); ++i) {
            auto& elem = tuple_obj->values[i];
            auto tup = std::make_shared<Tuple>(std::vector<std::shared_ptr<Object>>{elem});
            
            result += elem->type->__str__(tup) + (i == tuple_obj->values.size() - 1 ? "" : ", ");
        }

        return result + (tuple_obj->values.size() == 1 ? ",)" : ")");
    }

    return obj->type->__str__(args);

    }
}