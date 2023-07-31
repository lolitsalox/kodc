#pragma once

#include "defines.h"

Result io_read(const char* filename, char** buffer, size_t* buffer_size) {
    assert(filename && buffer && buffer_size && "One of the parameters is NULL");

    Result res = {0};
    FILE* fp;
    long fsize = 0;

    if (!(fp = fopen(filename, "rb"))) {
        ERROR_ARGS("IO", "Couldn't open file <%s>", filename);
        res.what = "Couldn't open file";
        return res;
    }

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    *buffer_size = (size_t)fsize;

    *buffer = (char*) malloc(fsize * sizeof(char) + 1);
    if (!*buffer) {
        res.what = "Couldn't allocate for buffer";
        return res;
    }

    if (fread(*buffer, fsize, 1, fp) != 1) {
        res.what = "Couldn't read file";
        free(*buffer);
        fclose(fp);
        return res;
    }

    (*buffer)[fsize] = 0;

    fclose(fp);
    return res;
}

Result io_write(const char* filename, const char* src) {
    assert(filename && src && "One of the parameters is NULL");

    Result res = {0};
    FILE* fp = NULL;

    if (!(fp = fopen(filename, "wb"))) {
        res.what = "Couldn't open file";
        return res;
    }

    if (fwrite(src, strlen(src), 1, fp) != 1) {
        res.what = "Couldn't write to file";
        fclose(fp);
        return res;
    }

    fclose(fp);
    return res;
}
