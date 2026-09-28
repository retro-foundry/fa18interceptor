#include "rounded_signed_divide.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

int main(void) {
    int16_t result = 0;
    assert(fa18_round_signed_divide(104, 10, &result) == 0 && result == 10);
    assert(fa18_round_signed_divide(105, 10, &result) == 0 && result == 11);
    assert(fa18_round_signed_divide(-104, 10, &result) == 0 && result == -10);
    assert(fa18_round_signed_divide(-105, 10, &result) == 0 && result == -11);
    assert(fa18_round_signed_divide(100, 3, &result) == 0 && result == 34);
    assert(fa18_round_signed_divide(105, -10, &result) == 0 && result == -11);
    assert(fa18_round_signed_divide(1, 0, &result) == -1);
    assert(fa18_round_signed_divide(INT32_MAX, 1, &result) == -1);
    assert(fa18_round_signed_divide(1, 1, NULL) == -1);
    puts("rounded signed divide contract passed");
    return 0;
}
