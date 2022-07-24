#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handling.h"

char* compress_buffer(char* buffer) {
    ulong buffer_size = strlen(buffer) * sizeof(char) + 1;
    ulong destLen = compressBound(buffer_size);

    char* output = (char*)malloc(destLen * sizeof(char));

    int test = compress(output, &destLen, buffer, buffer_size);
    
    if (test == Z_OK) return output;
    else if (test == Z_BUF_ERROR) error_output("Could Not Compress Because Buffer Is Too Small");
    else if (test == Z_MEM_ERROR) error_output("Could Not Compress Because There Was Not Enough Memory");
    else error_output("Could Not Compress");
}

char* uncompress_buffer(char* buffer, ulong original_size, ulong compressed_buffer_size) {
    char* output = (char*)malloc(original_size * sizeof(char));

    ulong destLen = compressed_buffer_size;

    int test = uncompress(output, &original_size, buffer, compressed_buffer_size);

    if (test == Z_OK) return output;
    else if (test == Z_BUF_ERROR) error_output("Could Not Compress Because Buffer Is Too Small");
    else if (test == Z_DATA_ERROR) error_output("Could Not Compress Because Data Is Incomplete Or Corrupted");
    else error_output("Could Not Compress");

    return output;
}