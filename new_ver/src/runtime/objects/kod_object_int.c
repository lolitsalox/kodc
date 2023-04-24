#include "kod_object_int.h"



KodObjectNumberMethods int_as_number = {
    .add=0,
    .sub=0,
    .mul=0,
    .div=0,

    ._int=0,
    ._float=0,
    ._bool=0,

    .hash=0,
};

KodObjectType KodObjectType_Int = {
    TYPE_HEADER(KodObjectType_Type)
    .as_number=&int_as_number,
    .as_subscript=0,
    .free=kod_object_free
};