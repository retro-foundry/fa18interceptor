#include "map_packet_transform.h"

#include <assert.h>

int main(void) {
    const FA18MapPacketTransform transform = {
        UINT32_C(0x000a0014), {1, 2, 3}, 1,
        {{256, 99, 0, 0, 77, 256, 128, 55, 128}}
    };
    const FA18MapPacketPair pairs[] = {{{2, 4}}, {{-2, 8}}};
    FA18MapPacketProjectionRecord output[2];
    FA18MapPacketTransformRegisters registers;
    uint16_t transformed;
    assert(fa18_transform_map_packet_pairs(&transform, pairs, 2, 2, output, 2,
                                            &transformed, &registers) == 0);
    assert(transformed == 2);
    assert(output[0].value[0] == 26 && output[0].value[1] == 52 &&
           output[0].value[2] == 42);
    assert(output[1].value[0] == 18 && output[1].value[1] == 60 &&
           output[1].value[2] == 42);
    assert(registers.last_y_register == 60);
    assert(fa18_transform_map_packet_pairs(&transform, 0, 0, 0, 0, 0,
                                            &transformed, &registers) == 0 &&
           !transformed && !registers.last_y_register);
    assert(fa18_transform_map_packet_pairs(&transform, pairs, 1, 2, output, 2,
                                            &transformed, &registers) == -1);
    return 0;
}
