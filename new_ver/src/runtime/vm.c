#include "vm.h"

#include "objects/kod_object_type.h"
#include "objects/kod_object_int.h"
#include "objects/kod_object_float.h"
#include "objects/kod_object_func.h"
#include "objects/kod_object_native_func.h"
#include "objects/kod_object_tuple.h"
#include "objects/kod_object_null.h"
#include "objects/kod_object_bool.h"
#include "objects/kod_object_string.h"

#include "builtins.h"

#include "../operations.h"

#define BIN_OP_CASE(op) { \
    KodObject* right = NULL; \
    if ((s = object_stack_pop(&vm->stack, &right)).type == ST_FAIL) return s; \
    \
    KodObject* left = NULL; \
    if ((s = object_stack_pop(&vm->stack, &left)).type == ST_FAIL) return s; \
    \
    if (!left->type->as_number) {\
    if ((s = kod_object_deref(right)).type == ST_FAIL) return s;\
    if ((s = kod_object_deref(left)).type == ST_FAIL) return s;\
    RETURN_STATUS_FAIL("Type has no number representation"); \
    }\
    if (!left->type->as_number->op) {\
        if ((s = kod_object_deref(right)).type == ST_FAIL) return s; \
        if ((s = kod_object_deref(left)).type == ST_FAIL) return s; \
        RETURN_STATUS_FAIL("Type has no attribute "#op); \
    }\
    KodObject* obj; \
    if ((s = left->type->as_number->op(left, right, &obj)).type == ST_FAIL) {\
        Status s_;\
        if ((s_ = kod_object_deref(right)).type == ST_FAIL) return s_;\
        if ((s_ = kod_object_deref(left)).type == ST_FAIL) return s_;\
        return s; \
    }\
    if ((s = kod_object_ref(obj)).type == ST_FAIL) return s;\
    if ((s = object_stack_push(&vm->stack, AS_OBJECT(obj))).type == ST_FAIL) return s; \
    if ((s = kod_object_deref(right)).type == ST_FAIL) return s;\
    if ((s = kod_object_deref(left)).type == ST_FAIL) return s;\
break; \
    }

static Status read_data(Code* code, size_t* offset, size_t size, void* out) {
    if (!code) RETURN_STATUS_FAIL("code is null")
    if (!out) RETURN_STATUS_FAIL("out is null")

    if (code->size < *offset + size) RETURN_STATUS_FAIL("Not enough data in code object")

    memcpy(out, code->code + *offset, size);
    *offset += size;

    RETURN_STATUS_OK;
}

static Status read_8(Code* code, size_t* offset, u8* out) {
    return read_data(code, offset, sizeof(u8), out);
}

static Status vm_find_object(VirtualMachine* vm, char* name, KodObject** obj) {
    if (!vm) RETURN_STATUS_FAIL("vm is null")
    if (!name) RETURN_STATUS_FAIL("name is null")
    if (!obj) RETURN_STATUS_FAIL("obj is null")

    Status s;

    CallFrame frame = { 0 };
    for (i64 i = vm->frame_stack.size - 1; i >= 0; --i) {
        frame = vm->frame_stack.frames[i];
        if ((s = object_map_find(&frame.locals, name, obj)).type == ST_FAIL) return s;
        if (*obj) RETURN_STATUS_OK;
    }

    if ((s = object_map_find(&vm->globals, name, obj)).type == ST_FAIL) return s;
    if (*obj) RETURN_STATUS_OK;

    RETURN_STATUS_FAIL("Object not found")
}

Status load_constant_objects(ConstPool constant_pool, KodObject** data) {
    Status s;

    for (size_t i = 0; i < constant_pool.size; ++i) {
        if (!data) {
            RETURN_STATUS_FAIL("constant_objects is null")
        }
        if (data[i]) continue;

        ConstantInformation ci = constant_pool.data[i];
        KodObject* obj = NULL;

        switch (ci.tag) {
            case CONSTANT_NULL: {
                if ((s = kod_object_new_null((KodObjectNull**) &obj)).type == ST_FAIL) return s;
                break;
            }

            case CONSTANT_BOOL: {
                if ((s = kod_object_new_bool(ci._bool, (KodObjectBool**) &obj)).type == ST_FAIL) return s;
                break;
            }
            
            case CONSTANT_INTEGER: {
                if ((s = kod_object_new_int(ci._int, (KodObjectInt**) &obj)).type == ST_FAIL) return s;
                break;
            }

            case CONSTANT_FLOAT: {
                if ((s = kod_object_new_float(ci._float, (KodObjectFloat**) &obj)).type == ST_FAIL) return s;
                break;
            }

            case CONSTANT_CODE: {
                if ((s = kod_object_new_func(ci._code, (KodObjectFunc**)&obj)).type == ST_FAIL) return s;
                break;
            }

            case CONSTANT_ASCII: {
                if ((s = kod_object_new_string(ci._string, (KodObjectString**)&obj)).type == ST_FAIL) return s;
                break;
            }
            
            case CONSTANT_TUPLE: {
                if ((s = kod_object_new_tuple(ci._tuple.size, (KodObjectTuple**) &obj)).type == ST_FAIL) return s;
                if ((s = load_constant_objects(ci._tuple, ((KodObjectTuple*)obj)->data)).type == ST_FAIL) return s;
                break;
            }

            default:
                ERROR_ARGS("Runtime", "Can't load constant %s\n", constant_tag_to_str(ci.tag)); 
                RETURN_STATUS_FAIL("Unknown constant");
        }

        if ((s = kod_object_ref(obj)).type == ST_FAIL) return s;
        data[i] = obj;

    }
    RETURN_STATUS_OK
}

Status vm_init(CompiledModule* module, bool repl, VirtualMachine* out) {
    if (!module) {
        RETURN_STATUS_FAIL("Module is null");
    }
    if (!out) {
        RETURN_STATUS_FAIL("Vm is null");
    }

    Status s;
    out->module = module;

    if (!out->initialized) {
        out->repl = repl;

        out->constant_objects = calloc(module->constant_pool.size, sizeof(KodObject*));
        if (!out->constant_objects) RETURN_STATUS_FAIL("Coudln't allocate for constant objects")
        out->constant_objects_size = module->constant_pool.size;

        if ((s = frame_stack_init(&out->frame_stack)).type == ST_FAIL) return s;
        if ((s = object_stack_init(&out->stack)).type == ST_FAIL) return s;
        if ((s = object_map_init(&out->globals)).type == ST_FAIL) return s;

        if ((s = kod_object_ref(AS_OBJECT(&KodType_String))).type == ST_FAIL) return s;
        if ((s = object_map_insert(&out->globals, "str", AS_OBJECT(&KodType_String))).type == ST_FAIL) return s;

        if ((s = kod_object_ref(AS_OBJECT(&KodType_Int))).type == ST_FAIL) return s;
        if ((s = object_map_insert(&out->globals, "int", AS_OBJECT(&KodType_Int))).type == ST_FAIL) return s;

        if ((s = kod_object_ref(AS_OBJECT(&KodType_Float))).type == ST_FAIL) return s;
        if ((s = object_map_insert(&out->globals, "float", AS_OBJECT(&KodType_Float))).type == ST_FAIL) return s;

        if ((s = kod_object_ref(AS_OBJECT(&KodType_Bool))).type == ST_FAIL) return s;
        if ((s = object_map_insert(&out->globals, "bool", AS_OBJECT(&KodType_Bool))).type == ST_FAIL) return s;

        if ((s = kod_object_ref(AS_OBJECT(&KodType_Tuple))).type == ST_FAIL) return s;
        if ((s = object_map_insert(&out->globals, "tuple", AS_OBJECT(&KodType_Tuple))).type == ST_FAIL) return s;

        if ((s = kod_object_ref(AS_OBJECT(&KodObject_True))).type == ST_FAIL) return s;
        if ((s = object_map_insert(&out->globals, "true", AS_OBJECT(&KodObject_True))).type == ST_FAIL) return s;

        if ((s = kod_object_ref(AS_OBJECT(&KodObject_False))).type == ST_FAIL) return s;
        if ((s = object_map_insert(&out->globals, "false", AS_OBJECT(&KodObject_False))).type == ST_FAIL) return s;

        if ((s = kod_object_ref(AS_OBJECT(&KodObject_Null))).type == ST_FAIL) return s;
        if ((s = object_map_insert(&out->globals, "null", AS_OBJECT(&KodObject_Null))).type == ST_FAIL) return s;        

        // if ((s = kod_object_ref(AS_OBJECT(&KodType_Type))).type == ST_FAIL) return s;
        // if ((s = object_map_insert(&out->globals, "type", AS_OBJECT(&KodType_Type))).type == ST_FAIL) return s;

        // if ((s = kod_object_ref(AS_OBJECT(&KodType_Func))).type == ST_FAIL) return s;
        // if ((s = object_map_insert(&out->globals, "func", AS_OBJECT(&KodType_Func))).type == ST_FAIL) return s;

        if ((s = builtins_init(out)).type == ST_FAIL) return s;

        out->initialized = true;
    }

    if (out->constant_objects_size != module->constant_pool.size) {
        void* p = realloc(out->constant_objects, module->constant_pool.size * sizeof(KodObject*));
        if (!p) RETURN_STATUS_FAIL("Couldn't reallocate constant objects")
        out->constant_objects = p;
        memset(out->constant_objects + out->constant_objects_size, 0, (module->constant_pool.size - out->constant_objects_size) * sizeof(KodObject*));
        out->constant_objects_size = module->constant_pool.size;
    }

    return load_constant_objects(module->constant_pool, out->constant_objects);
}

Status vm_destroy(VirtualMachine* vm) {
    Status s;

#ifdef DEBUG_VM
    LOG("destroying frame stack\n");
#endif
    if ((s = frame_stack_clear(&vm->frame_stack)).type == ST_FAIL) return s;

#ifdef DEBUG_VM
    LOG("destroying constant objects\n");
#endif
    for (size_t i = 0; i < vm->module->constant_pool.size; ++i) {
        if ((s = kod_object_deref(vm->constant_objects[i])).type == ST_FAIL) return s;
    }

#ifdef DEBUG_VM
    LOG("destroying globals\n");
#endif
    if ((s = object_map_clear(&vm->globals)).type == ST_FAIL) return s;
    
    // object_map_print(&vm->globals);
    vm->initialized = false;
    vm->module = NULL;
    RETURN_STATUS_OK
}


Status vm_run_code_object(VirtualMachine* vm, Code* code_obj, ObjectMap* initial_env, KodObject** out) {
    if (!vm) RETURN_STATUS_FAIL("VirtualMachine is null");
    if (!out) RETURN_STATUS_FAIL("Output parameter is null");
    Status s;

    if (vm->frame_stack.size == 0) {
        CallFrame frame;
        if ((s = call_frame_init(&frame, &vm->module->entry)).type == ST_FAIL) return s;
        if ((s = frame_stack_push(&vm->frame_stack, frame)).type == ST_FAIL) return s;
    }

    CallFrame* call_frame = NULL;
    if ((s = frame_stack_top(&vm->frame_stack, &call_frame)).type == ST_FAIL) return s;

    // Set initial environment
    if (initial_env) {
        call_frame->locals = *initial_env;
    }

    while (call_frame->ip < call_frame->code->size) {
        enum Operation op = OP_UNKNOWN;
        if ((s = read_8(call_frame->code, &call_frame->ip, (u8*)&op)).type == ST_FAIL) return s;
#ifdef DEBUG_VM
        LOG_ARGS("OP: %s\n", op_to_str(op));
#endif
        switch (op) {
            case OP_POP_TOP: {
                KodObject* obj = NULL;
                if ((s = object_stack_pop(&vm->stack, &obj)).type == ST_FAIL) return s;
                if (obj->kind == OBJECT_NULL) {
                    if ((s = kod_object_deref(obj)).type == ST_FAIL) return s;
                    break;
                }
                if (vm->repl) {
                    if (obj->type->str == 0) {
                        RETURN_STATUS_FAIL("Type has no str attribute")
                    }

                    char* output = NULL;
                    if ((s = obj->type->str(obj, &output)).type == ST_FAIL) return s;
                    puts(output);
                    if (output)
                        free(output);
                }
                if ((s = kod_object_deref(obj)).type == ST_FAIL) return s;
                break;
            }
            
            case OP_LOAD_CONST: {
                // index inside constant pool
                u8 index = 0;
                if ((s = read_8(call_frame->code, &call_frame->ip, &index)).type == ST_FAIL) return s;

                KodObject* obj = vm->constant_objects[index];
                if (!obj) RETURN_STATUS_FAIL("Object found in constant objects was null");

                if ((s = kod_object_ref(obj)).type == ST_FAIL) return s;
                if ((s = object_stack_push(&vm->stack, AS_OBJECT(obj))).type == ST_FAIL) return s;
                break;
            }

            case OP_LOAD_NAME: {
                // index inside name pool
                u8 index = 0;
                if ((s = read_8(call_frame->code, &call_frame->ip, &index)).type == ST_FAIL) return s;

                char* name = NULL;
                if ((s = name_pool_get(&vm->module->name_pool, index, &name)).type == ST_FAIL) return s;

                KodObject* obj = NULL;
                if ((s = vm_find_object(vm, name, &obj)).type == ST_FAIL) return s;

                if ((s = kod_object_ref(obj)).type == ST_FAIL) return s;
                if ((s = object_stack_push(&vm->stack, AS_OBJECT(obj))).type == ST_FAIL) return s;
                break;
            }

            case OP_STORE_NAME: {
                // index inside name pool
                u8 index = 0;
                if ((s = read_8(call_frame->code, &call_frame->ip, &index)).type == ST_FAIL) return s;

                char* name = NULL;
                if ((s = name_pool_get(&vm->module->name_pool, index, &name)).type == ST_FAIL) return s;

                KodObject* obj = NULL;
                if ((s = object_stack_pop(&vm->stack, &obj)).type == ST_FAIL) return s;

                if ((s = object_map_insert(&call_frame->locals, name, obj)).type == ST_FAIL) return s;
                break;
            }

            case OP_BUILD_TUPLE: {
                // size of tuple
                u8 tuple_size = 0;
                if ((s = read_8(call_frame->code, &call_frame->ip, &tuple_size)).type == ST_FAIL) return s;
                
                KodObjectTuple* tuple = NULL;
                if ((s = kod_object_new_tuple((u64)tuple_size, &tuple)).type == ST_FAIL) return s;

                KodObject* obj = NULL;
                for (u8 i = 0; i < tuple_size; ++i) {
                    obj = NULL;
                    if ((s = object_stack_pop(&vm->stack, &obj)).type == ST_FAIL) return s;
                    tuple->data[i] = obj;
                }

                if ((s = kod_object_ref(AS_OBJECT(tuple))).type == ST_FAIL) return s;
                if ((s = object_stack_push(&vm->stack, AS_OBJECT(tuple))).type == ST_FAIL) return s;
                break;
            }

            case OP_CALL: {
                // num of args
                u8 arg_size = 0;
                if ((s = read_8(call_frame->code, &call_frame->ip, &arg_size)).type == ST_FAIL) return s;

                KodObject* fn_obj = NULL;
                if ((s = object_stack_pop(&vm->stack, &fn_obj)).type == ST_FAIL) return s;
                
                if (fn_obj->kind == OBJECT_FUNC) {
                    Code code = ((KodObjectFunc*)fn_obj)->_code;

                    if (code.params.size != arg_size) RETURN_STATUS_FAIL("Argument size does not match function parameter size")

                    CallFrame new_frame;
                    if ((s = call_frame_init(&new_frame, &code)).type == ST_FAIL) return s;
                    
                    KodObject* obj = NULL;
                    for (u8 i = 0; i < code.params.size; ++i) {
                        obj = NULL;
                        if ((s = object_stack_pop(&vm->stack, &obj)).type == ST_FAIL) return s;
                        if ((s = object_map_insert(&new_frame.locals, code.params.data[i], obj)).type == ST_FAIL) return s;
                    }
                    if ((s = frame_stack_push(&vm->frame_stack, new_frame)).type == ST_FAIL) return s;

                    KodObject* result = NULL;
                    if ((s = vm_run_code_object(vm, &code, NULL, &result)).type == ST_FAIL) {
                        Status s_;

                        if ((s_ = kod_object_deref(fn_obj)).type == ST_FAIL) {
                            ERROR("KodRuntime", s_.what);
                            free(s_.what);
                        }
                        if ((s_ = frame_stack_pop(&vm->frame_stack, &call_frame)).type == ST_FAIL) {
                            ERROR("KodRuntime", s_.what);
                            free(s_.what);
                        }
                        if ((s_ = call_frame_clear(call_frame)).type == ST_FAIL) {
                            ERROR("KodRuntime", s_.what);
                            free(s_.what);
                        }
                        if ((s_ = frame_stack_top(&vm->frame_stack, &call_frame)).type == ST_FAIL) {
                            ERROR("KodRuntime", s_.what);
                            free(s_.what);
                        }

                        return s;
                    }

                    if ((s = kod_object_deref(fn_obj)).type == ST_FAIL) return s;
                    if ((s = frame_stack_top(&vm->frame_stack, &call_frame)).type == ST_FAIL) return s;
                    break;
                }

                if (!fn_obj->type->call) RETURN_STATUS_FAIL("Type has no call attribute")
                
                // build a tuple for the args
                KodObjectTuple* args = NULL;
                if ((s = kod_object_new_tuple(arg_size, &args)).type == ST_FAIL) return s;
                
                KodObject* obj = NULL;
                for (u8 i = 0; i < arg_size; ++i) {
                    obj = NULL;
                    if ((s = object_stack_pop(&vm->stack, &obj)).type == ST_FAIL) return s;
                    args->data[i] = obj;
                }

                KodObject* result = NULL;
                if ((s = fn_obj->type->call(vm, fn_obj, AS_OBJECT(args), NULL, &result)).type == ST_FAIL) return s;

                if ((s = kod_object_deref(AS_OBJECT(args))).type == ST_FAIL) return s;

                if ((s = kod_object_ref(result)).type == ST_FAIL) return s;
                if ((s = object_stack_push(&vm->stack, result)).type == ST_FAIL) return s;
                break;
            }

            case OP_RETURN: {
                KodObject* obj = NULL;
                if ((s = object_stack_top(&vm->stack, &obj)).type == ST_FAIL) return s;
                *out = obj;

                if (vm->frame_stack.size > 1) {
                    if ((s = frame_stack_pop(&vm->frame_stack, NULL)).type == ST_FAIL) return s;
                    if ((s = call_frame_clear(call_frame)).type == ST_FAIL) return s;
                }
                RETURN_STATUS_OK
                break;
            }

            case OP_POP_JUMP_IF_FALSE: {
                u8 ip_to_jump = 0;
                if ((s = read_8(call_frame->code, &call_frame->ip, &ip_to_jump)).type == ST_FAIL) return s;

                KodObject* obj = NULL;
                if ((s = object_stack_pop(&vm->stack, &obj)).type == ST_FAIL) return s;

                if (!obj->type->as_number) RETURN_STATUS_FAIL("Type has no number representation");
                if (!obj->type->as_number->_bool) RETURN_STATUS_FAIL("Type has no boolean representation");

                bool is_false = false;
                if ((s = obj->type->as_number->_bool(obj, &is_false)).type == ST_FAIL) return s;
                if (is_false == false) {
                    call_frame->ip = ip_to_jump;
                }
                if ((s = kod_object_deref(obj)).type == ST_FAIL) return s;
                break;
            }

            case OP_JUMP: {
                u8 ip_to_jump = 0;
                if ((s = read_8(call_frame->code, &call_frame->ip, &ip_to_jump)).type == ST_FAIL) return s;
                call_frame->ip = ip_to_jump;
                break;
            }

            case OP_BINARY_ADD: BIN_OP_CASE(add)
            case OP_BINARY_SUB: BIN_OP_CASE(sub)
            case OP_BINARY_MUL: BIN_OP_CASE(mul)
            case OP_BINARY_DIV: BIN_OP_CASE(div)

            case OP_BINARY_BOOLEAN_LESS_THAN: BIN_OP_CASE(lt)

            default: {
                ERROR_ARGS("Runtime", "Unknown opcode: %s\n", op_to_str(op));
                RETURN_STATUS_FAIL("Unknown opcode");
            }
        }
    }

    RETURN_STATUS_OK
}
