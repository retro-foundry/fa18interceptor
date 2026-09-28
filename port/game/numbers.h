#ifndef FA18_GAME_NUMBERS_H
#define FA18_GAME_NUMBERS_H

#include <stdint.h>

/* Eight-digit packed BCD of `value` (values above 99,999,999 overflow the
 * top digit, as in the original). */
uint32_t to_packed_bcd(uint32_t value);

/* Convert the display value to packed BCD for the numeral plotter. */
void pack_display_value(void);

#endif
