#pragma once

#include "../object.h"

void init_int_attributes();
Kod_Object* new_int_object(int64_t value);
Environment* get_int_attributes();