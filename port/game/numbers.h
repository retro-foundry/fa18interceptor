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

/* Print `value` in decimal into the `width`-character field that ends at
 * field + offset + width, blanking leading zeros. */
void print_number(gaddr field, int16_t offset, uint32_t value, int8_t width);

/* DISPLAY_VALUE = the eight packed BCD digits of DISPLAY_VALUE_BCD. */
void unpack_display_value(void);
/* $C25A00: DBRA-add the selected table weight into the packed-nibble total. */
uint32_t add_repeated_nibble_weight(gaddr table_word, uint16_t repeats, uint32_t total);

/* The date line ($C24E2C): from the mode table's +$08 long / 3600 (DIVU),
 * the month name (bits 5-6, capped at $7F) copied reversed into DATE_LINE
 * +$0C..+$14, and the day (low five bits + 1, at most 30) printed two wide
 * at +$15. */
void format_date_line(void);

#endif
