#pragma once

#include "../compiler/compiler.h"

#include "env.h"
#include "objectStack.h"

typedef struct CallFrame {
    struct CallFrame* parent;
    Environment env;
    ObjectStack stack;
    size_t ip;
} CallFrame;

CallFrame init_call_frame(CallFrame* parent, Environment* globals);
void free_call_frame(CallFrame* frame);

typedef struct VirtualMachine {
    CompiledModule* module;
} VirtualMachine;

VirtualMachine init_vm(CompiledModule* module);

void vm_run_entry(VirtualMachine* vm);
