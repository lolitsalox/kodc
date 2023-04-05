#pragma once

#include "../compiler/compiler.h"

#include "objectStack.h"

#include "objects/dict.h"

typedef struct CallFrame {
    struct CallFrame* parent;
    KodDictObject env;
    ObjectStack stack;
    size_t ip;
} CallFrame;

CallFrame init_call_frame(CallFrame* parent, KodDictObject* globals);
void free_call_frame(CallFrame* frame, KodDictObject* constant_objects);

typedef struct VirtualMachine {
    CompiledModule* module;
    KodDictObject* constant_objects;
    bool repl;
    bool running;
} VirtualMachine;

VirtualMachine init_vm(CompiledModule* module, bool repl);
void destroy_vm(VirtualMachine* vm);

KodObject* vm_run_entry(VirtualMachine* vm);
KodObject* run_code_object(VirtualMachine* vm, Code* code, CallFrame* parent_call_frame, KodDictObject* initial_env, CallFrame* saved_frame);