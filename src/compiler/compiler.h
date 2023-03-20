#pragma once

#include "../parser/ast.h"

enum CompilationStatus {
    STATUS_OK,
    STATUS_FAIL,
};

enum ConstantTag {
    CONSTANT_INTEGER,
    CONSTANT_FLOAT,
    CONSTANT_ASCII,
    CONSTANT_CODE,    
};

typedef struct Code {
    size_t size;
    uint8_t* code;
} Code;

typedef struct ConstantInformation {
    enum ConstantTag tag;
    union {
        int64_t _int;
        double  _float;
        char*   _string;
        Code    _code;
    };
} ConstantInformation;

typedef struct NamePool {
    size_t size;
    char** data;
} NamePool;

typedef struct ConstPool {
    size_t size;
    ConstantInformation* data;
} ConstPool;

typedef struct CompiledModule {
    char* filename;
    uint16_t major_version;
    uint16_t minor_version;

    NamePool name_pool;
    ConstPool constant_pool;
    Code entry;
    
} CompiledModule;

//compile, eval, exec
// ...
// >>> x = 5 -> AST(ASSIGNMENT(x, 5))
// >>> print(x)
CompiledModule* new_compiled_module(char* filename, unit16_t major, unit16_t minor);
enum CompilationStatus compile_module(ast_node_t* root, CompiledModule* compiled_module);
void free_module(CompiledModule** compile_module);