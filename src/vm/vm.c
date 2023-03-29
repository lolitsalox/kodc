#include "vm.h"

#include "../compiler/operations.h"
#include <stdio.h>
#include <stdlib.h>

#include "native/null.h"
#include "native/bool.h"
#include "native/int.h"
#include "native/float.h"
#include "native/string.h"
#include "native/code.h"

static void unary_op(VirtualMachine* vm, CallFrame* frame, Code* code, String unary_name);
static void binary_op(VirtualMachine* vm, CallFrame* frame, Code* code, String binary_name);

CallFrame init_call_frame(CallFrame* parent, Environment* globals) {
    CallFrame frame = {.ip=0, .parent=parent};
    init_environment(&frame.env);
    if (globals)
        frame.env = *globals;
    init_stack_object(&frame.stack);
    return frame;
}

void free_call_frame(CallFrame* frame, ConstObjectPool* cop) {
    debug_print("FREEING CALL FRAME%s\n","");
    free_object_stack(&frame->stack, cop);
    free_environment(&frame->env);
}

static void update_constant_object_pool(ConstObjectPool* cop, ConstPool* constant_pool) {
    if (!cop || !constant_pool) return;
    cop->size = constant_pool->size;
    cop->data = malloc(constant_pool->size * sizeof(Kod_Object));

    for (size_t i = 0; i < cop->size; ++i) {
        ConstantInformation ci = constant_pool->data[i];
        Kod_Object* obj = &cop->data[i];
        obj->ref_count = 1;
        switch (ci.tag) {
            case CONSTANT_NULL: 
                obj->type = OBJECT_NULL;
                obj->attributes = *get_null_attributes();
                break;
            case CONSTANT_BOOL: 
                obj->type = OBJECT_BOOL;
                obj->attributes = *get_bool_attributes();
                obj->_bool = ci._bool;
                break;
            case CONSTANT_INTEGER: 
                obj->type = OBJECT_INTEGER;
                obj->attributes = *get_int_attributes();
                obj->_int = ci._int;
                break;
            case CONSTANT_FLOAT: 
                obj->type = OBJECT_FLOAT;
                obj->attributes = *get_float_attributes();
                obj->_float = ci._float;
                break;
            case CONSTANT_ASCII: 
                obj->type = OBJECT_STRING;
                obj->attributes = *get_string_attributes();
                obj->_string = ci._string;
                break;
            case CONSTANT_CODE: 
                obj->type = OBJECT_CODE;
                obj->attributes = *get_code_attributes();
                obj->_code = ci._code;
                break;
            default:
                break;
        }
        
        // ref_environment(&obj->attributes);
    }
}

static bool initialized = false;

VirtualMachine init_vm(CompiledModule* module, bool repl) {
    if (!initialized) {
        init_native_attributes();
        init_native_functions();
        initialized = true;
    }
    VirtualMachine vm = (VirtualMachine){.module=module, .repl=repl};
    if (module)
        update_constant_object_pool(&vm.cop, &module->constant_pool);
    return vm;
}
extern int DEBUG;
void destroy_vm(VirtualMachine* vm) {
    // DEBUG = 1;
    if (DEBUG) puts("\nDESTROYING VM");
    // free_native();
    initialized = false;
    free(vm->cop.data);
    free_native_attributes();
    free_module(vm->module);
}

extern Kod_Object* native_print(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size);
Kod_Object* run_code_object(VirtualMachine* vm, Code* code, CallFrame* parent_call_frame, Environment* initial_env, CallFrame* saved_frame) {
    debug_print("RUNNING CODE OBJECT\n%s", "");
    Kod_Object* return_value = NULL;
    
    CallFrame frame;
    if (!saved_frame)
        frame = init_call_frame(parent_call_frame, initial_env);
    else frame = *saved_frame;

    while (frame.ip < code->size) {
        enum Operation op = code->code[frame.ip++];
        switch (op) {
            case OP_POP_TOP: {
                Kod_Object* object = object_stack_pop(&frame.stack);
                if (object && object->type != OBJECT_NULL && vm->repl) {
                    Kod_Object* args[] = {object};
                    native_print(vm, &frame, args, 1);
                }
                deref_object(object);
                break;
            }

            case OP_LOAD_CONST: {
                size_t index = *(size_t*)(code->code + frame.ip);
                frame.ip += sizeof(size_t);
                if (index >= vm->cop.size) break;
                Kod_Object* obj = vm->cop.data + index;
                ref_object(obj);
                if (obj->type == OBJECT_CODE) {
                    obj->_code.parent_closure = &frame.env;
                    ref_environment(obj->_code.parent_closure);
                }
                object_stack_push(&frame.stack, obj);
                break;
            }

            case OP_STORE_NAME: {
                size_t index = *(size_t*)(code->code + frame.ip);
                frame.ip += sizeof(size_t);
                char* name = vm->module->name_pool.data[index];
                Kod_Object* object = object_stack_pop(&frame.stack);
                debug_print("STORE_NAME%s\n", "");
                set_environment(&frame.env, (ObjectNamePair){name, object});
                break;
            }

            case OP_LOAD_NAME: {
                size_t index = *(size_t*)(code->code + frame.ip);
                frame.ip += sizeof(size_t);
                char* name = vm->module->name_pool.data[index];
                CallFrame* curr_frame = &frame;
                Kod_Object* obj = NULL;
                while (!(obj = get_environment(&curr_frame->env, name))) {
                    curr_frame = curr_frame->parent;
                    if (!curr_frame) {
                        printf("NameError: %s is not defined\n", name);
                        frame.ip = code->size; break;
                    } 
                }
                if (frame.ip == code->size) break;
                ++obj->ref_count;
                object_stack_push(&frame.stack, obj);
                break;
            }

            case OP_CALL: {
                size_t arg_count = *(size_t*)(code->code + frame.ip);
                frame.ip += sizeof(size_t);
                Kod_Object* fn_object = object_stack_pop(&frame.stack);
                switch (fn_object->type) {
                    case OBJECT_NATIVE_FUNCTION:
                        Kod_Object** args = malloc(sizeof(Kod_Object*) * arg_count);
                        for (size_t i = 0; i < arg_count; ++i) {
                            args[i] = object_stack_pop(&frame.stack);
                            // ref_object(args[i]);
                            // printf("ref count = %d\n", args[i]->ref_count);
                        }
                        Kod_Object* res = fn_object->_function.callable(vm, &frame, args, arg_count);
                        object_stack_push(&frame.stack, res);
                        for (size_t i = 0; i < arg_count; ++i) {
                            deref_object(args[i]);
                        }
                        free(args);
                        break;

                    case OBJECT_CODE:
                        Environment new_env;
                        init_environment(&new_env);
                        update_environment(&new_env, fn_object->_code.parent_closure);
                        for (size_t i = 0; i < fn_object->_code.params.size; ++i) {
                            Kod_Object* arg = object_stack_pop(&frame.stack);
                            set_environment(&new_env, (ObjectNamePair){.name=fn_object->_code.params.items[i], .object=arg});
                        }
                        object_stack_push(&frame.stack, run_code_object(vm, &fn_object->_code, &frame, &new_env, NULL));

                        break;

                    default:
                        fputs("fn_object is not callable??", stderr);
                        frame.ip = code->size;
                        break;
                }
                deref_object(fn_object);
                break;
            }

            case OP_RETURN: {
                return_value = object_stack_pop(&frame.stack);
                frame.ip = code->size;
                break;
            }

            case OP_JUMP: {
                size_t addr = *(size_t*)(code->code + frame.ip);
                frame.ip = addr;
                break;
            }

            case OP_POP_JUMP_IF_FALSE: {
                size_t addr = *(size_t*)(code->code + frame.ip);
                frame.ip += sizeof(size_t);
                Kod_Object* object = object_stack_pop(&frame.stack);

                switch (object->type) {
                    case OBJECT_NULL: {
                        // todo check primitives

                        break;
                    }
                    default: break;
                }

                Kod_Object* fn_object = get_environment(&object->attributes, "__bool__");
                if (!fn_object) {
                    fprintf(stderr, "__bool__ was not found for object of type %s\n", object_type_to_str(object->type));
                    frame.ip = code->size;
                    break;
                }
                Kod_Object* res = NULL;
                switch (fn_object->type) {
                    case OBJECT_NATIVE_FUNCTION:
                        Kod_Object* args[] = {object};
                        res = fn_object->_function.callable(vm, &frame, args, 1);
                        if (!res) {
                            puts("RuntimeError: res is null");
                            frame.ip = code->size;
                            break;
                        }
                        // printf("res type = %s\n", object_type_to_str(res->type));
                        if (res->type != OBJECT_BOOL) {
                            puts("RuntimeError: __bool__ did not return a bool value");
                            frame.ip = code->size;
                        }
                        else if (!res->_bool)
                            frame.ip = addr;

                        deref_object(object);
                        deref_object(res);
                        break;

                    case OBJECT_CODE:
                        if (fn_object->_code.params.size < 1) {
                            fprintf(stderr, "__bool__ needs to get the self\n");
                            frame.ip = code->size;
                            break;
                        }
                        char* name = fn_object->_code.params.items[0];
                        Environment new_env;
                        init_environment(&new_env);
                        set_environment(&new_env, (ObjectNamePair){.name=name, .object=object});
                        res = run_code_object(vm, &fn_object->_code, &frame, &new_env, NULL);

                        if (res->type != OBJECT_BOOL) {
                            puts("RuntimeError: __bool__ did not return a bool value");
                            frame.ip = code->size;
                        }
                        else if (!res->_bool)
                            frame.ip = addr;

                        deref_object(res);
                        break;

                    default:
                        fputs("__bool__ is not callable??", stderr);
                        break;
                }
                break;
            }

            case OP_UNARY_ADD: unary_op(vm, &frame, code, "__unary_add__"); break;
            case OP_UNARY_SUB: unary_op(vm, &frame, code, "__unary_sub__"); break;
            case OP_UNARY_NOT: unary_op(vm, &frame, code, "__unary_not__"); break;
            case OP_UNARY_BOOL_NOT: unary_op(vm, &frame, code, "__unary_bool_not__"); break;

            case OP_BINARY_ADD: binary_op(vm, &frame, code, "__add__"); break;
            case OP_BINARY_SUB: binary_op(vm, &frame, code, "__sub__"); break;
            case OP_BINARY_MUL: binary_op(vm, &frame, code, "__mul__"); break;
            case OP_BINARY_DIV: binary_op(vm, &frame, code, "__div__"); break;
            case OP_BINARY_MOD: binary_op(vm, &frame, code, "__mod__"); break;
            case OP_BINARY_POW: binary_op(vm, &frame, code, "__pow__"); break;
            case OP_BINARY_AND: binary_op(vm, &frame, code, "__and__"); break;
            case OP_BINARY_OR: binary_op(vm, &frame, code, "__or__"); break;
            case OP_BINARY_XOR: binary_op(vm, &frame, code, "__xor__"); break;
            case OP_BINARY_LEFT_SHIFT: binary_op(vm, &frame, code, "__shl__"); break;
            case OP_BINARY_RIGHT_SHIFT: binary_op(vm, &frame, code, "__shr__"); break;
            case OP_BINARY_BOOLEAN_AND: binary_op(vm, &frame, code, "__bool_and__"); break;
            case OP_BINARY_BOOLEAN_OR: binary_op(vm, &frame, code, "__bool_or__"); break;
            case OP_BINARY_BOOLEAN_EQUAL: binary_op(vm, &frame, code, "__eq__"); break;
            case OP_BINARY_BOOLEAN_NOT_EQUAL: binary_op(vm, &frame, code, "__ne__"); break;
            case OP_BINARY_BOOLEAN_GREATER_THAN: binary_op(vm, &frame, code, "__gt__"); break;
            case OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO: binary_op(vm, &frame, code, "__ge__"); break;
            case OP_BINARY_BOOLEAN_LESS_THAN: binary_op(vm, &frame, code, "__lt__"); break;
            case OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO: binary_op(vm, &frame, code, "__le__"); break;

            default: printf("dont know how to do %s\n", op_to_str(op)); frame.ip = code->size; break;
        }
    }
    // print_environment(&frame.env);
    if (!return_value) {
        bool found = false;
        for (size_t i = 0; !found && (i < vm->cop.size); ++i) {
            if (vm->cop.data[i].type == OBJECT_NULL) {
                ref_object(&vm->cop.data[i]);
                return_value = &vm->cop.data[i];
                found = true;
            }
        }
        if (!found)
            return_value = new_null_object();
    } 

    if (!saved_frame)
        free_call_frame(&frame, &vm->cop);
    else *saved_frame = frame;
    return return_value;
}

Kod_Object* vm_run_entry(VirtualMachine* vm) {
    Environment* native_functions = get_native_functions();
    ref_environment(native_functions);
    Kod_Object* result = run_code_object(vm, &vm->module->entry, NULL, native_functions, NULL);
    return result;
}

static void unary_op(VirtualMachine* vm, CallFrame* frame, Code* code, String unary_name) {
    Kod_Object* object = object_stack_pop(&frame->stack);
    Kod_Object* fn_object = get_environment(&object->attributes, unary_name);
    if (!fn_object) {
        fprintf(stderr, "%s was not found for object of type %s\n", unary_name, object_type_to_str(object->type));
        frame->ip = code->size;
        return;
    }
    switch (fn_object->type) {
        case OBJECT_NATIVE_FUNCTION:
            Kod_Object* args[] = {object};
            object_stack_push(&frame->stack, fn_object->_function.callable(vm, frame, args, 1));
            break;

        case OBJECT_CODE:
            if (fn_object->_code.params.size < 1) {
                fprintf(stderr, "%s needs to get the self\n", unary_name);
                frame->ip = code->size;
                break;
            }
            char* name = fn_object->_code.params.items[0];
            Environment new_env;
            init_environment(&new_env);
            set_environment(&new_env, (ObjectNamePair){.name=name, .object=object});
            object_stack_push(&frame->stack, run_code_object(vm, &fn_object->_code, frame, &new_env, NULL));
            break;

        default:
            fprintf(stderr, "%s is not callable??", unary_name);
            break;
    }
}

static void binary_op(VirtualMachine* vm, CallFrame* frame, Code* code, String binary_name) {
    Kod_Object* right = object_stack_pop(&frame->stack);
    Kod_Object* left = object_stack_pop(&frame->stack);
    Kod_Object* fn_object = get_environment(&left->attributes, binary_name);
    if (!fn_object) {
        fprintf(stderr, "%s was not found for object of type %s\n", binary_name, object_type_to_str(left->type));
        frame->ip = code->size;
        return;
    }
    switch (fn_object->type) {
        case OBJECT_NATIVE_FUNCTION:
            Kod_Object* args[] = {left, right};
            object_stack_push(&frame->stack, fn_object->_function.callable(vm, frame, args, 2));
            break;

        case OBJECT_CODE:
            if (fn_object->_code.params.size < 2) {
                fprintf(stderr, "%s needs to get the self and the other\n", binary_name);
                frame->ip = code->size;
                break;
            }
            Environment new_env;
            init_environment(&new_env);
            for (size_t i = 0; i < fn_object->_code.params.size; ++i) {
                Kod_Object* arg = object_stack_pop(&frame->stack);
                set_environment(&new_env, (ObjectNamePair){.name=fn_object->_code.params.items[i], .object=arg});
            }
            object_stack_push(&frame->stack, run_code_object(vm, &fn_object->_code, frame, &new_env, NULL));
            break;

        default:
            fprintf(stderr, "%s is not callable??", binary_name);
            break;
    }
    deref_object(left);
    deref_object(right);
}