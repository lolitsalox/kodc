#include "compiler.h"

#include "operations.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t write_8(Code* code, uint8_t data) {
    code->code = realloc(code->code, ++code->size * sizeof(uint8_t));
    uint8_t* ptr = (uint8_t*)(code->code + code->size - 1);
    *ptr = data;
    return code->size - 1;
}

static size_t write_16(Code* code, uint16_t data) {
    code->size += 2;
    code->code = realloc(code->code, code->size * sizeof(uint8_t));
    uint16_t* ptr = (uint16_t*)(code->code + code->size - 2);
    *ptr = data;
    return code->size - 2;
}

static size_t write_32(Code* code, uint32_t data) {
    code->size += 4;
    code->code = realloc(code->code, code->size * sizeof(uint8_t));
    uint32_t* ptr = (uint32_t*)(code->code + code->size - 4);
    *ptr = data;
    return code->size - 4;
}

static size_t write_64(Code* code, uint64_t data) {
    code->size += 8;
    code->code = realloc(code->code, code->size * sizeof(uint8_t));
    uint64_t* ptr = (uint64_t*)(code->code + code->size - 8);
    *ptr = data;
    return code->size - 8;
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
        case CONSTANT_NULL: return "CONST_NULL";
        case CONSTANT_BOOL: return "CONST_BOOL";
        case CONSTANT_INTEGER: return "CONST_INTEGER";
        case CONSTANT_FLOAT: return "CONST_FLOAT";
        case CONSTANT_ASCII: return "CONST_ASCII";
        case CONSTANT_CODE: return "CONST_CODE";
    }
    return "CONST_UNKNOWN";
}

void print_code(Code* code, char* end) {
    // for(size_t i = 0; i < code->size; ++i) {
    //     printf("%02x ", code->code[i]);
    // }
    puts(code->name);

    for(size_t i = 0; i < code->size; ++i) {
        printf("\t\t%02lld: ", i);
        enum Operation op = code->code[i];
        switch(op) {
            case OP_POP_JUMP_IF_FALSE:
            case OP_JUMP:
            case OP_LOAD_NAME:
            case OP_LOAD_CONST:
            case OP_STORE_NAME:
            case OP_CALL:
                printf("%s ", op_to_str(op));
                i++;
                if (code->size < i + 8) {
                    fputs("can't print", stderr);
                    return;
                }

                printf("%llu\n", *(uint64_t*)(code->code + i));
                i += 7;
                break;
            case OP_RETURN:
            case OP_UNARY_ADD:
            case OP_UNARY_SUB:
            case OP_UNARY_NOT:
            case OP_UNARY_BOOL_NOT:
            case OP_BINARY_ADD:
            case OP_BINARY_SUB:
            case OP_BINARY_MUL:
            case OP_BINARY_DIV:
            case OP_BINARY_MOD:
            case OP_BINARY_POW:
            case OP_BINARY_AND:
            case OP_BINARY_OR:
            case OP_BINARY_XOR:
            case OP_BINARY_LEFT_SHIFT:
            case OP_BINARY_RIGHT_SHIFT:
            case OP_BINARY_BOOLEAN_AND:
            case OP_BINARY_BOOLEAN_OR:
            case OP_BINARY_BOOLEAN_EQUAL:
            case OP_BINARY_BOOLEAN_NOT_EQUAL:
            case OP_BINARY_BOOLEAN_GREATER_THAN:
            case OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO:
            case OP_BINARY_BOOLEAN_LESS_THAN:
            case OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO:
            case OP_KEEP_ALIVE:
                puts(op_to_str(op));
                break;
            default:
                printf("[print_code] - Warning : can't print operation '%s'\n", op_to_str(code->code[i]));
                return;
        }
    }
    printf(end);
}

void print_constant_information(ConstantInformation* constant_information) {
    printf("%s: ", constant_tag_to_str(constant_information->tag));
    switch (constant_information->tag) {
                case CONSTANT_NULL: printf("null"); break;
                case CONSTANT_BOOL: printf("%d", constant_information->_bool); break;
                case CONSTANT_ASCII: printf("%s", constant_information->_string); break;
                case CONSTANT_CODE: print_code(&constant_information->_code, ""); break;
                case CONSTANT_INTEGER: printf("%lld", constant_information->_int); break;
                case CONSTANT_FLOAT: printf("%f", constant_information->_float); break;
                default: printf("??? (tag=%u)", constant_information->tag); break;
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
                case CONSTANT_NULL: return i;
                case CONSTANT_BOOL: if (constant_information._bool == constant_pool->data[i]._bool) return i; else break;

                case CONSTANT_ASCII: 
                    if (strcmp(constant_information._string, constant_pool->data[i]._string) == 0)
                        return i;
                    break;
                
                case CONSTANT_CODE: 
                    if ( strcmp(constant_information._code.name, constant_pool->data[i]._code.name) == 0 &&
                        constant_information._code.size == constant_pool->data[i]._code.size &&  (
                            memcmp(
                                constant_information._code.code, 
                                constant_pool->data[i]._code.code, 
                                constant_information._code.size) == 0
                            )
                    ) return i;
                    break;
                
                case CONSTANT_INTEGER: 
                    if (constant_information._int == constant_pool->data[i]._int) return i;
                    break;
                
                case CONSTANT_FLOAT: 
                if (constant_information._float == constant_pool->data[i]._float) return i;
                    break;
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
        .entry={.code=NULL, .params=init_string_array(), .size=0}
    };
}

bool keep_alive = false;
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
    if (!compiled_module || !root) return STATUS_FAIL;

    switch (root->ast_type) {
        case AST_ROOT:
        case AST_BLOCK: {
            linked_list_node_t* node = root->ast_compound.head;
            while (node) {
                if (compile_module(node->item, compiled_module, code) != STATUS_OK) return STATUS_FAIL;

                node = node->next;
            }
            break;
        }

        case AST_ASSIGNMENT: {
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

        case AST_IDENTIFIER: {
            char* name = malloc(root->ast_string.length + 1);
            strncpy(name, root->ast_string.value, root->ast_string.length);
            name[root->ast_string.length] = 0;

            size_t index = update_name_pool(
                &compiled_module->name_pool, 
                name
            );            

            write_8(code, OP_LOAD_NAME);
            write_64(code, index);
            break;
        }

        case AST_UNARY_OP: {
            if (compile_module(root->ast_unary_op.value, compiled_module, code) != STATUS_OK) return STATUS_FAIL;
            enum Operation op;
            switch (root->ast_unary_op.op) {
                case TOKEN_ADD: op = OP_UNARY_ADD; break;
                case TOKEN_SUB: op = OP_UNARY_SUB; break; 
                case TOKEN_NOT: op = OP_UNARY_NOT; break; 
                case TOKEN_BOOL_NOT: op = OP_UNARY_BOOL_NOT; break; 
                default: return STATUS_FAIL;
            }
            
            write_8(code, op);
            break;
        }

        case AST_BIN_OP: {
            if (compile_module(root->ast_bin_op.left, compiled_module, code) != STATUS_OK) return STATUS_FAIL;
            if (compile_module(root->ast_bin_op.right, compiled_module, code) != STATUS_OK) return STATUS_FAIL;
            enum Operation op;

            switch (root->ast_bin_op.op) {
                case TOKEN_ADD: op = OP_BINARY_ADD; break;
                case TOKEN_SUB: op = OP_BINARY_SUB; break;
                case TOKEN_MUL: op = OP_BINARY_MUL; break;
                case TOKEN_DIV: op = OP_BINARY_DIV; break;
                case TOKEN_MOD: op = OP_BINARY_MOD; break;
                case TOKEN_POW: op = OP_BINARY_POW; break;
                case TOKEN_AND: op = OP_BINARY_AND; break;
                case TOKEN_OR:  op = OP_BINARY_OR; break;
                case TOKEN_HAT: op = OP_BINARY_XOR; break;
                case TOKEN_SHL: op = OP_BINARY_LEFT_SHIFT; break;
                case TOKEN_SHR: op = OP_BINARY_RIGHT_SHIFT; break;
                case TOKEN_BOOL_AND:    op = OP_BINARY_BOOLEAN_AND; break;
                case TOKEN_BOOL_OR:     op = OP_BINARY_BOOLEAN_OR; break;
                case TOKEN_BOOL_EQ:     op = OP_BINARY_BOOLEAN_EQUAL; break;
                case TOKEN_BOOL_NOTE:   op = OP_BINARY_BOOLEAN_NOT_EQUAL; break;
                case TOKEN_BOOL_GT:     op = OP_BINARY_BOOLEAN_GREATER_THAN; break;
                case TOKEN_BOOL_GTE:    op = OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO; break;
                case TOKEN_BOOL_LT:     op = OP_BINARY_BOOLEAN_LESS_THAN; break;
                case TOKEN_BOOL_LTE:    op = OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO; break;
                default: return STATUS_FAIL;
            }

            write_8(code, op);
            break;
        }

        case AST_IF_STATEMENT: {
            if (compile_module(root->ast_conditional_statement.expression, compiled_module, code) != STATUS_OK) return STATUS_FAIL;
            write_8(code, OP_POP_JUMP_IF_FALSE);
            size_t offset = write_64(code, 0); // temporary
            if (compile_module(root->ast_conditional_statement.body, compiled_module, code) != STATUS_OK) return STATUS_FAIL;
            *(uint64_t*)(code->code + offset) = code->size;
            break;
        }

        case AST_WHILE_STATEMENT: {
            size_t expr_offset = code->size;
            if (compile_module(root->ast_conditional_statement.expression, compiled_module, code) != STATUS_OK) return STATUS_FAIL;

            write_8(code, OP_POP_JUMP_IF_FALSE);
            size_t offset = write_64(code, 0); // temporary

            if (compile_module(root->ast_conditional_statement.body, compiled_module, code) != STATUS_OK) return STATUS_FAIL;
            
            write_8(code, OP_JUMP);
            write_64(code, expr_offset);

            *(uint64_t*)(code->code + offset) = code->size;
            break;
        }
        
        case AST_RETURN_STATEMENT: {
            ast_node_t* value = root->ast_return_statement.value;
            if (!value) {
                // loading null
                size_t index = update_constant_pool(&compiled_module->constant_pool, (ConstantInformation){.tag=CONSTANT_NULL});
                write_8(code, OP_LOAD_CONST);
                write_64(code, index);
            } else if (compile_module(value, compiled_module, code) != STATUS_OK) return STATUS_FAIL;
            
            write_8(code, OP_RETURN);
            break;
        }

        case AST_CALL: {
            linked_list_node_t* curr_arg = root->ast_call.arguments->ast_compound.tail;
            while (curr_arg) {
                if (compile_module((ast_node_t*)curr_arg->item, compiled_module, code) != STATUS_OK) return STATUS_FAIL;
                curr_arg = curr_arg->prev;
            }
            
            if (compile_module(root->ast_call.callable, compiled_module, code) != STATUS_OK) return STATUS_FAIL;
            write_8(code, OP_CALL);
            write_64(code, root->ast_call.arguments->ast_compound.size);
            break;
        }

        case AST_FUNCTION: {
            ConstantInformation constant_code = (ConstantInformation){
                    .tag=CONSTANT_CODE,
                    ._code={{0}},
            };

            linked_list_node_t* curr_param = root->ast_function.parameters->ast_compound.head;
            while (curr_param) {
                ast_node_t* ident = ((ast_node_t*)curr_param->item);
                if (ident->ast_type != AST_IDENTIFIER) {
                    puts("[param is not an identifier]");
                    exit(0);
                }
                
                char* param_name = malloc(ident->ast_string.length + 1);
                strncpy(param_name, ident->ast_string.value, ident->ast_string.length);
                param_name[ident->ast_string.length] = 0;

                update_name_pool(
                    &compiled_module->name_pool,
                    param_name
                );    

                append_string_array(&constant_code._code.params, ident->ast_string.value, ident->ast_string.length);
                curr_param = curr_param->next;
            }
            
            // Compiling function body
            if (compile_module(root->ast_function.body, compiled_module, &constant_code._code) != STATUS_OK) return STATUS_FAIL;

            
            char* fn_name = malloc(root->ast_function.name.length + 1);
            strncpy(fn_name, root->ast_function.name.value, root->ast_function.name.length);
            fn_name[root->ast_function.name.length] = 0;
            
            constant_code._code.name = fn_name;
            
            // Adding code object to constant pool
            size_t index = update_constant_pool(&compiled_module->constant_pool, constant_code);
            
            write_8(code, OP_LOAD_CONST);
            write_64(code, index);

            if (ast_string_compare(root->ast_function.name, (ast_string_t){.value="<anonymous>",.length=sizeof("<anonymous>")}) != 0) {
                fn_name = malloc(root->ast_function.name.length + 1);
                strncpy(fn_name, root->ast_function.name.value, root->ast_function.name.length);
                fn_name[root->ast_function.name.length] = 0;

                index = update_name_pool(
                    &compiled_module->name_pool, 
                    fn_name
                );            

                write_8(code, OP_STORE_NAME);
                write_64(code, index);
            }
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
    free(code.name);
    free_string_array(&code.params);
    free(code.code);
}

static void free_constant(ConstantInformation constant_info) {
    switch (constant_info.tag) {
        case CONSTANT_NULL:
        case CONSTANT_BOOL:
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

void save_module_to_file(CompiledModule* compiled_module, char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        fputs("Error - could not open the file to save the module to", stderr);
        return;
    }
    size_t module_filename_size = strlen(compiled_module->filename) + 1;
    fwrite(&module_filename_size, sizeof(module_filename_size), 1, fp);

    if (fwrite(compiled_module->filename, module_filename_size, 1, fp) != 1) {
        fputs("Error while writing", stderr);
    }
    fwrite(&compiled_module->major_version, sizeof(uint16_t), 1, fp);
    fwrite(&compiled_module->minor_version, sizeof(uint16_t), 1, fp);

    fwrite(&compiled_module->name_pool.size, sizeof(size_t), 1, fp);
    for (size_t i = 0; i < compiled_module->name_pool.size; ++i) {
        fwrite(compiled_module->name_pool.data[i], strlen(compiled_module->name_pool.data[i]) + 1, 1, fp);
    }

    fwrite(&compiled_module->constant_pool.size, sizeof(size_t), 1, fp);
    for (size_t i = 0; i < compiled_module->constant_pool.size; ++i) {
        ConstantInformation ci = compiled_module->constant_pool.data[i];
        fwrite(&ci.tag, sizeof(ci.tag), 1, fp);
        switch (ci.tag) {
            case CONSTANT_NULL: break;
            case CONSTANT_BOOL: fwrite(&ci._bool, sizeof(ci._bool), 1, fp); break;
            case CONSTANT_INTEGER: fwrite(&ci._int, sizeof(ci._int), 1, fp); break;
            case CONSTANT_FLOAT: fwrite(&ci._float, sizeof(ci._float), 1, fp); break;
            case CONSTANT_ASCII: fwrite(ci._string, strlen(ci._string) + 1, 1, fp); break;
            case CONSTANT_CODE: {
                // name
                fwrite(ci._code.name, strlen(ci._code.name) + 1, 1, fp);
                // params (size, strings)
                fwrite(&ci._code.params.size, sizeof(ci._code.params.size), 1, fp);
                for (size_t j = 0; j < ci._code.params.size; ++j) {
                    fwrite(ci._code.params.items[j], strlen(ci._code.params.items[j]) + 1, 1, fp);
                }
                // code (size, uint8 array)
                fwrite(&ci._code.size, sizeof(ci._code.size), 1, fp);
                fwrite(ci._code.code, ci._code.size, 1, fp);
                break;
            }
        }
    }
    fwrite(&compiled_module->entry.size, sizeof(size_t), 1, fp);
    fwrite(compiled_module->entry.code, compiled_module->entry.size, 1, fp);
    fclose(fp);
}