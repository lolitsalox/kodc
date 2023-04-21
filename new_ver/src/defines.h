#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

// #define DEBUG_TOKEN
// #define DEBUG_LEXER
// #define DEBUG_PARSER

#define LOG(msg) { printf("INFO: %s", msg); }
#define LOG_ARGS(fmt, ...) { printf("INFO: " fmt, __VA_ARGS__); }
#define ERROR(type, msg) { printf("%sError: %s\n", type, msg); }
#define ERROR_ARGS(type, fmt, ...) { printf("%sError: " fmt, type, __VA_ARGS__); }
#define UNIMPLEMENTED { printf("UnimplementedError: " __FILE__ ":%d inside %s\n", __LINE__, __FUNCTION__); }

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

enum STATUS {
    STATUS_FAIL = -1,
    STATUS_OK,
};