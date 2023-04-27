#pragma once

#include <defines.h>
#include <compiler/compiler.h>
#include "objects/kod_object.h"
#include "objects/object_map.h"
#include "objects/frame_stack.h"
#include "objects/object_stack.h"

typedef struct VirtualMachine {
    FrameStack frame_stack;
    bool running;
    bool repl;
    ObjectMap globals;
    CompiledModule* module;
    KodObject** constant_objects;
    size_t constant_objects_size;
    bool initialized;
} VirtualMachine;

Status vm_init(CompiledModule* module, bool repl, VirtualMachine* out);
Status vm_destroy(VirtualMachine* vm);

Status vm_run_code_object(VirtualMachine* vm, Code* obj, ObjectMap* initial_env, KodObject** out);