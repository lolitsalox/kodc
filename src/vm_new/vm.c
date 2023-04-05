#include "vm.h"


CallFrame init_call_frame(CallFrame* parent, KodDictObject* globals) {
    return (CallFrame){0};
}

void free_call_frame(CallFrame* frame, KodDictObject* constant_objects) {
    return;
}


VirtualMachine init_vm(CompiledModule* module, bool repl) {
    return (VirtualMachine){0};
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
