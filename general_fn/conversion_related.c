#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

char* ulong_to_str(ulong number) {
    const int digit_count = snprintf(NULL, 0, "%lu", number);

    assert(digit_count > 0);

    char* buf = (char*)malloc((digit_count + 1) * sizeof(char));
    int c = snprintf(buf, digit_count + 1, "%lu", number);

    assert(buf[digit_count] == '\0');
    assert(c == digit_count);

    return buf;
}
