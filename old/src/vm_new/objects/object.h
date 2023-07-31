#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <stdio.h>

#define MAX_REPR_BUFFER_SIZE (1 << 8) 
#define UNIMPLEMENTED { printf("UnimplementedError: " __FILE__ ":%d inside %s\n", __LINE__, __FUNCTION__); return NULL; }

extern int DEBUG;

#if 0
#define debug_print(fmt, ...) \
        do { if (DEBUG) fprintf(stdout, "DEBUG: %s:%d: " fmt, __FILE__, \
                                __LINE__, __VA_ARGS__); } while (0)
#else
#define debug_print(fmt, ...) \
        do { if (DEBUG) fprintf(stdout, "DEBUG: " fmt, __VA_ARGS__); } while (0)
#endif


enum KodObjectType {
    OBJECT_NULL,
    OBJECT_BOOL,
    OBJECT_INTEGER,
    OBJECT_FLOAT,
    OBJECT_STRING,
    OBJECT_CODE,
    OBJECT_NATIVE_FUNCTION,
    OBJECT_TYPE,
    OBJECT_OBJECT,
    OBJECT_TUPLE,
    OBJECT_DICT,
};

typedef struct KodTypeObject KodTypeObject;

typedef struct KodObject {
    enum KodObjectType type;
    uint32_t ref_count;
    KodTypeObject* type_object; // each object has a type
} KodObject;


typedef KodObject* (*binaryfunc) (KodObject* a, KodObject* b);
typedef KodObject* (*unaryfunc) (KodObject* a);
typedef size_t (*hashfunc) (KodObject* a);
typedef KodObject* (*methfunc) (KodObject* a, KodObject* args, KodObject* kwargs);
typedef KodObject* (*typefunc) (KodTypeObject* a, KodObject* args, KodObject* kwargs);
typedef int (*boolfunc) (KodObject* a);
typedef KodObject* (*builtinfunc) (KodObject* args, KodObject* kwargs);

typedef struct Kod_MethodDef {
    char* name;
    methfunc method;
    uint8_t flags; // does take args and stuff like that
    char* doc;  // help
} Kod_MethodDef;

KodObject* ref_object(KodObject* self);
bool deref_object(KodObject* self);
char* object_type_to_str(enum KodObjectType);

#define As_Object(a) ((KodObject*)a)