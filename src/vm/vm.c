#include "vm.h"

#include "../compiler/operations.h"
#include <stdio.h>
#include <stdlib.h>

static void unary_op(VirtualMachine* vm, CallFrame* frame, Code* code, String unary_name);
static void binary_op(VirtualMachine* vm, CallFrame* frame, Code* code, String binary_name);

CallFrame init_call_frame(CallFrame* parent, Environment* globals) {
    CallFrame frame = {.ip=0, .parent=parent};
    init_environment(&frame.env);
    update_environment(&frame.env, globals);
    init_stack_object(&frame.stack);
    return frame;
}

void free_call_frame(CallFrame* frame) {
    // free_object_stack(&frame->stack);
    // free_environment(&frame->env);
}

static bool initialized = false;

VirtualMachine init_vm(CompiledModule* module) {
    if (!initialized) {
        init_native_attributes();
        init_native_functions();
        initialized = true;
    }
    return (VirtualMachine){.module=module};
}

extern Kod_Object* run_code_object(VirtualMachine* vm, Code* code, CallFrame* parent_call_frame, Environment* initial_env) {
    Kod_Object* return_value = NULL;
    CallFrame frame = init_call_frame(parent_call_frame, initial_env);
    while (frame.ip < code->size) {
        enum Operation op = code->code[frame.ip++];
        switch (op) {
            case OP_POP_TOP: {
                Kod_Object* object = object_stack_pop(&frame.stack);
                deref_object(object);
                break;
            }

            case OP_LOAD_CONST: {
                size_t index = *(size_t*)(code->code + frame.ip);
                frame.ip += sizeof(size_t);
                Kod_Object* obj = NULL;
                ConstantInformation constant = vm->module->constant_pool.data[index];
                switch (constant.tag) {
                    case CONSTANT_NULL: obj = new_null_object(); break;
                    case CONSTANT_BOOL: obj = new_bool_object(constant._bool); break;
                    case CONSTANT_INTEGER: obj = new_int_object(constant._int); break;
                    case CONSTANT_ASCII: obj = new_string_object(constant._string); break;
                    case CONSTANT_CODE: obj = new_code_object(constant._code); break;
                    case CONSTANT_FLOAT: puts("no float for now"); break;
                }
                object_stack_push(&frame.stack, obj);
                break;
            }

            case OP_STORE_NAME: {
                size_t index = *(size_t*)(code->code + frame.ip);
                frame.ip += sizeof(size_t);
                char* name = vm->module->name_pool.data[index];
                Kod_Object* object = object_stack_pop(&frame.stack);
                set_environment(&frame.env, (ObjectNamePair){name, object});
                // deref_object(object);
                break;
            }

            case OP_LOAD_NAME: {
                size_t index = *(size_t*)(code->code + frame.ip);
                frame.ip += sizeof(size_t);
                char* name = vm->module->name_pool.data[index];
                CallFrame* curr_frame = &frame;
                Kod_Object* obj = NULL;
                while (!(obj = get_environment(&curr_frame->env, name))) {
                    curr_frame = frame.parent;
                    if (!curr_frame) {
                        printf("NameError: %s is not defined\n", name);
                        frame.ip = code->size; break;
                    } 
                }
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
                        }
                        return_value = fn_object->_function.callable(args, arg_count);
                        object_stack_push(&frame.stack, return_value);
                        free(args);
                        break;

                    case OBJECT_CODE:
                        Environment* new_env = new_environment();
                        for (size_t i = 0; i < fn_object->_code.params.size; ++i) {
                            Kod_Object* arg = object_stack_pop(&frame.stack);
                            set_environment(new_env, (ObjectNamePair){.name=fn_object->_code.params.items[i], .object=arg});
                            // deref_object(arg);
                        }
                        return_value = run_code_object(vm, &fn_object->_code, &frame, new_env);
                        object_stack_push(&frame.stack, return_value);
                        break;

                    default:
                        fputs("fn_object is not callable??", stderr);
                        break;
                }
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
                Kod_Object* fn_object = get_environment(object->attributes, "__bool__");
                if (!fn_object) {
                    fprintf(stderr, "__bool__ was not found for object of type %s\n", object_type_to_str(object->type));
                    frame.ip = code->size;
                    break;
                }
                switch (fn_object->type) {
                    case OBJECT_NATIVE_FUNCTION:
                        Kod_Object* args[] = {object};
                        return_value = fn_object->_function.callable(args, 1);
                        if (return_value->type == OBJECT_INTEGER && !return_value->_int)
                            frame.ip = addr;
                        // deref_object(return_value);
                        break;

                    case OBJECT_CODE:
                        if (fn_object->_code.params.size < 1) {
                            fprintf(stderr, "__bool__ needs to get the self\n");
                            frame.ip = code->size;
                            break;
                        }
                        char* name = fn_object->_code.params.items[0];
                        Environment* new_env = new_environment();
                        set_environment(new_env, (ObjectNamePair){.name=name, .object=object});
                        // deref_object(object);
                        return_value = run_code_object(vm, &fn_object->_code, &frame, new_env);
                        if (return_value->type == OBJECT_INTEGER && !return_value->_int)
                            frame.ip = addr;
                        // deref_object(return_value);
                        // free(new_env);
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
    free_call_frame(&frame);
    if (!return_value)
        return new_null_object();
    return return_value;
}

void vm_run_entry(VirtualMachine* vm) {
    Kod_Object* result = run_code_object(vm, &vm->module->entry, NULL, get_native_functions());
    // deref_object(result);
}

static void unary_op(VirtualMachine* vm, CallFrame* frame, Code* code, String unary_name) {
    Kod_Object* object = object_stack_pop(&frame->stack);
    Kod_Object* fn_object = get_environment(object->attributes, unary_name);
    if (!fn_object) {
        fprintf(stderr, "%s was not found for object of type %s\n", unary_name, object_type_to_str(object->type));
        frame->ip = code->size;
        return;
    }
    switch (fn_object->type) {
        case OBJECT_NATIVE_FUNCTION:
            Kod_Object* args[] = {object};
            object_stack_push(&frame->stack, fn_object->_function.callable(args, 1));
            break;

        case OBJECT_CODE:
            if (fn_object->_code.params.size < 1) {
                fprintf(stderr, "%s needs to get the self\n", unary_name);
                frame->ip = code->size;
                break;
            }
            char* name = fn_object->_code.params.items[0];
            Environment* new_env = new_environment();
            set_environment(new_env, (ObjectNamePair){.name=name, .object=object});
            // deref_object(object);
            object_stack_push(&frame->stack, run_code_object(vm, &fn_object->_code, frame, new_env));
            // free(new_env);
            break;

        default:
            fprintf(stderr, "%s is not callable??", unary_name);
            break;
    }
}

static void binary_op(VirtualMachine* vm, CallFrame* frame, Code* code, String binary_name) {
    Kod_Object* right = object_stack_pop(&frame->stack);
    Kod_Object* left = object_stack_pop(&frame->stack);
    Kod_Object* fn_object = get_environment(left->attributes, binary_name);
    if (!fn_object) {
        fprintf(stderr, "%s was not found for object of type %s\n", binary_name, object_type_to_str(left->type));
        frame->ip = code->size;
        return;
    }
    switch (fn_object->type) {
        case OBJECT_NATIVE_FUNCTION:
            Kod_Object* args[] = {left, right};
            object_stack_push(&frame->stack, fn_object->_function.callable(args, 2));
            break;

        case OBJECT_CODE:
            if (fn_object->_code.params.size < 2) {
                fprintf(stderr, "%s needs to get the self and the other\n", binary_name);
                frame->ip = code->size;
                break;
            }
            Environment* new_env = new_environment();
            for (size_t i = 0; i < fn_object->_code.params.size; ++i) {
                Kod_Object* arg = object_stack_pop(&frame->stack);
                set_environment(new_env, (ObjectNamePair){.name=fn_object->_code.params.items[i], .object=arg});
                // deref_object(arg);
            }
            object_stack_push(&frame->stack, run_code_object(vm, &fn_object->_code, frame, new_env));
            // free(new_env);
            break;

        default:
            fprintf(stderr, "%s is not callable??", binary_name);
            break;
    }
}