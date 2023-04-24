#include "kod_object.h"

#define TYPE_HEADER(parent_type) .base={.kind=OBJECT_TYPE,.ref_count=1,.type=&parent_type},

typedef struct KodObjectNumberMethods {
    binary_func add;
    binary_func sub;
    binary_func mul;
    binary_func div;

    unary_func _bool;
    unary_func _int;
    unary_func _float;
    
    unary_func hash;
} KodObjectNumberMethods;

typedef struct KodObjectSubscriptMethods {
    unary_func get;
    unary_func set;
} KodObjectSubscriptMethods;

typedef struct KodObjectType {
    KodObject base;
    char* tp_name;
    KodObjectNumberMethods* as_number;
    KodObjectSubscriptMethods* as_subscript;
    unary_func str;
    unary_func free;
} KodObjectType;

extern KodObjectType KodObjectType_Type;

// Status kod_type_init(char* tp_name, KodObjectType* out);
// Status kod_type_new(char* tp_name, KodObjectType** out);