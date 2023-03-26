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
    OP_CALL_METHOD,         // direct: argument count, stack: object, ..., self object
    OP_RETURN,              // direct: none, stack: object

    // LOOPs
    OP_JUMP,                // direct: relative byte offset, stack: none
    OP_POP_JUMP_IF_FALSE,   // direct: relative byte offset, stack: object

    OP_KEEP_ALIVE,
};

char* op_to_str(enum Operation op) {
    switch(op) {
        case OP_LOAD_CONST:         return "LOAD_CONST";       
        case OP_LOAD_NAME:          return "LOAD_NAME";        
        case OP_LOAD_ATTRIBUTE:     return "LOAD_ATTRIBUTE";   
        case OP_STORE_NAME:         return "STORE_NAME";       
        case OP_STORE_ATTRIBUTE:    return "STORE_ATTRIBUTE";  
        case OP_CALL:               return "CALL";               
        case OP_CALL_METHOD:        return "CALL_METHOD";        
        case OP_RETURN:             return "RETURN";             
        case OP_JUMP:               return "JUMP";               
        case OP_POP_JUMP_IF_FALSE:  return "POP_JUMP_IF_FALSE";  
        case OP_UNARY_ADD:          return "UNARY_ADD";
        case OP_UNARY_SUB:          return "UNARY_SUB";
        case OP_UNARY_NOT:          return "UNARY_NOT";
        case OP_UNARY_BOOL_NOT:     return "UNARY_BOOL_NOT";
        case OP_BINARY_ADD:         return "BINARY_ADD";
        case OP_BINARY_SUB:         return "BINARY_SUB";
        case OP_BINARY_MUL:         return "BINARY_MUL";
        case OP_BINARY_DIV:         return "BINARY_DIV";
        case OP_BINARY_MOD:         return "BINARY_MOD";
        case OP_BINARY_POW:         return "BINARY_POW";
        case OP_BINARY_AND:         return "BINARY_AND";
        case OP_BINARY_OR:          return "BINARY_OR";
        case OP_BINARY_XOR:         return "BINARY_XOR";
        case OP_BINARY_LEFT_SHIFT:  return "BINARY_LEFT_SHIFT";
        case OP_BINARY_RIGHT_SHIFT: return "BINARY_RIGHT_SHIFT";
        case OP_BINARY_BOOLEAN_AND: return "BINARY_BOOLEAN_AND";
        case OP_BINARY_BOOLEAN_OR:  return "BINARY_BOOLEAN_OR";
        case OP_BINARY_BOOLEAN_EQUAL:           return "BINARY_BOOLEAN_EQUAL";
        case OP_BINARY_BOOLEAN_NOT_EQUAL:       return "BINARY_BOOLEAN_NOT_EQUAL";
        case OP_BINARY_BOOLEAN_GREATER_THAN:    return "BINARY_BOOLEAN_GREATER_THAN";
        case OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO:    return "BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO";
        case OP_BINARY_BOOLEAN_LESS_THAN:       return "BINARY_BOOLEAN_LESS_THAN";
        case OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO:       return "BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO";
        case OP_KEEP_ALIVE:       return "OP_KEEP_ALIVE";
    }
    return "OP_UNKNOWN";
}