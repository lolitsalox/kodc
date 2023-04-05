#include "vm.h"
#include "objects/dict.h"
#include "objects/tuple.h"
#include "objects/string.h"

#include "builtins.h"

CallFrame init_call_frame(CallFrame* parent, KodDictObject* globals) {
    return (CallFrame){0};
}

void free_call_frame(CallFrame* frame, KodDictObject* constant_objects) {
    return;
}


VirtualMachine init_vm(CompiledModule* module, bool repl) {
    VirtualMachine vm;
    vm.constant_objects = (KodDictObject*)KodType_Dict.new(&KodType_Dict, &Kod_Null, &Kod_Null);

    for (size_t i = 0; i < sizeof(native_functions) / sizeof(NativeFunctionPair); ++i) {
        printf("native function %s at 0x%p\n", native_functions[i].name, native_functions[i].func);
        KodObject* items[] = {As_Object(string_new_from_string(native_functions[i].name)), As_Object(null_new())};
        KodTupleObject tuple = {
            .object={
                .ref_count=1,
                .type=OBJECT_TUPLE,
                .type_object=&KodType_Tuple
            },
            ._tuple={
                .items=items, //new_native_function_object(native_functions[i].func
                .size=2
            }
        };

        KodType_Dict.methods[DICT_METHOD_SET].method(As_Object(vm.constant_objects), As_Object(&tuple), As_Object(&Kod_Null));
    }
    return vm;
}

void destroy_vm(VirtualMachine* vm) {
    return;
}


KodObject* vm_run_entry(VirtualMachine* vm) {
    UNIMPLEMENTED;
}

KodObject* run_code_object(VirtualMachine* vm, Code* code, CallFrame* parent_call_frame, KodDictObject* initial_env, CallFrame* saved_frame) {
    UNIMPLEMENTED;
}
