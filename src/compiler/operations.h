#pragma once

enum operation {
    // LOADs
    OP_LOAD_CONST,      // direct: constant index, stack: none
    OP_LOAD_NAME,       // direct: name index, stack: none
    OP_LOAD_ATTRIBUTE,  // direct: attribute index, stack: this

    // STOREs
    OP_STORE_NAME,      // direct: name index, stack: object
    OP_STORE_ATTRIBUTE, // direct: attribute index, stack: this, object
    
    // OPERATORs
    OP_UNARY,           // direct: unary operator enum, stack: object
    OP_BINARY,          // direct: binary operator enum, stack: object, object
    
    // FUNCTIONs
    OP_CALL,                // direct: argument count, stack: object, ...
    OP_CALL_METHOD,         // direct: argument count, stack: object, ..., self object
    OP_FUNCTION_DEFNITION,  // direct: none, stack: object
    OP_RETURN,              // direct: none, stack: object

    // LOOPs
    OP_JUMP,                // direct: relative byte offset, stack: none
    OP_POP_JUMP_IF_FALSE,   // direct: relative byte offset, stack: object
};

enum unary_operator {
    UNARY_ADD,
    UNARY_SUB,
    UNARY_NOT,
    UNARY_BOOL_NOT,
};

enum binary_operator {
    BINARY_ADD,
    BINARY_SUB,
    BINARY_MUL,
    BINARY_DIV,
    
    BINARY_AND,
    BINARY_OR,
    BINARY_XOR,
    BINARY_LEFT_SHIFT,
    BINARY_RIGHT_SHIFT,

    BINARY_BOOLEAN_AND,
    BINARY_BOOLEAN_OR,
    BINARY_BOOLEAN_EQUAL,
    BINARY_BOOLEAN_NOT_EQUAL,
    BINARY_BOOLEAN_GREATER_THAN,
    BINARY_BOOLEAN_LESS_THAN,
};