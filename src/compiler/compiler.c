#include "compiler.h"

#include "operations.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t find_name_pool(NamePool name_pool, char* name) {
    size_t i = 0;
    for (; i < name_pool.size; ++i) {
        if (strcmp(name, name_pool.data[i]) == 0) {
            break;
        }
    };
    return i;
}

static void update_name_pool(NamePool name_pool, char* name) {
    size_t index = find_name_pool(name_pool, name);
    if (index == name_pool.size) {
        name_pool.data = realloc(name_pool.data, ++name_pool.size * sizeof(char*));       
    } else {
        free(name_pool.data[index]);
    }
    name_pool.data[index] = name;
}

static size_t find_constant_pool(ConstPool constant_pool, ConstantInformation constant_information) {
    size_t i = 0;
    for (; i < constant_pool.size; ++i) {
        if (constant_information.tag == constant_pool.data[i].tag) {
            switch (constant_information.tag) {
                case CONSTANT_ASCII: if (strcmp(constant_information._string, constant_pool.data[i]._string) == 0) return i;
                case CONSTANT_CODE: if (constant_information._code.size == 
                                        constant_pool.data[i]._code.size && 
                                            (memcmp(
                                                constant_information._code.code, 
                                                constant_pool.data[i]._code.code, 
                                                constant_information._code.size) == 0)
                                    ) return i;
                case CONSTANT_INTEGER: if (constant_information._int == constant_pool.data[i]._int) return i;
                case CONSTANT_FLOAT: if (constant_information._float == constant_pool.data[i]._float) return i;
            }
        }
    }
    return i;
}

static void update_constant_pool(ConstPool constant_pool, ConstantInformation constant_information) {
    size_t index = find_constant_pool(constant_pool, constant_information);
    if (index == constant_pool.size) {
        constant_pool.data = realloc(constant_pool.data, ++constant_pool.size * sizeof(ConstantInformation));       
    } else {
        switch (constant_information.tag) {
                case CONSTANT_ASCII: free(constant_pool.data[index]._string); break;
                case CONSTANT_CODE: free(constant_pool.data[index]._code.code); break;
                default: break;
            }
    }
    constant_pool.data[index] = constant_information;
}

static CompiledModule init_compiled_module(char* filename, uint16_t major, uint16_t minor) {
    return (CompiledModule){
        .filename=filename,
        .major_version=major,
        .minor_version=minor,
        .name_pool={0},
        .constant_pool={0},
        .entry={0},
    };
}

CompiledModule* new_compiled_module(char* filename, uint16_t major, uint16_t minor) {
    CompiledModule* compiled_module = malloc(sizeof(CompiledModule));
    if (!compiled_module) {
        fputs("[CompiledModule] - Error: cannot allocate memory.", stderr);
        return NULL;
    }

    *compiled_module = init_compiled_module(filename, major, minor);
    return compiled_module;
}

enum CompilationStatus compile_module(ast_node_t* root, CompiledModule* compiled_module) {
    if (!compiled_module) return STATUS_FAIL;

    switch (root->ast_type) {
        case AST_ROOT:
        case AST_BLOCK: {
            break;
        }

        case AST_ASSIGNMENT: {
            // compiled_module->entry.code;
            if (compile_module(root->ast_assignment.right, compiled_module) != STATUS_OK) {
                return STATUS_FAIL;
            } // LOAD_CONST 0 (3)

            if (root->ast_assignment.left->ast_type != AST_IDENTIFIER) {
                fputs("[CompileModule]: Error - left side is not an identifier", stderr);
                return STATUS_FAIL;
            }

            ast_string_t identifier = root->ast_assignment.left->ast_string;

            char* name = malloc(identifier.length + 1);
            strncpy(name, identifier.value, identifier.length);
            name[identifier.length] = 0;

            
            break;
        }

        case AST_INT: {
            // search in const pool and if there is not occourance then add it. get index
            
            write_8(compiled_module->entry, OP_LOAD_CONST);
            write_64(compiled_module->entry, index);
            break;
        }

        default:
            fprintf(stderr, "Visit for AST from type '%s' is not implemented yet\n", ast_type_to_str(root));
            break;
    }

    return STATUS_OK;
}

static free_name_pool(NamePool name_pool) {
    for (size_t i = 0; i < name_pool.size; ++i) {
        free(name_pool.data[i]);
    }
    free(name_pool.data);
}

static void free_code(Code code) {
    free(code.code);
}

static void free_constant(ConstantInformation constant_info) {
    switch (constant_info.tag) {
        case CONSTANT_INTEGER:
        case CONSTANT_FLOAT: break;

        case CONSTANT_ASCII: free(constant_info._string); break;
        case CONSTANT_CODE: free_code(constant_info._code); break;
    }
}

static void free_constant_pool(ConstPool constant_pool) {
    for (size_t i = 0; i < constant_pool.size; ++i) {
        free_constant(constant_pool.data[i]);
    }

    free(constant_pool.data);
}

void free_module(CompiledModule* compile_module) {
    free(compile_module->filename);
    
    free_name_pool(compile_module->name_pool);
    free_constant_pool(compile_module->constant_pool);
    free_code(compile_module->entry);

    free(compile_module);
}
