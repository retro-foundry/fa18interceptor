#ifndef FA18_HEX_FIELD_H
#define FA18_HEX_FIELD_H
#include <stddef.h>
#include <stdint.h>

/* Complete $C0F56A. Width is the raw low argument byte, interpreted signed.
 * Positive width writes buffer[anchor+1..anchor+width], highest byte first
 * in time, then blanks leading zeroes while retaining the last digit.
 * Anchor itself is untouched. Width $80 skips conversion but its decrement
 * wraps to $7F and scans for zeroes from anchor-127 through anchor-1.
 * No NUL or alternate formatting is added. Returns 0 on invalid bounds;
 * preceding source-ordered writes remain. */
int fa18_format_native_hex_field(uint8_t *buffer,size_t bytes,size_t anchor,
                                  uint32_t value,uint8_t width);
#endif
