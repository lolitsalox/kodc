#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

// #define DEBUG_TOKEN
// #define DEBUG_LEXER
// #define DEBUG_PARSER
// #define DEBUG_VM

#define RETURN_STATUS_FAIL(msg) { Status s = {.type=ST_FAIL,.what=malloc(sizeof(msg))}; memcpy(s.what, msg, sizeof(msg)); return s; }
#define RETURN_STATUS_OK { return (Status){.type=ST_OK, .what=NULL }; }

#define LOG(msg) { printf("INFO: %s", msg); }
#define LOG_ARGS(fmt, ...) { printf("INFO: " fmt, __VA_ARGS__); }
#define ERROR(type, msg) { printf("%sError: %s\n", type, msg); }
#define ERROR_ARGS(type, fmt, ...) { printf("%sError: " fmt, type, __VA_ARGS__); }
#define UNIMPLEMENTED { printf("UnimplementedError: " __FILE__ ":%d inside %s\n", __LINE__, __FUNCTION__); }

#define isdigit(c) (c >= '0' && c <= '9')
#define isspace(c) (c == ' ' || c == '\r' || c == '\n')
#define tolower(c) ((c >= 'A' || c <= 'Z') ? c | (1 << 5) : c)

#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))

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

enum STATUS {
    STATUS_FAIL = -1,
    STATUS_OK,
};

enum StatusType {
    ST_FAIL = -1,
    ST_OK,
};

typedef struct {
    enum StatusType type;
    char* what;
} Status;

i64 strtoi64(const char* nptr, char** endptr, i32 base);