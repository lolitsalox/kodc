#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

uint32_t io_read(const char* filename, char** buffer) {
    FILE* fp; 
    long fsize = 0;

    if (!(fp = fopen(filename, "rb"))) {
        printf("[%s]: Error - couldn't open file\n", filename);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *buffer = (char*) malloc(fsize * sizeof(char) + 1);
    if (fread(*buffer, fsize, 1, fp) != 1) {
        printf("[%s]: Error - couldn't read file\n", filename);
        fclose(fp);
        exit(1);
    }
    (*buffer)[fsize] = 0;

    fclose(fp);
    return fsize;
}

void io_write(const char* filename, const char* src) {
    FILE* fp = NULL;

    if (!(fp = fopen(filename, "wb"))) {
        printf("[%s]: Error - couldn't open file\n", filename);
        exit(1);
    }

    if (fwrite(src, strlen(src), 1, fp) != 1) {
        printf("[%s]: Error - couldn't write to file\n", filename);
        fclose(fp);
        exit(1);
    }

    fclose(fp);
}
