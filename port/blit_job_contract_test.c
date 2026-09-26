#include "blit_job.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    FA18AreaFillPacket fill;
    assert(fa18_build_run060_frame7991_area_fill(&fill) == 0);
    assert(fill.control_a == 0x0d0c && fill.control_b == 2);
    assert(fill.first_mask == 0x00ff && fill.last_mask == 0x00ff);
    assert(fill.c_modulus == 0x28 && fill.b_modulus == 1);
    assert(fill.a_modulus == 1 && fill.d_modulus == 0);
    assert(fill.width_words == 20 && fill.height_rows == 52);
    FA18BlitOperation setup;
    assert(fa18_prepare_c304b2_setup(&fill, &setup) == 0);
    assert(setup.bltcon0 == 0x0d0c && setup.bltcon1 == 2);
    assert(setup.bltapt == 0x76ee && setup.bltbpt == 0x76ee);
    assert(setup.bltcpt == 0x10026 && setup.bltdpt == 0x76ee);
    assert(setup.bltamod == 1 && setup.bltbmod == 1 &&
           setup.bltcmod == 0x28 && setup.bltdmod == 0);
    assert(setup.bltsize == 0x0d14);
    FA18DisplayBlitPacket transition[3];
    assert(fa18_build_run075_frame559_blit_packets(transition) == 0);
    assert(transition[0].control_a == 0x8aea &&
           transition[0].control_b == 0x0053 &&
           transition[0].width_height == 0x1e02);
    assert(transition[1].c_source == 0x6eef &&
           transition[2].d_destination == 0x6e58);
    FA18DisplayBlitGeometry geometry;
    assert(fa18_decode_display_blit_geometry(&transition[0], &geometry) == 0);
    assert(geometry.extent.width_words == 2 && geometry.extent.height_rows == 120);
    assert(geometry.first_mask == 0xffff && geometry.last_mask == 0xffff);
    assert(geometry.source_plane_mask == 0x0a);

    FA18BlitExtent extent = fa18_decode_blit_extent(0x0e14);
    assert(extent.width_words == 20 && extent.height_rows == 56);
    extent = fa18_decode_blit_extent(0x0302);
    assert(extent.width_words == 2 && extent.height_rows == 12);
    FA18BlitOperation operation;
    fa18_prepare_lane_blit(0x0302, 0x007b6a, &operation);
    assert(operation.bltcon0 == 0x0d0c && operation.bltcon1 == 2);
    assert(operation.bltapt == 0x007b6a && operation.bltbpt == 0x007b6a && operation.bltdpt == 0x007b6a);
    assert(operation.bltsize == 0x0302);
    assert(fa18_choose_lane_control(1, 1) == FA18_LANE_CONTROL_C);
    assert(fa18_choose_lane_control(0, 0) == FA18_LANE_CONTROL_B);
    assert(fa18_choose_lane_control(0, 1) == FA18_LANE_CONTROL_A);
    fa18_prepare_adjusted_lane_blit(0x0342, 0x1000, 0x20, 0x0200,
                                    0x0100, 0x0120, 0x0012, 1, &operation);
    assert(operation.bltcon0 == 0x0fec && operation.bltcon1 == 2);
    assert(operation.bltapt == 0x1000 && operation.bltbpt == 0x1020);
    assert(operation.bltcpt == 0x12bfe && operation.bltamod == 1);
    puts("blit job contract passed"); return 0;
}
