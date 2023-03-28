#pragma once

#include "../parser/ast.h"
#include "../misc/StringArray.h"

enum CompilationStatusCode {
    STATUS_OK,
    STATUS_FAIL,
};

typedef struct CompilationStatus {
    enum CompilationStatusCode code;
    char* what;
} CompilationStatus;

enum ConstantTag {
    CONSTANT_NULL,
    CONSTANT_BOOL,
    CONSTANT_INTEGER,
    CONSTANT_FLOAT,
    CONSTANT_ASCII,
    CONSTANT_CODE,    
};

typedef struct Code {
    StringArray params;
    char* name;
    size_t size;
    uint8_t* code;
} Code;

typedef struct ConstantInformation {
    enum ConstantTag tag;
    union {
        bool    _bool;
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
CompiledModule* new_compiled_module(char* filename, uint16_t major, uint16_t minor);

CompiledModule* new_compiled_module(char* filename, uint16_t major, uint16_t minor);
CompilationStatus compile_module(ast_node_t* root, CompiledModule* compiled_module, Code* code);

void save_module_to_file(CompiledModule* compiled_module, char* filename);
CompiledModule* load_module_from_file(char* filename);

void free_module(CompiledModule* compiled_module);

void free_code(Code code);

void print_code(Code* code, char* end);
void print_constant_information(ConstantInformation* constant_information);
void print_name_pool(NamePool* name_pool);
void print_constant_pool(ConstPool* constant_pool);