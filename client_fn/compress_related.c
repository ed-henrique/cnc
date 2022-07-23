#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handling.h"

char* compress_buffer(char* buffer) {
    unsigned long buffer_size = strlen(buffer) * sizeof(char) + 1;
    uLongf destLen = compressBound(buffer_size);

    char* output = (char*)malloc(destLen * sizeof(char));

    if (compress((Bytef*)output, &destLen, (Bytef*)buffer, buffer_size) != Z_OK) error_output("Could Not Compress Buffer");

    return output;
}

char* uncompress_buffer(char* buffer, int original_size) {
    unsigned long buffer_size = strlen(buffer) * sizeof(char) + 1;
    char* output;

    uLongf* destLen = (uLongf*)buffer_size;

    if (compress(output, destLen, buffer, buffer_size) != Z_OK) error_output("Could Not Compress Buffer");

    return output;
}