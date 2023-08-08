#pragma once

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "da.h"

// #define DEBUG_TOKEN
// #define DEBUG_LEXER
// #define DEBUG_PARSER
// #define DEBUG_COMPILER
// #define DEBUG_VM

#define LOG(type, msg) printf("%s: %s", type, msg)
#define LOG_ARGS(type, fmt, ...) printf("%s: " fmt, type, __VA_ARGS__)

#define INFO(msg) LOG("Info", msg)
#define INFO_ARGS(fmt, ...) LOG_ARGS("Info", fmt, __VA_ARGS__)

#define WARNING(msg) LOG("Warning", msg)
#define WARNING_ARGS(fmt, ...) LOG_ARGS("Warning", fmt, __VA_ARGS__)

#define ERROR_LOG(type, msg) printf("%sError: %s", type, msg)
#define ERROR_ARGS(type, fmt, ...) printf("%sError: " fmt, type, __VA_ARGS__)

#define UNIMPLEMENTED printf("Unimplemented: " __FILE__ ":%d inside %s\n", __LINE__, __FUNCTION__)

#define unwrap(result) {Result res = result; if (res.is_err) { ERROR_LOG("Unwrap", res.what); exit(1); } }

#define isdigit(c) (c >= '0' && c <= '9')
#define isspace(c) (c == ' ' || c == '\r' || c == '\n' || c == '\t')
#define tolower(c) ((c >= 'A' && c <= 'Z') ? c | (1 << 5) : c)
#define toupper(c) ((c >= 'a' && c <= 'z') ? c & ~(1 << 5) : c)

#define STATIC_ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef union Result {
    size_t is_err;
    const char* what;
} Result; 

inline Result result_ok() { return (Result){0}; }
inline Result result_error(const char* what) { return (Result){.what=what}; }

i64 strtoi64(const char* nptr, char** endptr, i32 base);

void* kod_malloc(size_t n);
void* kod_calloc(size_t n, size_t elem_size);
void* kod_realloc(void* p, size_t new_size);
void kod_free(void* p);