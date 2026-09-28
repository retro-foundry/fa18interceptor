#include "record_triple_transform.h"

#include <assert.h>

int main(void) {
    const int16_t source[] = { 0x0100, -0x0200, 0x0300, 1, 2, 3 };
    const int16_t matrix[] = { 256, 0, 0, 0, 256, 0, 0, 0, 256 };
    int16_t output[6] = {0};
    FA18RecordTripleTransformInput input = {
        source, 6, 2, 1, 2, -3, 4, matrix, output, 6
    };
    assert(fa18_transform_record_triples(&input) == 0);
    /* `$C1F4B0-$C1F4F6`: ASR.W, ADD.W, MULS.W and ASR.L #8. */
    assert(output[0] == 130 && output[1] == -259 && output[2] == 388);
    assert(output[3] == 2 && output[4] == -2 && output[5] == 5);
    input.destination_word_capacity = 5;
    assert(fa18_transform_record_triples(&input) == -1);
    return 0;
}
