#pragma once

#include <stdint.h>

uint32_t io_read(const char* filename, char** buffer);
void io_write(const char* filename, const char* src);