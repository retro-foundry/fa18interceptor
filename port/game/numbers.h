#ifndef FA18_GAME_NUMBERS_H
#define FA18_GAME_NUMBERS_H

#include <stdint.h>

#include "memory.h"

/* Eight-digit packed BCD of `value` (values above 99,999,999 overflow the
 * top digit, as in the original). */
uint32_t to_packed_bcd(uint32_t value);

/* Convert the display value to packed BCD for the numeral plotter. */
void pack_display_value(void);

/* Write `count` decimal digits of `value` backwards, ending just before
 * `end`; unless `keep_zeros`, leading zeros (not the last digit) become
 * spaces. Returns the address of the first digit. */
gaddr format_decimal(gaddr end, uint32_t value, int count, int keep_zeros);

#endif
