#include "projection_page_blitter.h"

#include <assert.h>

int main(void) {
    uint8_t chip[0xc000] = {0};
    const uint32_t planes[FA18_COPPER_PAGE_PLANES] = {
        0x0000, 0x2000, 0x4000, 0x6000, 0x8000
    };
    FA18FivePlanePage page;
    FA18FivePlaneChipBinding binding;
    FA18ProjectionPageBlitter blitter;
    FA18BlitOperation operation = { .bltafwm = 0xffff, .bltalwm = 0xffff };
    FA18RendererLaneStage lanes = {
        {0x6000, 0x4000, 0x2000, 0x0000},
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    fa18_five_plane_page_init(&page);
    assert(fa18_five_plane_chip_binding_init(&binding, chip, sizeof chip, planes) == 0);
    assert(fa18_projection_page_blitter_init(&blitter, &page, &binding,
                                              &operation, &lanes) == 0);
    assert(blitter.initialized && blitter.line_submissions == 0);
    ++lanes.plane_pointers[0];
    assert(fa18_projection_page_blitter_init(&blitter, &page, &binding,
                                              &operation, &lanes) == -1);
    assert(fa18_projection_page_emit_blitter(0, 0) == -1);
    assert(fa18_projection_page_emit_final(0, 0) == -1);
    return 0;
}
