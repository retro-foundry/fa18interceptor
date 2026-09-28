#ifndef FA18_ROUNDED_SIGNED_DIVIDE_H
#define FA18_ROUNDED_SIGNED_DIVIDE_H

#include <stdint.h>

/* `$C25980-$C259C1`: signed 68000 word divide followed by the source's
 * remainder-threshold rounding. A zero divisor or quotient overflow models
 * the original DIVS.W exception by returning -1. */
int fa18_round_signed_divide(int32_t dividend, int16_t divisor,
                             int16_t *result);

#endif
