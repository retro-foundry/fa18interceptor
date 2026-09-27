#ifndef FA18_RECORD_STRIDE_GATE_H
#define FA18_RECORD_STRIDE_GATE_H

#include <stdint.h>

enum { FA18_RECORD_STRIDE_WORDS = 16 };

/* `$C22C80-$C22CCD`: decrement the first word in each observed $20-byte
 * record stride when the caller-owned state flag is clear. */
int fa18_decrement_record_stride_words(uint8_t state_flag,
                                       uint16_t words[FA18_RECORD_STRIDE_WORDS]);

#endif
