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

typedef struct ConstObjectPool {
    size_t size;
    Kod_Object* data;
} ConstObjectPool;

CallFrame init_call_frame(CallFrame* parent, Environment* globals);
void free_call_frame(CallFrame* frame, ConstObjectPool* cop);

typedef struct VirtualMachine {
    CompiledModule* module;
    ConstObjectPool cop; 
} VirtualMachine;

VirtualMachine init_vm(CompiledModule* module);
void destroy_vm(VirtualMachine* vm);

Kod_Object* vm_run_entry(VirtualMachine* vm);
