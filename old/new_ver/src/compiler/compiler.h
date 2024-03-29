#pragma once

#include "../defines.h"
#include "../parser/ast.h"

typedef struct CompilationStatus {
    enum STATUS code;
    char* what;
} CompilationStatus;

enum ConstantTag {
    CONSTANT_NULL,
    CONSTANT_BOOL,
    CONSTANT_INTEGER,
    CONSTANT_FLOAT,
    CONSTANT_ASCII,
    CONSTANT_CODE,    
    CONSTANT_TUPLE,
};

typedef struct NamePool {
    size_t size;
    char** data;
} NamePool;

typedef struct KodDictObject KodDictObject; 

typedef struct Code {
    NamePool params;
    char* name;
    size_t size;
    uint8_t* code;
    KodDictObject* parent_closure;
} Code;

typedef struct ConstantInformation ConstantInformation;

typedef struct ConstPool {
    size_t size;
    ConstantInformation* data;
    bool is_set;
} ConstPool;

struct ConstantInformation {
    enum ConstantTag tag;
    union {
        bool    _bool;
        i64     _int;
        f64     _float;
        char*   _string;
        Code    _code;
        ConstPool _tuple;
        
    };
};


typedef struct CompiledModule {
    char* filename;
    uint16_t major_version;
    uint16_t minor_version;

    NamePool name_pool;
    ConstPool constant_pool;
    Code entry;
    
} CompiledModule;

char* constant_tag_to_str(enum ConstantTag constant_tag);

CompiledModule* new_compiled_module(char* filename, uint16_t major, uint16_t minor);
CompilationStatus compile_module(AstNode* root, CompiledModule* compiled_module, Code* code);

// void save_module_to_file(CompiledModule* compiled_module, char* filename);
// CompiledModule* load_module_from_file(char* filename);

void free_module(CompiledModule* compiled_module);
void free_code(Code code);

void print_code(Code* code, char* end, ConstPool* constant_pool, NamePool* name_pool);

void print_constant_information(ConstantInformation* constant_information);

void print_name_pool(NamePool* name_pool);
void print_constant_pool(ConstPool* constant_pool);

Status name_pool_get(NamePool* name_pool, size_t index, char** out);
Status constant_pool_get(ConstPool* constant_pool, size_t index, ConstantInformation* out);