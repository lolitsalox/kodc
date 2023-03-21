#pragma once

enum Operation {
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
    OP_FUNCTION_DEFNITION,  // direct: none, stack: code object
    OP_RETURN,              // direct: none, stack: object

    // LOOPs
    OP_JUMP,                // direct: relative byte offset, stack: none
    OP_POP_JUMP_IF_FALSE,   // direct: relative byte offset, stack: object
};

enum UnaryOperator {
    UNARY_ADD,
    UNARY_SUB,
    UNARY_NOT,
    UNARY_BOOL_NOT,
};

enum BinaryOperator {
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

char* op_to_str(enum Operation op) {
    switch(op) {
        case OP_LOAD_CONST:         return "LOAD_CONST";       
        case OP_LOAD_NAME:          return "LOAD_NAME";        
        case OP_LOAD_ATTRIBUTE:     return "LOAD_ATTRIBUTE";   
        case OP_STORE_NAME:         return "STORE_NAME";       
        case OP_STORE_ATTRIBUTE:    return "STORE_ATTRIBUTE";  
        case OP_UNARY:              return "UNARY";            
        case OP_BINARY:             return "BINARY";           
        case OP_CALL:               return "CALL";               
        case OP_CALL_METHOD:        return "CALL_METHOD";        
        case OP_FUNCTION_DEFNITION: return "FUNCTION_DEFNITION"; 
        case OP_RETURN:             return "RETURN";             
        case OP_JUMP:               return "JUMP";               
        case OP_POP_JUMP_IF_FALSE:  return "POP_JUMP_IF_FALSE";  
    }
    return "OP_UNKNOWN";
}