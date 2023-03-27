// #pragma once

// #include "env.h"
// #include <stdint.h>
// #include <stdbool.h>
// #include "../parser/ast.h"
// #include "../misc/StringArray.h"
// #include "../compiler/compiler.h"

// typedef struct Object Object;

// enum ObjectType {
//     OBJECT_NULL,
//     OBJECT_INT,
//     OBJECT_FLOAT,
//     OBJECT_STRING,
//     OBJECT_CODE,
//     OBJECT_NATIVE_FUNCTION,
//     OBJECT_TYPE,
// };

// typedef struct NativeFunctionObject {
//     char* name;
//     Object* (*caller)(Environment* env, linked_list_t params);
// } NativeFunctionObject;

// struct Object {
//     enum ObjectType type;
//     Environment* attributes;
//     int ref_count;

//     union {
//         int64_t _int;
//         double _float;
//         char* _string;
//         Code _code;
//         NativeFunctionObject _native_function;
//     };
// };

// const char* object_type_to_str(enum ObjectType type);
// void print_object(Object* object, uint32_t indent_level);
// Object* new_object(Object object);
// bool delete_object(Object* object);
// void free_object(Object* object);

// void ref_object(Object* object);
// void deref_object(Object* object);

// typedef struct ObjectNode {
//     Object* object;
//     struct ObjectNode* down;
// } ObjectNode;

// ObjectNode init_object_node();
// ObjectNode* new_object_node();
// bool delete_object_node(ObjectNode* object_node);
// void free_object_node(ObjectNode* object_node);

// typedef struct ObjectStack {
//     ObjectNode* top;
//     bool empty;
// } ObjectStack;

// ObjectStack init_object_stack();
// ObjectStack* new_object_stack();

// void object_stack_push(ObjectStack* object_stack, Object object);
// Object object_stack_top(ObjectStack* object_stack);
// Object object_stack_pop(ObjectStack* object_stack);

// bool delete_object_stack(ObjectStack* object_stack);
// void free_object_stack(ObjectStack* object_stack);