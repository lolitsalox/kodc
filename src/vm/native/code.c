
#include "code.h"
#include "../vm.h"

#include <stdio.h>
#include <stdlib.h>

static Environment code_attributes;

void init_code_attributes() {
    init_environment(&code_attributes);
}

Kod_Object* new_code_object(Code value) {
    debug_print("creating code object\n%s", "");
    Environment attributes;
    init_environment(&attributes);
    update_environment(&attributes, &code_attributes);
    Kod_Object* obj = new_object(OBJECT_CODE, attributes);
    obj->_code.parent_closure = NULL;
    // obj->_code = deep_copy_code(value);
    debug_print("new code object at %p - ", obj);
    if (DEBUG) print_code(&obj->_code, "\n", NULL, NULL);
    return obj;
}

Environment* get_code_attributes() {
    return &code_attributes;
}