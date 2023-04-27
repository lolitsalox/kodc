#include "compiler.h"

#include <operations.h>

char* constant_tag_to_str(enum ConstantTag constant_tag) {
    switch (constant_tag) {
        case CONSTANT_NULL: return "CONSTANT_NULL";
        case CONSTANT_BOOL: return "CONSTANT_BOOL";
        case CONSTANT_INTEGER: return "CONSTANT_INTEGER";
        case CONSTANT_FLOAT: return "CONSTANT_FLOAT";
        case CONSTANT_ASCII: return "CONSTANT_ASCII";
        case CONSTANT_CODE: return "CONSTANT_CODE";
        case CONSTANT_TUPLE: return "CONSTANT_TUPLE";
    }
    return "CONSTANT_UNKNOWN";
}

// static size_t read_until_null(FILE* fp, char** buffer) {
//     char c = 0;
//     char* data = calloc(1, sizeof(char));
//     size_t size = 1;
//     while ((c = fgetc(fp))) {
//         data = realloc(data, ++size);
//         data[size-2] = c;
//     }
//     data[size-1] = 0;
//     *buffer = data;
//     return size;
// }

// static char* constant_tag_to_str(enum ConstantTag constant_tag);

// static ConstantInformation read_constant(FILE* fp) {
//     ConstantInformation constant;
//     constant.tag = CONSTANT_NULL;

//     fread(&constant.tag, sizeof(constant.tag), 1, fp);
//     switch (constant.tag) {
//         case CONSTANT_NULL: break;
//         case CONSTANT_BOOL:
//             fread(&constant._bool, sizeof(constant._bool), 1, fp);
//             break;
//         case CONSTANT_INTEGER:
//             fread(&constant._int, sizeof(constant._int), 1, fp);
//             break;
//         case CONSTANT_FLOAT:
//             fread(&constant._float, sizeof(constant._float), 1, fp);
//             break;
//         case CONSTANT_ASCII:
//             read_until_null(fp, &constant._string);
//             break;
//         case CONSTANT_CODE:
//             read_until_null(fp, &constant._code.name);
//             size_t params_size = 0;
//             fread(&params_size, sizeof(size_t), 1, fp);
//             constant._code.params = {0};
//             for (size_t i = 0; i < params_size; ++i) {
//                 String param;
//                 size_t len = read_until_null(fp, &param);
//                 append_string_array(&constant._code.params, param, len);
//             }
//             fread(&constant._code.size, sizeof(size_t), 1, fp);
//             constant._code.code = malloc(constant._code.size);
//             fread(constant._code.code, constant._code.size, 1, fp);
//             constant._code.parent_closure = NULL;
//             break;
//         default:
//             puts("[read_constant] weird constant tag");
//             constant.tag = CONSTANT_NULL;
//             break;
//     }
//     return constant;
// }

static size_t write_data(Code* code, void* data, size_t size) {
    code->size += size;
    code->code = realloc(code->code, code->size);
    void* ptr = code->code + code->size - size;
    memcpy(ptr, data, size);
    return code->size - size;
}

static size_t write_8(Code* code, u8 data) {
    return write_data(code, &data, sizeof(u8));
}

// static size_t write_16(Code* code, u16 data) {
//     return write_data(code, &data, sizeof(u16));
// }

// static size_t write_32(Code* code, u32 data) {
//     return write_data(code, &data, sizeof(u32));
// }

// static size_t write_64(Code* code, u64 data) {
//     return write_data(code, &data, sizeof(u64));
// }

void print_name_pool(NamePool* name_pool) {
    puts("NAME POOL:");
    for (size_t i = 0; i < name_pool->size; ++i) {
        printf("\t%lli (%s)\n", i, name_pool->data[i]);
    }
    puts("");
}

// static char* constant_tag_to_str(enum ConstantTag constant_tag) {
//     switch(constant_tag) {
//         case CONSTANT_NULL: return "CONST_NULL";
//         case CONSTANT_BOOL: return "CONST_BOOL";
//         case CONSTANT_INTEGER: return "CONST_INTEGER";
//         case CONSTANT_FLOAT: return "CONST_FLOAT";
//         case CONSTANT_ASCII: return "CONST_ASCII";
//         case CONSTANT_CODE: return "CONST_CODE";
//     }
//     return "CONST_UNKNOWN";
// }

void print_bytecode(Code* code, char* end, ConstPool* constant_pool, NamePool* name_pool) {
    for(size_t i = 0; i < code->size; ++i) {
        printf("\t%3lld ", i);
        enum Operation op = code->code[i];
        switch(op) {
            case OP_POP_JUMP_IF_FALSE:
            case OP_JUMP:
            case OP_LOAD_NAME:
            case OP_LOAD_CONST:
            case OP_STORE_NAME:
            case OP_CALL:
            case OP_LOAD_METHOD:
            case OP_LOAD_ATTRIBUTE:
            case OP_BUILD_TUPLE:
            case OP_STORE_ATTRIBUTE:
                printf("%-25s", op_to_str(op));
                i++;
                if (code->size < i + sizeof(u8)) {
                    fputs("can't print", stderr);
                    return;
                }
                u8 index = *(u8*)(code->code + i);
                printf("%u", index);
                switch (op) {
                    case OP_LOAD_NAME:
                    case OP_STORE_NAME:
                    case OP_LOAD_METHOD:
                    case OP_LOAD_ATTRIBUTE:
                    case OP_STORE_ATTRIBUTE:
                        if (name_pool)
                            printf(" (%s)", name_pool->data[index]);
                        break;
                    case OP_LOAD_CONST:
                        // <CODE object at 0x00ff>
                        if (constant_pool) {
                            printf(" (");
                            print_constant_information(constant_pool->data + index);
                            printf(")");
                        }
                        break;
                    default: break;
                }
                puts("");
                i += sizeof(u8) - 1;
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
            case OP_POP_TOP:
                puts(op_to_str(op));
                break;
            default:
                printf("[print_code] - Warning : can't print operation '%s'\n", op_to_str(code->code[i]));
                return;
        }
    }
    printf(end);
}

void print_code(Code* code, char* end, ConstPool* constant_pool, NamePool* name_pool) {
    if (code->name)
        puts(code->name);

    print_bytecode(code, "", constant_pool, name_pool);

    for(size_t i = 0; i < code->size; ++i) {
        enum Operation op = code->code[i];
        switch(op) {
            case OP_POP_JUMP_IF_FALSE:
            case OP_JUMP:
            case OP_LOAD_NAME:
            case OP_STORE_NAME:
            case OP_LOAD_METHOD:
            case OP_LOAD_ATTRIBUTE:
            case OP_STORE_ATTRIBUTE:
            case OP_CALL:
            case OP_LOAD_CONST: {
                i++;
                if (op == OP_LOAD_CONST && constant_pool) {
                    u8 index = *(u8*)(code->code + i);
                    if (constant_pool->data[index].tag == CONSTANT_CODE) {
                        printf("\nDisassembly of ");
                        print_constant_information(constant_pool->data + index);
                        puts("");
                        print_bytecode(&constant_pool->data[index]._code, "", constant_pool, name_pool);
                    }
                }
                i += sizeof(u8) - 1;
                break;
            }
            default: break;
        }
    }
    printf(end);
}

void print_constant_information(ConstantInformation* constant_information) {
    // printf("%s: ", constant_tag_to_str(constant_information->tag));
    switch (constant_information->tag) {
                case CONSTANT_NULL: printf("null"); break;
                case CONSTANT_BOOL: printf("%s", constant_information->_bool ? "true" : "false"); break;
                case CONSTANT_ASCII: printf("%s", constant_information->_string); break;
                case CONSTANT_CODE: printf("<code object %s at %p>", constant_information->_code.name, &constant_information->_code); break;
                case CONSTANT_INTEGER: printf("%lld", constant_information->_int); break;
                case CONSTANT_FLOAT: printf("%g", constant_information->_float); break;
                case CONSTANT_TUPLE: 
                    printf("(");
                    for (size_t i = 0; i < constant_information->_tuple.size; ++i) {
                        print_constant_information(&constant_information->_tuple.data[i]);
                        if (i != constant_information->_tuple.size - 1) printf(", ");
                    }
                    printf(")");
                    break;
                default: printf("tag=%u", constant_information->tag); break;
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

                case CONSTANT_TUPLE:
                    break; // doesnt 
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
                case CONSTANT_ASCII: 
                    if (constant_pool->data[index]._string) free(constant_pool->data[index]._string); 
                    break;
                case CONSTANT_CODE: 
                    if (constant_pool->data[index]._code.code) free(constant_pool->data[index]._code.code); 
                    break;
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
        .entry={.code=NULL, .params={0}, .size=0}
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

CompilationStatus compile_module(AstNode* root, CompiledModule* compiled_module, Code* code) {
    if (!compiled_module) return (CompilationStatus){.code=STATUS_FAIL, .what="compiled_module is null"};
    if (!root) return (CompilationStatus){.code=STATUS_FAIL, .what="ast node 'root' is null"};

    CompilationStatus status = {.code=STATUS_OK, .what="Compiled successfully!"};

    switch (root->type) {
        case AST_ROOT:
        case AST_BLOCK: {
            AstListNode* list_node = root->_list.head;
            while (list_node) {
                if (!list_node) {
                    size_t index = update_constant_pool(&compiled_module->constant_pool, (ConstantInformation){.tag=CONSTANT_NULL});            
                    write_8(code, OP_LOAD_CONST);
                    write_8(code, index);
                    write_8(code, OP_RETURN);
                    break;
                }
                status = compile_module(list_node->node, compiled_module, code);
                if (status.code == STATUS_FAIL) return status; 
                
                switch (((AstNode*)list_node->node)->type) {
                    case AST_ASSIGNMENT:
                    case AST_STORE_ATTR:
                    case AST_IF_STATEMENT:
                    case AST_WHILE_STATEMENT:
                    case AST_FUNCTION:
                    case AST_RETURN_STATEMENT:
                    case AST_BLOCK:
                    case AST_ROOT:
                        break;
                    default:
                        write_8(code, OP_POP_TOP);
                }
                list_node = list_node->next;
            }

            if (root->_list.size == 0 || (root->_list.tail && root->_list.tail->node->type != AST_RETURN_STATEMENT)) {
                size_t index = update_constant_pool(&compiled_module->constant_pool, (ConstantInformation){.tag=CONSTANT_NULL});            
                write_8(code, OP_LOAD_CONST);
                write_8(code, index);
                write_8(code, OP_RETURN);
            }
            break;
        }

        case AST_ASSIGNMENT: {
            status = compile_module(root->_assignment.right, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;

            if (root->_assignment.left->type != AST_IDENTIFIER) {
                return (CompilationStatus){.code=STATUS_FAIL,.what="[CompileModule]: Error - left side is not an identifier"};
            }

            char* identifier = root->_assignment.left->_string;
            size_t ident_size = strlen(identifier) + 1;

            char* name = malloc(ident_size);
            memcpy(name, identifier, ident_size);

            size_t index = update_name_pool(&compiled_module->name_pool, name);

            write_8(code, OP_STORE_NAME);
            write_8(code, index);
            break;
        }

        case AST_STORE_ATTR: {
            status = compile_module(root->_store_attr.right, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;

            status = compile_module(root->_store_attr.left->_access.value, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;

            if (root->_store_attr.left->_access.field->type != AST_IDENTIFIER) {
                return (CompilationStatus){.code=STATUS_FAIL,.what="[CompileModule]: Error - attribute is not an identifier"};
            }

            char* identifier = root->_store_attr.left->_access.field->_string;
            size_t ident_size = strlen(identifier) + 1;

            char* name = malloc(ident_size);
            memcpy(name, identifier, ident_size);

            size_t index = update_name_pool(&compiled_module->name_pool, name);

            write_8(code, OP_STORE_ATTRIBUTE);
            write_8(code, index);
            break;
        }

        case AST_NULL: {
            size_t index = update_constant_pool(&compiled_module->constant_pool, (ConstantInformation){.tag=CONSTANT_NULL});            
            write_8(code, OP_LOAD_CONST);
            write_8(code, index);
            break;
        }
        
        case AST_INT: {
            size_t index = update_constant_pool(&compiled_module->constant_pool, (ConstantInformation){.tag=CONSTANT_INTEGER,._int=root->_int});            
            write_8(code, OP_LOAD_CONST);
            write_8(code, index);
            break;
        }
        
        case AST_BOOL: {
            size_t index = update_constant_pool(&compiled_module->constant_pool, (ConstantInformation){.tag=CONSTANT_BOOL,._bool=root->_bool});            
            write_8(code, OP_LOAD_CONST);
            write_8(code, index);
            break;
        }
        
        case AST_FLOAT: {
            size_t index = update_constant_pool(
                &compiled_module->constant_pool, 
                (ConstantInformation){
                    .tag=CONSTANT_FLOAT,
                    ._float=root->_float
                }
            );            
            write_8(code, OP_LOAD_CONST);
            write_8(code, index);
            break;
        }
        
        case AST_STRING: {
            size_t string_size = strlen(root->_string) + 1;

            char* str = malloc(string_size);
            memcpy(str, root->_string, string_size);

            size_t index = update_constant_pool(
                &compiled_module->constant_pool, 
                (ConstantInformation){
                    .tag=CONSTANT_ASCII,
                    ._string=str
                }
            );            
            write_8(code, OP_LOAD_CONST);
            write_8(code, index);
            break;
        }

        case AST_IDENTIFIER: {
            size_t ident_size = strlen(root->_string) + 1;

            char* name = malloc(ident_size);
            memcpy(name, root->_string, ident_size);

            size_t index = update_name_pool(
                &compiled_module->name_pool, 
                name
            );            

            write_8(code, OP_LOAD_NAME);
            write_8(code, index);
            break;
        }

        case AST_UNARY_OP: {
            status = compile_module(root->_unary_op.value, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;
            enum Operation op;
            switch (root->_unary_op.op) {
                case TOKEN_ADD: op = OP_UNARY_ADD; break;
                case TOKEN_SUB: op = OP_UNARY_SUB; break; 
                case TOKEN_NOT: op = OP_UNARY_NOT; break; 
                case TOKEN_BOOL_NOT: op = OP_UNARY_BOOL_NOT; break; 
                default: return (CompilationStatus){.code=STATUS_FAIL, "congratulations, you've reached the unreachable"};
            }
            
            write_8(code, op);
            break;
        }

        case AST_BIN_OP: {
            status = compile_module(root->_binary_op.left, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;
            
            status = compile_module(root->_binary_op.right, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;
            enum Operation op;

            switch (root->_binary_op.op) {
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
                case TOKEN_BOOL_NE:     op = OP_BINARY_BOOLEAN_NOT_EQUAL; break;
                case TOKEN_BOOL_GT:     op = OP_BINARY_BOOLEAN_GREATER_THAN; break;
                case TOKEN_BOOL_GTE:    op = OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO; break;
                case TOKEN_BOOL_LT:     op = OP_BINARY_BOOLEAN_LESS_THAN; break;
                case TOKEN_BOOL_LTE:    op = OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO; break;
                default: return (CompilationStatus){.code=STATUS_FAIL, "[CompileModule]: Error - dont know how to compile this op"};
            }

            write_8(code, op);
            break;
        }

        case AST_IF_STATEMENT: {
            status = compile_module(root->_conditional.expr, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;

            write_8(code, OP_POP_JUMP_IF_FALSE);
            size_t offset = write_8(code, 0); // temporary

            status = compile_module(root->_conditional.body, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;

            *(u8*)(code->code + offset) = code->size;
            break;
        }

        case AST_WHILE_STATEMENT: {
            size_t expr_offset = code->size;
            status = compile_module(root->_conditional.expr, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;

            write_8(code, OP_POP_JUMP_IF_FALSE);
            size_t offset = write_8(code, 0); // temporary

            status = compile_module(root->_conditional.body, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;
            
            write_8(code, OP_JUMP);
            write_8(code, expr_offset);

            *(u8*)(code->code + offset) = code->size;
            break;
        }
        
        case AST_RETURN_STATEMENT: {
            AstNode* value = root->_return;
            if (!value) {
                // loading null
                size_t index = update_constant_pool(&compiled_module->constant_pool, (ConstantInformation){.tag=CONSTANT_NULL});
                write_8(code, OP_LOAD_CONST);
                write_8(code, index);
            } else {
                status = compile_module(value, compiled_module, code);
                if (status.code == STATUS_FAIL) return status;
            }
            
            write_8(code, OP_RETURN);
            break;
        }

        case AST_CALL: {
            AstListNode* curr_arg = root->_call.args->_list.tail;
            
            while (curr_arg) {
                status = compile_module((AstNode*)curr_arg->node, compiled_module, code);
                if (status.code == STATUS_FAIL) return status;
            
                curr_arg = curr_arg->prev;
            }
            
            status = compile_module(root->_call.callable, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;

            write_8(code, OP_CALL);
            write_8(code, root->_call.args->_list.size);
            break;
        }

        case AST_ACCESS: {
            status = compile_module(root->_access.value, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;

            // root->_access.field must be an identifier
            size_t name_size = strlen(root->_access.field->_string) + 1;

            char* name = malloc(name_size);
            memcpy(name, root->_access.field->_string, name_size);

            size_t index = update_name_pool(
                &compiled_module->name_pool, 
                name
            );       

            write_8(code, OP_LOAD_ATTRIBUTE);
            write_8(code, index);
            break;
        }

        case AST_METHOD_CALL: {
            AstListNode* curr_arg = root->_method_call.args->_list.tail;

            while (curr_arg) {
                status = compile_module((AstNode*)curr_arg->node, compiled_module, code);
                if (status.code == STATUS_FAIL) return status;
            
                curr_arg = curr_arg->prev;
            }
            
            status = compile_module(root->_method_call.self, compiled_module, code);
            if (status.code == STATUS_FAIL) return status;

            // root->_method_call.callable must be an identifier
            size_t name_size = strlen(root->_method_call.callable->_string) + 1;

            char* name = malloc(name_size);
            memcpy(name, root->_method_call.callable->_string, name_size);

            size_t index = update_name_pool(
                &compiled_module->name_pool, 
                name
            );            
            write_8(code, OP_LOAD_METHOD);
            write_8(code, index);

            write_8(code, OP_CALL);
            write_8(code, root->_method_call.args->_list.size + 1);
            break;
        }

        case AST_FUNCTION: {
            ConstantInformation constant_code = (ConstantInformation){
                    .tag=CONSTANT_CODE,
                    ._code={{0}},
            };

            AstListNode* curr_param = root->_function.params->_list.head;

            while (curr_param) {
                AstNode* ident = ((AstNode*)curr_param->node);
            
                if (ident->type != AST_IDENTIFIER) {
                    return (CompilationStatus){.code=STATUS_FAIL, "[CompileModule]: Error - param is not an identifier"};
                }
                
                size_t param_name_size = strlen(ident->_string) + 1;

                char* param_name = malloc(param_name_size);
                memcpy(param_name, ident->_string, param_name_size);

                // updating global pool name
                update_name_pool(
                    &compiled_module->name_pool,
                    param_name
                );    
                
                // updating param list
                param_name = malloc(param_name_size);
                memcpy(param_name, ident->_string, param_name_size);

                update_name_pool(
                    &constant_code._code.params,
                    param_name
                );
                
                curr_param = curr_param->next;
            }
            
            // Compiling function body
            status = compile_module(root->_function.body, compiled_module, &constant_code._code);
            if (status.code == STATUS_FAIL) return status;
            
            size_t fn_name_size = strlen(root->_function.name) + 1;

            char* fn_name = malloc(fn_name_size);
            memcpy(fn_name, root->_function.name, fn_name_size);

            constant_code._code.name = fn_name;
            
            // Adding code object to constant pool
            size_t index = update_constant_pool(&compiled_module->constant_pool, constant_code);
            
            write_8(code, OP_LOAD_CONST);
            write_8(code, index);

            fn_name = malloc(fn_name_size);
            memcpy(fn_name, root->_function.name, fn_name_size);

            index = update_name_pool(
                &compiled_module->name_pool, 
                fn_name
            );

            write_8(code, OP_STORE_NAME);
            write_8(code, index);
            break;
        }

        case AST_TUPLE: {
            AstList list = root->_list;
            bool is_constant = true;

            AstListNode* curr = list.head;
            if (!curr) return (CompilationStatus){.code=STATUS_FAIL,.what="head of tuple is null"};
            while (curr) {
                if (curr->node && (curr->node->type == AST_IDENTIFIER || curr->node->type == AST_LAMBDA || curr->node->type == AST_FUNCTION)) {
                    is_constant = false;
                    break;
                }
                curr = curr->next;
            }
            curr = list.head;

            if (is_constant) {
                size_t index = update_constant_pool(
                    &compiled_module->constant_pool, 
                    (ConstantInformation){
                        .tag=CONSTANT_TUPLE,
                        ._tuple={0},
                    }
                );
                
                while (curr) {
                    switch (curr->node->type) {
                        case AST_INT: 
                            update_constant_pool(
                                &compiled_module->constant_pool.data[index]._tuple,
                                (ConstantInformation){
                                    .tag=CONSTANT_INTEGER,
                                    ._int=curr->node->_int
                                }
                            );
                            break;
                        case AST_FLOAT: 
                            update_constant_pool(
                                &compiled_module->constant_pool.data[index]._tuple,
                                (ConstantInformation){
                                    .tag=CONSTANT_FLOAT,
                                    ._float=curr->node->_float
                                }
                            );
                            break;
                        case AST_STRING: 
                            size_t string_size = strlen(curr->node->_string) + 1;

                            char* str = malloc(string_size);
                            memcpy(str, curr->node->_string, string_size);

                            update_constant_pool(
                                &compiled_module->constant_pool.data[index]._tuple, 
                                (ConstantInformation){
                                    .tag=CONSTANT_ASCII,
                                    ._string=str
                                }
                            );
                            break;

                        default: 
                            UNIMPLEMENTED 
                            return (CompilationStatus){.code=STATUS_FAIL,.what="unimplemented??"};
                    }
                    curr = curr->next;
                }

                write_8(code, OP_LOAD_CONST);
                write_8(code, index);
                break;
            }

            curr = list.tail;
            while (curr) {
                status = compile_module(curr->node, compiled_module, code);
                if (status.code == STATUS_FAIL) return status; 
                curr = curr->prev;
            }
            write_8(code, OP_BUILD_TUPLE);
            write_8(code, list.size);
            break;
        }

        default:
            fprintf(stderr, "Visit for AST from type '%s' is not implemented yet\n", ast_type_to_str(root->type));
            status = (CompilationStatus){.code=STATUS_FAIL, .what="provided in stderr"};
            break;
    }

    return status;
}

static void free_name_pool(NamePool name_pool) {
    if (!name_pool.data) return;
    for (size_t i = 0; i < name_pool.size; ++i) {
        free(name_pool.data[i]);
    }
    free(name_pool.data);
}

void free_code(Code code) {
    if (code.name) {
        free(code.name);
    }
    free_name_pool(code.params);
    if (code.code)
        free(code.code);

}
static void free_constant_pool(ConstPool constant_pool);
static void free_constant(ConstantInformation constant_info) {
    switch (constant_info.tag) {
        case CONSTANT_NULL:
        case CONSTANT_BOOL:
        case CONSTANT_INTEGER:
        case CONSTANT_FLOAT: break;

        case CONSTANT_ASCII: free(constant_info._string); break;
        case CONSTANT_CODE: free_code(constant_info._code); break;
        case CONSTANT_TUPLE: free_constant_pool(constant_info._tuple); break;
    }
}

void free_constant_pool(ConstPool constant_pool) {
    if (!constant_pool.data) return;

    for (size_t i = 0; i < constant_pool.size; ++i) {
        free_constant(constant_pool.data[i]);
    }

    free(constant_pool.data);
}

void free_module(CompiledModule* compile_module) {
    free_name_pool(compile_module->name_pool);
    free_constant_pool(compile_module->constant_pool);
    free_code(compile_module->entry);

    free(compile_module);
}

// void save_module_to_file(CompiledModule* compiled_module, char* filename) {
//     FILE* fp = fopen(filename, "wb");
//     if (!fp) {
//         fputs("Error - could not open the file to save the module to", stderr);
//         return;
//     }
//     size_t module_filename_size = strlen(compiled_module->filename) + 1;

//     if (fwrite(compiled_module->filename, module_filename_size, 1, fp) != 1) {
//         fputs("Error while writing", stderr);
//     }
//     fwrite(&compiled_module->major_version, sizeof(uint16_t), 1, fp);
//     fwrite(&compiled_module->minor_version, sizeof(uint16_t), 1, fp);

//     fwrite(&compiled_module->name_pool.size, sizeof(size_t), 1, fp);
//     for (size_t i = 0; i < compiled_module->name_pool.size; ++i) {
//         fwrite(compiled_module->name_pool.data[i], strlen(compiled_module->name_pool.data[i]) + 1, 1, fp);
//     }

//     fwrite(&compiled_module->constant_pool.size, sizeof(size_t), 1, fp);
//     for (size_t i = 0; i < compiled_module->constant_pool.size; ++i) {
//         ConstantInformation ci = compiled_module->constant_pool.data[i];
//         fwrite(&ci.tag, sizeof(ci.tag), 1, fp);
//         switch (ci.tag) {
//             case CONSTANT_NULL: break;
//             case CONSTANT_BOOL: fwrite(&ci._bool, sizeof(ci._bool), 1, fp); break;
//             case CONSTANT_INTEGER: fwrite(&ci._int, sizeof(ci._int), 1, fp); break;
//             case CONSTANT_FLOAT: fwrite(&ci._float, sizeof(ci._float), 1, fp); break;
//             case CONSTANT_ASCII: fwrite(ci._string, strlen(ci._string) + 1, 1, fp); break;
//             case CONSTANT_CODE: {
//                 // name
//                 fwrite(ci._code.name, strlen(ci._code.name) + 1, 1, fp);
//                 // params (size, strings)
//                 fwrite(&ci._code.params.size, sizeof(ci._code.params.size), 1, fp);
//                 for (size_t j = 0; j < ci._code.params.size; ++j) {
//                     fwrite(ci._code.params.items[j], strlen(ci._code.params.items[j]) + 1, 1, fp);
//                 }
//                 // code (size, uint8 array)
//                 fwrite(&ci._code.size, sizeof(ci._code.size), 1, fp);
//                 fwrite(ci._code.code, ci._code.size, 1, fp);
//                 break;
//             }
//         }
//     }
//     fwrite(&compiled_module->entry.size, sizeof(size_t), 1, fp);
//     fwrite(compiled_module->entry.code, compiled_module->entry.size, 1, fp);
//     fclose(fp);
// }

// CompiledModule* load_module_from_file(char* filename) {
//     FILE* fp = fopen(filename, "rb");
//     if (!fp) {
//         puts("Can't open file.");
//         return NULL;
//     }
    
//     char* file_name = NULL;
//     read_until_null(fp, &file_name);
    
//     unsigned short major_version, minor_version;
//     fread(&major_version, sizeof(uint16_t), 1, fp);
//     fread(&minor_version, sizeof(uint16_t), 1, fp);
//     NamePool name_pool;
//     fread(&name_pool.size, sizeof(size_t), 1, fp);
//     name_pool.data = calloc(name_pool.size, sizeof(char*));
    
//     for (size_t i = 0; i < name_pool.size; ++i) {
//         read_until_null(fp, name_pool.data + i);
//     }
    
//     ConstPool constant_pool;
//     fread(&constant_pool.size, sizeof(size_t), 1, fp);
//     constant_pool.data = malloc(constant_pool.size * sizeof(ConstantInformation));
//     for (size_t i = 0; i < constant_pool.size; ++i) {
//         constant_pool.data[i] = read_constant(fp);
//     }
    
//     CompiledModule* compiled_module = new_compiled_module(file_name, major_version, minor_version);
//     compiled_module->name_pool = name_pool;
//     compiled_module->constant_pool = constant_pool;
    
//     compiled_module->entry.name = malloc(sizeof "__main__");
//     strcpy(compiled_module->entry.name, "__main__");
//     fread(&compiled_module->entry.size, sizeof(size_t), 1, fp);
//     compiled_module->entry.code = malloc(compiled_module->entry.size);
//     fread(compiled_module->entry.code, compiled_module->entry.size, 1, fp);
    
//     fclose(fp);
//     return compiled_module;
// }


Status name_pool_get(NamePool* name_pool, size_t index, char** out) {
    if (index >= name_pool->size) {
        RETURN_STATUS_FAIL("Index larger than name_pool size")
    }
    *out = name_pool->data[index];
    RETURN_STATUS_OK
}

Status constant_pool_get(ConstPool* constant_pool, size_t index, ConstantInformation* out) {
    if (index >= constant_pool->size) {
        RETURN_STATUS_FAIL("Index larger than constant_pool size")
    }
    *out = constant_pool->data[index];
    RETURN_STATUS_OK
}