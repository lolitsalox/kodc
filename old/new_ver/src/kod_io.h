#pragma once

#include "defines.h"

#pragma warning(disable : 4996)
#pragma warning(disable : 6011)

enum STATUS io_read(const char* filename, char** buffer, size_t* buffer_size, char** err) {
    FILE* fp; 
    long fsize = 0;

    if (!(fp = fopen(filename, "rb"))) {
        *err = "Couldn't open file";
        return STATUS_FAIL;
    }

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *buffer = (char*) malloc(fsize * sizeof(char) + 1);
    if (fread(*buffer, fsize, 1, fp) != 1) {
        *err = "Couldn't read file";
        fclose(fp);
        return STATUS_FAIL;
    }

    (*buffer)[fsize] = 0;

    fclose(fp);
    *buffer_size = (size_t)fsize;
    return STATUS_OK;
}

enum STATUS io_write(const char* filename, const char* src, char** err) {
    FILE* fp = NULL;

    if (!(fp = fopen(filename, "wb"))) {
        *err = "Couldn't open file";
        return STATUS_FAIL;
    }

    if (fwrite(src, strlen(src), 1, fp) != 1) {
        *err = "Couldn't write to file";
        fclose(fp);
        return STATUS_FAIL;
    }

    fclose(fp);
    return STATUS_OK;
}
