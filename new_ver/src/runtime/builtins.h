#pragma once

#include "../runtime/vm.h"
#include "../runtime/objects/native_object.h"

Status builtins_init(VirtualMachine* vm);
Status builtins_destroy(VirtualMachine* vm);