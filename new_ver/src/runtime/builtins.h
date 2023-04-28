#pragma once

#include "../runtime/vm.h"
#include "../runtime/objects/kod_object_native_func.h"

#define STRUCT_BUILTIN_FUNC(builtin_name) {.base={.kind=OBJECT_NATIVE_FUNC, .ref_count=1, .type=&KodType_NativeFunc}, .name=#builtin_name, ._func=native_ ## builtin_name},

Status native_print(VirtualMachine* vm, KodObject *args, KodObject *kwargs, KodObject **out);

Status builtins_init(VirtualMachine* vm);