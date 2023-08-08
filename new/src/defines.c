#include "defines.h"

i64 strtoi64(const char* nptr, char** endptr, i32 base) {
    const char *p = nptr;
    bool negative = false;
    bool got_digit = false;
    int64_t ret = 0;
 
    if (!(base == 0 || base >= 2)) return 0;
    if (!(base <= 36)) return 0;
 
    while (isspace(*nptr)) nptr++;
 
    if(*nptr == '-') {
        negative = true;
        nptr++;
    } else if(*nptr == '+')
        nptr++;
 
    if((base==0 || base==16) && *nptr=='0' && tolower(*(nptr+1))=='x') {
        base = 16;
        nptr += 2;
    }
 
    if(base == 0) {
        if(*nptr=='0')
            base = 8;
        else
            base = 10;
    }
 
    while(*nptr) {
        char cur = tolower(*nptr);
        int v;
 
        if(isdigit(cur)) {
            if(cur >= '0'+base)
                break;
            v = cur-'0';
        } else {
            if(cur<'a' || cur>='a'+base-10)
                break;
            v = cur-'a'+10;
        }
        got_digit = true;
 
        if(negative)
            v = -v;
 
        nptr++;
 
        if(!negative && (ret>INT64_MAX/base || ret*base>INT64_MAX-v)) {
            ret = INT64_MAX;
        }
        else if (negative && (ret<INT64_MAX / base || ret*base<INT64_MAX - v)) {
            ret = INT64_MAX;
        }
        else
            ret = ret*base + v;
    }
 
    if(endptr)
        *endptr = (char*)(got_digit ? nptr : p);
 
    return ret;
}

void* kod_malloc(size_t n) {
    return malloc(n);
}

void* kod_calloc(size_t n, size_t elem_size) {
    return calloc(n, elem_size);
}

void* kod_realloc(void* p, size_t new_size) {
    return realloc(p, new_size);
}

void kod_free(void* p) {
    free(p);
}
