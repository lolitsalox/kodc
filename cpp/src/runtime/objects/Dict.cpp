#include <runtime/objects/Dict.hpp>
#include <runtime/objects/Tuple.hpp>
#include <runtime/objects/String.hpp>

namespace kod {

std::string TypeDict::__str__(std::shared_ptr<Object> obj) {
    if (Dict* dict_obj = dynamic_cast<Dict*>(obj.get())) {
        std::string result = "{";

        auto it = dict_obj->dict.begin();
        auto end = dict_obj->dict.end();
        while (it != end) {
            const auto& key = it->first;
            const auto& value = it->second;
            
            result += key->type->__str__(key) + ": " + value->type->__str__(value);
            
            ++it;
            if (it != end) {
                result += ", ";
            }
        }
        result += "}";

        return result;
    }

    return obj->type->__str__(obj);
}

std::shared_ptr<Object>& Dict::operator[](std::string const& key) {
    return dict[std::make_shared<String>(key)];
}

std::shared_ptr<Object>& Dict::operator[](std::shared_ptr<Object> key) {
    return dict[key];
}

}