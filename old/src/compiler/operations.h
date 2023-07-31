#pragma once

enum Operation {
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
};

char* op_to_str(enum Operation op);