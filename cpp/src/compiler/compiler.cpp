#include <compiler/compiler.hpp>
#include <exception>


namespace kod {
std::string vec_to_string(std::vector<Constant> const& vec) {
    std::string result = "(";
    // dont print a comma if its the last element
    for (size_t i = 0; i < vec.size(); ++i) {
        result += vec[i].to_string() + (i == vec.size() - 1 ? "" : ", ");
    }
    result += ")";
    return result;
}

std::string Opcode_to_string(Opcode opcode) {
    switch (opcode) {
        case Opcode::OP_LOAD_CONST:             return "LOAD_CONST";
        case Opcode::OP_LOAD_NAME:              return "LOAD_NAME";
        case Opcode::OP_LOAD_ATTRIBUTE:         return "LOAD_ATTRIBUTE";
        case Opcode::OP_LOAD_METHOD:            return "LOAD_METHOD";
        case Opcode::OP_STORE_NAME:             return "STORE_NAME";
        case Opcode::OP_STORE_ATTRIBUTE:        return "STORE_ATTRIBUTE";
        case Opcode::OP_POP_TOP:                return "POP_TOP";
        case Opcode::OP_UNARY_ADD:              return "UNARY_ADD";
        case Opcode::OP_UNARY_SUB:              return "UNARY_SUB";
        case Opcode::OP_UNARY_NOT:              return "UNARY_NOT";
        case Opcode::OP_UNARY_BOOL_NOT:         return "UNARY_BOOL_NOT";
        case Opcode::OP_BINARY_ADD:             return "BINARY_ADD";
        case Opcode::OP_BINARY_SUB:             return "BINARY_SUB";
        case Opcode::OP_BINARY_MUL:             return "BINARY_MUL";
        case Opcode::OP_BINARY_DIV:             return "BINARY_DIV";
        case Opcode::OP_BINARY_MOD:             return "BINARY_MOD";
        case Opcode::OP_BINARY_POW:             return "BINARY_POW";
        case Opcode::OP_BINARY_AND:             return "BINARY_AND";
        case Opcode::OP_BINARY_OR:              return "BINARY_OR";
        case Opcode::OP_BINARY_XOR:             return "BINARY_XOR";
        case Opcode::OP_BINARY_LEFT_SHIFT:      return "BINARY_LEFT_SHIFT";
        case Opcode::OP_BINARY_RIGHT_SHIFT:     return "BINARY_RIGHT_SHIFT";
        case Opcode::OP_BINARY_BOOLEAN_AND:     return "BINARY_BOOLEAN_AND";
        case Opcode::OP_BINARY_BOOLEAN_OR:      return "BINARY_BOOLEAN_OR";
        case Opcode::OP_BINARY_BOOLEAN_EQUAL:   return "BINARY_BOOLEAN_EQUAL";
        case Opcode::OP_BINARY_BOOLEAN_NOT_EQUAL: return "BINARY_BOOLEAN_NOT_EQUAL";
        case Opcode::OP_BINARY_BOOLEAN_GREATER_THAN:                return "BINARY_BOOLEAN_GREATER_THAN";
        case Opcode::OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO:    return "BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO";
        case Opcode::OP_BINARY_BOOLEAN_LESS_THAN:                   return "BINARY_BOOLEAN_LESS_THAN";
        case Opcode::OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO:       return "BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO";
        case Opcode::OP_CALL:       return "CALL";
        case Opcode::OP_RETURN:     return "RETURN";
        case Opcode::OP_JUMP:       return "JUMP";
        case Opcode::OP_POP_JUMP_IF_FALSE:  return "POP_JUMP_IF_FALSE";
        case Opcode::OP_BUILD_TUPLE:        return "BUILD_TUPLE";
        case Opcode::OP_BUILD_LIST:         return "BUILD_LIST";
        case Opcode::OP_BUILD_DICT:         return "BUILD_DICT";
        case Opcode::OP_UNKNOWN: return "OP_UNKNOWN";
    }
    return "Unknown opcode: " + std::to_string(static_cast<uint32_t>(opcode));
}

std::string Code::to_string() const {
    std::string result = "Code Object: " + this->name + "\n";
    for (size_t i = 0; i < code.size();) {
        auto opcode = static_cast<Opcode>(read8(i));

        result += std::to_string(i) + ": " + Opcode_to_string(opcode) + " ";

        switch (opcode) {
            case Opcode::OP_CALL:
            case Opcode::OP_BUILD_TUPLE:
            case Opcode::OP_LOAD_NAME:
            case Opcode::OP_LOAD_ATTRIBUTE:
            case Opcode::OP_STORE_NAME:
            case Opcode::OP_LOAD_CONST: {
                auto index = static_cast<uint32_t>(read32(i));
                result += std::to_string(index);
            } break;
            case Opcode::OP_POP_TOP:
            case Opcode::OP_RETURN:
            case Opcode::OP_UNARY_ADD:
            case Opcode::OP_UNARY_SUB:
            case Opcode::OP_UNARY_NOT:
            case Opcode::OP_UNARY_BOOL_NOT:
            case Opcode::OP_BINARY_ADD:
            case Opcode::OP_BINARY_SUB:
            case Opcode::OP_BINARY_MUL:
            case Opcode::OP_BINARY_DIV:
            case Opcode::OP_BINARY_MOD:
            case Opcode::OP_BINARY_POW:
            case Opcode::OP_BINARY_AND:
            case Opcode::OP_BINARY_OR:
            case Opcode::OP_BINARY_XOR:
            case Opcode::OP_BINARY_LEFT_SHIFT:
            case Opcode::OP_BINARY_RIGHT_SHIFT:
            case Opcode::OP_BINARY_BOOLEAN_AND:
            case Opcode::OP_BINARY_BOOLEAN_OR:
            case Opcode::OP_BINARY_BOOLEAN_EQUAL:
            case Opcode::OP_BINARY_BOOLEAN_NOT_EQUAL:
            case Opcode::OP_BINARY_BOOLEAN_GREATER_THAN:
            case Opcode::OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO:
            case Opcode::OP_BINARY_BOOLEAN_LESS_THAN:
            case Opcode::OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO: break;

            default: throw std::runtime_error("Code::to_string() - Unknown opcode: " + Opcode_to_string(static_cast<Opcode>(opcode)));
        }
        result += "\n";
    }


    return result;
}

std::string Constant::to_string() const {
    switch (tag) {
        case ConstantTag::C_NULL: return "null";
        case ConstantTag::C_BOOL: return _bool ? "true" : "false";
        case ConstantTag::C_INTEGER: return std::to_string(_int);
        case ConstantTag::C_FLOAT: return std::to_string(_float);
        case ConstantTag::C_ASCII: return "\"" + _string + "\"";
        case ConstantTag::C_CODE: return "Code object: " + _code.name;
        case ConstantTag::C_TUPLE: return vec_to_string(_tuple);
        default: throw std::runtime_error("Constant::to_string() - Unknown constant tag: " + std::to_string(static_cast<uint32_t>(tag)));
    }
}

void CompiledModule::compile(const std::string& filename) {
    // create a buffer which will store the compiled code
    std::vector<uint8_t> buffer;

    
}

}    
