#include <compiler/compiler.hpp>
#include <exception>

namespace kod {

std::string Opcode_to_string(Opcode opcode) {
    switch (opcode) {
        case Opcode::OP_LOAD_CONST: return "OP_LOAD_CONST";
        case Opcode::OP_LOAD_NAME: return "OP_LOAD_NAME";
        case Opcode::OP_LOAD_ATTRIBUTE: return "OP_LOAD_ATTRIBUTE";
        case Opcode::OP_LOAD_METHOD: return "OP_LOAD_METHOD";
        case Opcode::OP_STORE_NAME: return "OP_STORE_NAME";
        case Opcode::OP_STORE_ATTRIBUTE: return "OP_STORE_ATTRIBUTE";
        case Opcode::OP_POP_TOP: return "OP_POP_TOP";
        case Opcode::OP_UNARY_ADD: return "OP_UNARY_ADD";
        case Opcode::OP_UNARY_SUB: return "OP_UNARY_SUB";
        case Opcode::OP_UNARY_NOT: return "OP_UNARY_NOT";
        case Opcode::OP_UNARY_BOOL_NOT: return "OP_UNARY_BOOL_NOT";
        case Opcode::OP_BINARY_ADD: return "OP_BINARY_ADD";
        case Opcode::OP_BINARY_SUB: return "OP_BINARY_SUB";
        case Opcode::OP_BINARY_MUL: return "OP_BINARY_MUL";
        case Opcode::OP_BINARY_DIV: return "OP_BINARY_DIV";
        case Opcode::OP_BINARY_MOD: return "OP_BINARY_MOD";
        case Opcode::OP_BINARY_POW: return "OP_BINARY_POW";
        case Opcode::OP_BINARY_AND: return "OP_BINARY_AND";
        case Opcode::OP_BINARY_OR: return "OP_BINARY_OR";
        case Opcode::OP_BINARY_XOR: return "OP_BINARY_XOR";
        case Opcode::OP_BINARY_LEFT_SHIFT: return "OP_BINARY_LEFT_SHIFT";
        case Opcode::OP_BINARY_RIGHT_SHIFT: return "OP_BINARY_RIGHT_SHIFT";
        case Opcode::OP_BINARY_BOOLEAN_AND: return "OP_BINARY_BOOLEAN_AND";
        case Opcode::OP_BINARY_BOOLEAN_OR: return "OP_BINARY_BOOLEAN_OR";
        case Opcode::OP_BINARY_BOOLEAN_EQUAL: return "OP_BINARY_BOOLEAN_EQUAL";
        case Opcode::OP_BINARY_BOOLEAN_NOT_EQUAL: return "OP_BINARY_BOOLEAN_NOT_EQUAL";
        case Opcode::OP_BINARY_BOOLEAN_GREATER_THAN: return "OP_BINARY_BOOLEAN_GREATER_THAN";
        case Opcode::OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO: return "OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO";
        case Opcode::OP_BINARY_BOOLEAN_LESS_THAN: return "OP_BINARY_BOOLEAN_LESS_THAN";
        case Opcode::OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO: return "OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO";
        case Opcode::OP_CALL: return "OP_CALL";
        case Opcode::OP_RETURN: return "OP_RETURN";
        case Opcode::OP_JUMP: return "OP_JUMP";
        case Opcode::OP_POP_JUMP_IF_FALSE: return "OP_POP_JUMP_IF_FALSE";
        case Opcode::OP_BUILD_TUPLE: return "OP_BUILD_TUPLE";
        case Opcode::OP_UNKNOWN: return "OP_UNKNOWN";
    }
    return "Unknown opcode: " + std::to_string(static_cast<uint32_t>(opcode));
}

std::string Code::to_string() const {
    std::string result;
    for (size_t i = 0; i < code.size(); ++i) {
        auto opcode = static_cast<Opcode>(code[i]);

        result += std::to_string(i) + ": " + Opcode_to_string(opcode) + " ";

        switch (opcode) {
            case Opcode::OP_LOAD_CONST: {
                auto const_index = static_cast<uint32_t>(read32(++i));
                result += std::to_string(const_index);
                i += sizeof(uint32_t) - 1;
                break;
            }
            case Opcode::OP_POP_TOP: break;
            case Opcode::OP_RETURN: break;
            default: throw std::runtime_error("Unknown opcode: " + Opcode_to_string(static_cast<Opcode>(opcode)));
        }
        result += "\n";
    }
    return result;
}

std::string Constant::to_string() const {
    switch (tag) {
        case ConstantTag::C_NULL: return "null";
        case ConstantTag::C_INTEGER: return std::to_string(_int);
        case ConstantTag::C_FLOAT: return std::to_string(_float);
        case ConstantTag::C_ASCII: return "\"" + _string + "\"";
    }
    return "Unknown constant tag: " + std::to_string(static_cast<uint32_t>(tag));
}

}    
