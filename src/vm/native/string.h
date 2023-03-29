#pragma once

#include "../object.h"

void init_string_attributes();
Kod_Object* new_string_object(char* value);
Environment* get_string_attributes();