#pragma once

#include <stdint.h>

struct env_t;
struct kod_object_t;

void builtins_init(struct env_t* global_env);
struct kod_object_t* make_int(int64_t val);
struct kod_object_t* make_string(char* val);