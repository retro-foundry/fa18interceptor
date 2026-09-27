#include "record_stride_gate.h"

int fa18_decrement_record_stride_words(uint8_t state_flag,
                                       uint16_t words[FA18_RECORD_STRIDE_WORDS]) {
    if (!words) return -1;
    if (state_flag) return 0;
    for (unsigned index = 0; index != FA18_RECORD_STRIDE_WORDS; ++index)
        words[index] = (uint16_t)(words[index] - 1u);
    return 0;
}
