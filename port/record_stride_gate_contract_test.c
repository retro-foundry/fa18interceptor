#include "record_stride_gate.h"

#include <assert.h>

int main(void) {
    uint16_t words[FA18_RECORD_STRIDE_WORDS];
    for (unsigned index = 0; index != FA18_RECORD_STRIDE_WORDS; ++index)
        words[index] = (uint16_t)(index * 0x20u);
    words[0] = 0;
    assert(fa18_decrement_record_stride_words(0, words) == 0);
    assert(words[0] == 0xffff);
    for (unsigned index = 1; index != FA18_RECORD_STRIDE_WORDS; ++index)
        assert(words[index] == (uint16_t)(index * 0x20u - 1u));
    assert(fa18_decrement_record_stride_words(1, words) == 0);
    assert(words[0] == 0xffff && words[15] == 0x01df);
    assert(fa18_decrement_record_stride_words(0, 0) == -1);
    return 0;
}
