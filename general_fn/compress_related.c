#include "error_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

char *compress_buffer(char *buffer, ulong buffer_size, ulong buffer_byte_size) {
  char *output = (char *)malloc(buffer_byte_size * sizeof(char));

  int test = compress((Bytef *)output, &buffer_byte_size, (const Bytef *)buffer,
                      buffer_size);

  if (test == Z_OK)
    return output;
  else if (test == Z_BUF_ERROR)
    error_output((char *)"Could Not Compress Because Buffer Is Too Small");
  else if (test == Z_MEM_ERROR)
    error_output(
        (char *)"Could Not Compress Because There Was Not Enough Memory");
  else
    error_output((char *)"Could Not Compress");

  return output;
}

char *uncompress_buffer(char *buffer, ulong original_size,
                        ulong compressed_buffer_size) {
  char *output = (char *)malloc(original_size * sizeof(char));

  int test = uncompress((Bytef *)output, &original_size, (const Bytef *)buffer,
                        compressed_buffer_size);

  if (test == Z_OK)
    return output;
  else if (test == Z_BUF_ERROR)
    error_output((char *)"Could Not Uncompress Because Buffer Is Too Small");
  else if (test == Z_DATA_ERROR)
    error_output(
        (char *)"Could Not Uncompress Because Data Is Incomplete Or Corrupted");
  else
    error_output((char *)"Could Not Uncompress");

  return output;
}