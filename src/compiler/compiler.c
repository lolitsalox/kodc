#include "compiler.h"

#include "operations.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void write_8(Code* code, uint8_t data) {
    code->code = realloc(code->code, ++code->size * sizeof(uint8_t));
    code->code[code->size - 1] = data;
}

static void write_16(Code* code, uint16_t data) {
    code->size += 2;
    code->code = realloc(code->code, code->size * sizeof(uint8_t));
    *(uint16_t*)(code->code + code->size - 2) = data;
}

static void write_32(Code* code, uint32_t data) {
    code->size += 4;
    code->code = realloc(code->code, code->size * sizeof(uint8_t));
    *(uint32_t*)(code->code + code->size - 4) = data;
}

static void write_64(Code* code, uint64_t data) {
    code->size += 8;
    code->code = realloc(code->code, code->size * sizeof(uint8_t));
    *(uint64_t*)(code->code + code->size - 8) = data;
}

void print_name_pool(NamePool* name_pool) {
    puts("NAME POOL:");
    for (size_t i = 0; i < name_pool->size; ++i) {
        printf("\t%lli (%s)\n", i, name_pool->data[i]);
    }
    puts("");
}

static char* constant_tag_to_str(enum ConstantTag constant_tag) {
    switch(constant_tag) {
        case CONSTANT_ASCII: return "CONST_ASCII";
        case CONSTANT_CODE: return "CONST_CODE";
        case CONSTANT_INTEGER: return "CONST_INTEGER";
        case CONSTANT_FLOAT: return "CONST_FLOAT";
    }
    return "CONST_UNKNOWN";
}

void print_code(Code* code, char* end) {
    for(size_t i = 0; i < code->size; ++i) {
        printf("%02x ", code->code[i]);
    }
    puts("");

    for(size_t i = 0; i < code->size; ++i) {
        enum Operation op = code->code[i];
        switch(op) {
            case OP_LOAD_CONST:
            case OP_STORE_NAME:
                printf("%s ", op_to_str(op));
                i++;
                if (code->size < i + 8) {
                    fputs("can't print", stderr);
                    return;
                }

                printf("%llu\n", *(uint64_t*)(code->code + i));
                i += 7;
                break;
            case OP_FUNCTION_DEFNITION:
                printf("%s ", op_to_str(op));
                i++;
                if (code->size < i + 8) {
                    fputs("can't print", stderr);
                    return;
                }

                printf("%llu\n", *(uint64_t*)(code->code + i));
                // FUNCTION_DEFINITION should take no direct arguments
                // create a function object on the stack
                // because you only need to know how many arguments to give when calling
                // TODO: ok let's have code objects store their uh parameter names
                // that too hand in hand in the Code struct right? ye


                // LOAD_CONST
                // FUNCTION_DEFINITION
                // because function definition makes a (callable) function out of the code object
                // STORE_NAME (actual functions only)
                i += 7;
                break;
            default:
                printf("[print_code] - Warning : can't print operation %hhu\n", code->code[i]);
                return;
        }
    }
    printf("%s", end);
}

void print_constant_information(ConstantInformation* constant_information) {
    printf("%s: ", constant_tag_to_str(constant_information->tag));
    switch (constant_information->tag) {
                case CONSTANT_ASCII: printf("%s", constant_information->_string); break;
                case CONSTANT_CODE: print_code(&constant_information->_code, ""); break;
                case CONSTANT_INTEGER: printf("%lld", constant_information->_int); break;
                case CONSTANT_FLOAT: printf("%f", constant_information->_float); break;
                default: printf("??? %u", constant_information->tag); break;
            }
}

void print_constant_pool(ConstPool* constant_pool) {
    puts("CONSTANT POOL:");
    for (size_t i = 0; i < constant_pool->size; ++i) {
        printf("\t%lli (", i);
        print_constant_information(&constant_pool->data[i]);
        puts(")");
    }
    puts("");
}

static size_t find_name_pool(NamePool* name_pool, char* name) {
    size_t i = 0;
    for (; i < name_pool->size; ++i) {
        if (strcmp(name, name_pool->data[i]) == 0) {
            break;
        }
    };
    return i;
}

/*
This function updates a name pool with a new name. If the name is not already in the pool, it is added. 
If the name is already in the pool, it is removed and the new name is added in its place. 

@param name_pool: the name pool to update
@param name: the new name to add

@return the index of the name in the pool
*/
static size_t update_name_pool(NamePool* name_pool, char* name) {
    // Find the index of the name in the name pool, if it exists
    size_t index = find_name_pool(name_pool, name);

    // If the name is not already in the pool, add it
    if (index == name_pool->size) {
        // Reallocate memory for the data array to make room for the new name
        name_pool->data = realloc(name_pool->data, ++name_pool->size * sizeof(char*));       
    } 
    // If the name is already in the pool, remove it and add the new name in its place
    else {
        // Free the memory for the old name
        free(name_pool->data[index]);
    }

    // Add the new name at the appropriate index
    name_pool->data[index] = name;
    return index;
}

static size_t find_constant_pool(ConstPool* constant_pool, ConstantInformation constant_information) {
    size_t i = 0;
    for (; i < constant_pool->size; ++i) {
        if (constant_information.tag == constant_pool->data[i].tag) {
            switch (constant_information.tag) {
                case CONSTANT_ASCII: if (strcmp(constant_information._string, constant_pool->data[i]._string) == 0) return i;
                case CONSTANT_CODE: if (constant_information._code.size == 
                                        constant_pool->data[i]._code.size && 
                                            (memcmp(
                                                constant_information._code.code, 
                                                constant_pool->data[i]._code.code, 
                                                constant_information._code.size) == 0)
                                    ) return i;
                case CONSTANT_INTEGER: if (constant_information._int == constant_pool->data[i]._int) return i;
                case CONSTANT_FLOAT: if (constant_information._float == constant_pool->data[i]._float) return i;
            }
        }
    }
    return i;
}

static size_t update_constant_pool(ConstPool* constant_pool, ConstantInformation constant_information) {
    size_t index = find_constant_pool(constant_pool, constant_information);
    if (index == constant_pool->size) {
        constant_pool->data = realloc(constant_pool->data, ++constant_pool->size * sizeof(ConstantInformation));    
    } else {
        switch (constant_information.tag) {
                case CONSTANT_ASCII: free(constant_pool->data[index]._string); break;
                case CONSTANT_CODE: free(constant_pool->data[index]._code.code); break;
                default: break;
            }
    }
    constant_pool->data[index] = constant_information;
    return index;
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

enum CompilationStatus compile_module(ast_node_t* root, CompiledModule* compiled_module, Code* code) {
    if (!compiled_module) return STATUS_FAIL;

    switch (root->ast_type) {
        case AST_ROOT:
        case AST_BLOCK: {
            // the AST_ROOT and AST_BLOCK should have different instructions made though
            // because of LOAD_FAST and STORE_FAST
            // and AST_BLOCK exists for statements as well as for functions
            linked_list_node_t* node = root->ast_compound.head;
            while (node) {
                if (compile_module(node->item, compiled_module, code) != STATUS_OK) return STATUS_FAIL;

                node = node->next;
            }
            break;
        }

        case AST_ASSIGNMENT: {
            // compiled_module->entry.code;
            if (compile_module(root->ast_assignment.right, compiled_module, code) != STATUS_OK) {
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

            size_t index = update_name_pool(&compiled_module->name_pool, name);

            write_8(code, OP_STORE_NAME);
            write_64(code, index);
            break;
        }

        case AST_INT: {
            size_t index = update_constant_pool(&compiled_module->constant_pool, (ConstantInformation){.tag=CONSTANT_INTEGER,._int=root->ast_int});            
            write_8(code, OP_LOAD_CONST);
            write_64(code, index);
            break;
        }
        
        case AST_FLOAT: {
            size_t index = update_constant_pool(
                &compiled_module->constant_pool, 
                (ConstantInformation){
                    .tag=CONSTANT_FLOAT,
                    ._float=root->ast_float
                }
            );            
            write_8(code, OP_LOAD_CONST);
            write_64(code, index);
            break;
        }
        
        case AST_STRING: {
            char* str = malloc(root->ast_string.length + 1);
            strncpy(str, root->ast_string.value, root->ast_string.length);
            str[root->ast_string.length] = 0;

            size_t index = update_constant_pool(
                &compiled_module->constant_pool, 
                (ConstantInformation){
                    .tag=CONSTANT_ASCII,
                    ._string=str
                }
            );            
            write_8(code, OP_LOAD_CONST);
            write_64(code, index);
            break;
        }

        case AST_FUNCTION: {
            ConstantInformation constant_code = (ConstantInformation){
                    .tag=CONSTANT_CODE,
                    ._code={0},
            };
            
            if (compile_module(root->ast_function.body, compiled_module, &constant_code._code) != STATUS_OK) return STATUS_FAIL;
            size_t index = update_constant_pool(&compiled_module->constant_pool, constant_code);
            
            write_8(code, OP_LOAD_CONST);
            write_64(code, index);
            
            // I'm saying make FUNCTION_DEFINITION take no direct arguments, only a single stack argument (the code object to make a function object from)
            // 
            // define function
            write_8(code, OP_FUNCTION_DEFNITION);
            write_64(code, root->ast_function.parameters->ast_compound.size); // param size

            char* fn_name = malloc(root->ast_function.name.length + 1);
            strncpy(fn_name, root->ast_function.name.value, root->ast_function.name.length);
            fn_name[root->ast_function.name.length] = 0;

            index = update_name_pool(
                &compiled_module->name_pool, 
                fn_name
            );            

            write_8(code, OP_STORE_NAME);
            write_64(code, index); // fn name index
            break;
        }

        default:
            fprintf(stderr, "Visit for AST from type '%s' is not implemented yet\n", ast_type_to_str(root->ast_type));
            break;
    }

    return STATUS_OK;
}

static void free_name_pool(NamePool name_pool) {
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
