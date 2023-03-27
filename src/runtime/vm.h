// #pragma once

// #include "../compiler/compiler.h"
// #include "../compiler/operations.h"
// #include "../misc/stack.h"
// #include "env.h"
// #include "object.h"

// typedef struct CallFrame {
//     struct CallFrame* parent;
//     Environment env;
//     stack_t stack;
//     size_t ip;
// } CallFrame;

// CallFrame init_call_frame(CallFrame* parent_call_frame);
// CallFrame* new_call_frame(CallFrame* parent_call_frame);
// void free_call_frame(CallFrame* call_frame);

// typedef struct VirtualMachine {
//     CompiledModule* compiled_module;
// } VirtualMachine;

// VirtualMachine init_virtual_machine(CompiledModule* compiled_module);
// VirtualMachine* new_virtual_machine(CompiledModule* compiled_module);
// void free_virtual_machine(VirtualMachine* virtual_machine);

// Object* run_code_objct(Code* code, CallFrame* parent_call_frame, ConstPool* constant_pool, NamePool* name_pool);
// void run_virtual_machine(VirtualMachine* virtual_machine);