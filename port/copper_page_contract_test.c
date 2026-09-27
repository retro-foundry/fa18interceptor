#include "copper_page.h"

#include <assert.h>
#include <string.h>

int main(void) {
    /* Observed frame-392 Copper moves from the published list, retained only
     * as a bounded primitive contract rather than a native presentation
     * fixture. */
    static const uint8_t stream[] = {
        0x01, 0x80, 0x00, 0x00, 0x01, 0x82, 0x01, 0x00,
        0x00, 0x8e, 0x05, 0x81, 0x01, 0x00, 0x02, 0x00,
        0x00, 0xe0, 0x00, 0x04, 0x00, 0xe2, 0xdb, 0x30,
        0x00, 0xe4, 0x00, 0x04, 0x00, 0xe6, 0xfa, 0x70,
        0x00, 0xe8, 0x00, 0x05, 0x00, 0xea, 0x19, 0xb0,
        0x00, 0xec, 0x00, 0x05, 0x00, 0xee, 0x38, 0xf0,
        0x00, 0xf0, 0x00, 0x05, 0x00, 0xf2, 0x58, 0x30,
        0x2a, 0x01, 0xff, 0xfe, 0x01, 0x00, 0x52, 0x00,
        0xf2, 0x01, 0xff, 0xfe, 0x01, 0x00, 0x02, 0x00,
        0xff, 0xff, 0xff, 0xfe
    };
    const FA18CopperInstructionStream streams[] = {
        { stream, sizeof stream }
    };
    FA18CopperPageState state;

    uint8_t palette_stream_bytes[] = {
        0x01, 0x80, 0x00, 0x00, 0x01, 0x82, 0x00, 0x00,
        0x01, 0x84, 0x00, 0x00, 0x01, 0x92, 0x02, 0x00,
        0xff, 0xff, 0xff, 0xfe
    };
    FA18CopperMutableInstructionStream palette_stream = {
        palette_stream_bytes, sizeof palette_stream_bytes
    };
    uint16_t palette[32] = { 0 };
    uint32_t updated_mask = 0;
    palette[1] = 0x0100;
    palette[2] = 0x0111;
    palette[9] = 0x0620;
    assert(fa18_update_copper_palette_moves(&palette_stream, 1, palette,
                                            (UINT32_C(1) << 1) |
                                            (UINT32_C(1) << 2) |
                                            (UINT32_C(1) << 9),
                                            &updated_mask) == 0);
    assert(updated_mask == ((UINT32_C(1) << 1) | (UINT32_C(1) << 2) |
                            (UINT32_C(1) << 9)));
    assert(palette_stream_bytes[0] == 0x01 && palette_stream_bytes[1] == 0x80);
    assert(palette_stream_bytes[6] == 0x01 && palette_stream_bytes[7] == 0x00);
    assert(palette_stream_bytes[10] == 0x01 && palette_stream_bytes[11] == 0x11);
    assert(palette_stream_bytes[14] == 0x06 && palette_stream_bytes[15] == 0x20);

    assert(fa18_decode_copper_page_at_vpos(streams, 1, 0x29, &state) == 0);
    assert(state.bplcon0 == 0x0200);
    assert(state.plane_pointers[0] == 0x04db30u);
    assert(state.plane_pointers[4] == 0x055830u);
    assert(state.palette_written_mask == 3 && state.palette[0] == 0 &&
           state.palette[1] == 0x0100);

    assert(fa18_decode_copper_page_at_vpos(streams, 1, 0x2a, &state) == 0);
    assert(state.bplcon0 == 0x5200);
    assert(state.plane_pointers[1] == 0x04fa70u);
    assert(state.plane_pointers[2] == 0x0519b0u);
    assert(state.plane_pointers[3] == 0x0538f0u);

    assert(fa18_decode_copper_page_at_vpos(streams, 1, 0xf2, &state) == 0);
    assert(state.bplcon0 == 0x0200);

    const FA18CopperInstructionStream incomplete[] = { { stream, 8 } };
    assert(fa18_decode_copper_page_at_vpos(incomplete, 1, 0x2a, &state) == -1);
    assert(fa18_decode_copper_page_at_vpos(streams, 1, 0x2a, 0) == -1);
    assert(fa18_decode_copper_page_at_vpos(streams, 1, 0xf2, &state) == 0);

    uint8_t plane_bytes[FA18_COPPER_PAGE_PLANES][FA18_COPPER_PAGE_BYTES] = {{0}};
    FA18CopperPlaneBuffer buffers[FA18_COPPER_PAGE_PLANES];
    for (unsigned plane = 0; plane < FA18_COPPER_PAGE_PLANES; ++plane) {
        buffers[plane] = (FA18CopperPlaneBuffer){
            state.plane_pointers[plane], plane_bytes[plane], sizeof plane_bytes[plane]
        };
    }
    plane_bytes[0][0] = 0x80;
    plane_bytes[2][0] = 0x80;
    plane_bytes[4][0] = 0x40;
    FA18Video video;
    memset(&video, 0, sizeof video);
    for (unsigned colour = 0; colour < 32; ++colour)
        state.palette[colour] = (uint16_t)(0x0100u + colour);
    state.palette_written_mask = UINT32_MAX;
    state.bplcon0 = 0x5200;
    assert(fa18_present_copper_page(&state, buffers, FA18_COPPER_PAGE_PLANES,
                                    &video) == 0);
    assert(video.pixels[0] == 5);
    assert(video.pixels[1] == 16);
    assert(video.pixels[2] == 0);
    assert(video.palette[5] == 0x0105 && video.palette[16] == 0x0110);
    video.palette[5] = 0x0aaau;
    state.palette_written_mask = 1;
    assert(fa18_present_copper_page(&state, buffers, FA18_COPPER_PAGE_PLANES,
                                    &video) == 0);
    assert(video.palette[5] == 0x0aaau);
    state.bplcon0 = 0x0200;
    assert(fa18_present_copper_page(&state, buffers, FA18_COPPER_PAGE_PLANES,
                                    &video) == -1);
    assert(fa18_present_copper_page(&state, buffers, 4, &video) == -1);
    return 0;
}
