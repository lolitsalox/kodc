#pragma once

#include <cstdint>
#include <unordered_set>
#include <vector>
#include <string>
#include <stdexcept>

namespace kod {

struct Code {
    std::string name;
    std::vector<std::string> params;
    std::vector<uint8_t> code;

    std::string to_string() const;

    bool operator==(const Code& other) const {
        if (name != other.name) return false;
        if (params != other.params) return false;
        if (code != other.code) return false;
        return true;
    }

    void write8(uint8_t value) {
        code.push_back(value);
    }

    void write16(uint16_t value) {
        code.push_back(value & 0xFF);
        code.push_back((value >> 8) & 0xFF);
    }

    void write32(uint32_t value) {
        code.push_back(value & 0xFF);
        code.push_back((value >> 8) & 0xFF);
        code.push_back((value >> 16) & 0xFF);
        code.push_back((value >> 24) & 0xFF);
    }

    void write64(uint64_t value) {
        code.push_back(value & 0xFF);
        code.push_back((value >> 8) & 0xFF);
        code.push_back((value >> 16) & 0xFF);
        code.push_back((value >> 24) & 0xFF);
        code.push_back((value >> 32) & 0xFF);
        code.push_back((value >> 40) & 0xFF);
        code.push_back((value >> 48) & 0xFF);
        code.push_back((value >> 56) & 0xFF);
    }

    uint32_t read32(size_t offset) const {
        // TODO: check bounds
        if (offset + 3 >= code.size()) {
            throw std::out_of_range("Offset out of bounds");
        }

        uint32_t result = 0;
        result |= code[offset] & 0xFF;
        result |= (code[offset + 1] & 0xFF) << 8;
        result |= (code[offset + 2] & 0xFF) << 16;
        result |= (code[offset + 3] & 0xFF) << 24;
        return result;
    }
};

enum class ConstantTag : uint8_t {
    C_NULL,
    C_BOOL,
    C_INTEGER,
    C_FLOAT,
    C_ASCII,
    C_CODE,    
    C_TUPLE,
};

struct Constant {
    ConstantTag tag;
        bool        _bool;
        int64_t     _int;
        double      _float;
        std::string   _string;
        Code        _code;
        std::vector<Constant> _tuple;

    std::string to_string() const;
    bool operator==(const Constant& other) const {
        if (tag != other.tag) return false;

        switch (tag) {
            case ConstantTag::C_NULL: return true;
            case ConstantTag::C_BOOL: return _bool == other._bool;
            case ConstantTag::C_INTEGER: return _int == other._int;
            case ConstantTag::C_FLOAT: return _float == other._float;
            case ConstantTag::C_ASCII: return _string == other._string;
            case ConstantTag::C_CODE: return _code == other._code;
            case ConstantTag::C_TUPLE: return _tuple == other._tuple;
            default: return false;
        }
    }

    Constant(ConstantTag tag) : tag(tag) {}
    Constant(int64_t value) : tag(ConstantTag::C_INTEGER), _int(value) {}
    Constant(double value) : tag(ConstantTag::C_FLOAT), _float(value) {}
    Constant(const std::string& value) : tag(ConstantTag::C_ASCII), _string(value) {}
    Constant(const Code& value) : tag(ConstantTag::C_CODE), _code(value) {}
    ~Constant() {}

};

struct CompiledModule {
    std::string filename;
    std::unordered_set<std::string> name_pool;  
    std::vector<Constant> constant_pool;
    Code entry;

    CompiledModule(const std::string& filename)
        : filename(filename) {}

};

enum class Opcode : uint8_t {
    // LOADs
    OP_LOAD_CONST,      // direct: constant index, stack: none
    OP_LOAD_NAME,       // direct: name index, stack: none
    OP_LOAD_ATTRIBUTE,  // direct: attribute index, stack: this
    OP_LOAD_METHOD,     // direct: attribute index, stack: this

    // STOREs
    OP_STORE_NAME,      // direct: name index, stack: object
    OP_STORE_ATTRIBUTE, // direct: attribute index, stack: this, object

    // YEETs
    OP_POP_TOP,
    
    // OPERATORs
    OP_UNARY_ADD,
    OP_UNARY_SUB,
    OP_UNARY_NOT,
    OP_UNARY_BOOL_NOT,

    OP_BINARY_ADD,
    OP_BINARY_SUB,
    OP_BINARY_MUL,
    OP_BINARY_DIV,
    OP_BINARY_MOD,
    OP_BINARY_POW,

    OP_BINARY_AND,
    OP_BINARY_OR,
    OP_BINARY_XOR,
    OP_BINARY_LEFT_SHIFT,
    OP_BINARY_RIGHT_SHIFT,

    OP_BINARY_BOOLEAN_AND,
    OP_BINARY_BOOLEAN_OR,
    OP_BINARY_BOOLEAN_EQUAL,
    OP_BINARY_BOOLEAN_NOT_EQUAL,
    OP_BINARY_BOOLEAN_GREATER_THAN,
    OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO,
    OP_BINARY_BOOLEAN_LESS_THAN,
    OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO,
    
    // FUNCTIONs
    OP_CALL,                // direct: argument count, stack: object, ...
    OP_RETURN,              // direct: none, stack: object

    // LOOPs
    OP_JUMP,                // direct: relative byte offset, stack: none
    OP_POP_JUMP_IF_FALSE,   // direct: relative byte offset, stack: object

    OP_BUILD_TUPLE,

    OP_UNKNOWN,
};

std::string Opcode_to_string(Opcode opcode);

}