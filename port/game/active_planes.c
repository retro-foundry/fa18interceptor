/* Four active-plane submissions and selected-table display ($C2FD8C). */
#include "active_planes.h"

#include "display_records.h"
#include "globals.h"
#include "hardware.h"
#include "memory.h"
#include "render_polygon.h"

#ifndef FA18_NATIVE
static void normalize_busy_count(gaddr count) {
    int16_t first = rd_s16(count), second = rd_s16(count + 2);
    /* CLR.W/SWAP/MOVE.W clears the high word after the signed comparison. */
    wr_u32(count, (uint16_t)(first > second ? first : second));
}

#endif

static void submit_plane(gaddr table, int index, uint16_t control, uint16_t size,
                         gaddr busy_count) {
    uint32_t plane = rd_u32(table + (gaddr)(index * 4)) + 0x28u;
#ifdef FA18_NATIVE
    unsigned rows=size>>6;
    if(!rows) rows=1024;
    for(unsigned i=0;i<rows*20u;++i) wr_u16(plane+2*i,control==0x03FA?0xffff:0);
    (void)busy_count; /* Hardware poll counters have no host event clock. */
#else
    if (busy_count) {
        wr_u16(busy_count, (uint16_t)(rd_u16(busy_count) + count_blitter_polls()));
        normalize_busy_count(busy_count);
    } else wait_blitter();
    custom_write(BLTCON0, control);
    custom_write_ptr(BLTCPT, plane);
    custom_write_ptr(BLTDPT, plane);
    custom_write(BLTSIZE, size);
#endif
}

static int select_record(int wide, const ActivePlaneHooks *hooks) {
    if (hooks && hooks->select_record)
        return hooks->select_record(wide, hooks->context);
    return prepare_display_records(wide, 0);
}

void submit_active_planes(const ActivePlaneHooks *hooks) {
    gaddr table = rd_u32(PAGE_PLANE_TABLE);
    uint16_t size = (uint16_t)((rd_u16(LINE_LAST_ROW) << 6) + 0x14u);
    uint32_t saved_mask;
    int selected, polygon;
    int i;

#ifdef FA18_NATIVE
    submit_plane(table,0,0x0100,size,0);
#else
    wait_blitter();
    custom_write(BLTCON0, 0x0100);
    custom_write(BLTCON1, 0);
    custom_write(BLTADAT, 0xFFFF);
    custom_write(BLTAFWM, 0xFFFF);
    custom_write(BLTALWM, 0xFFFF);
    custom_write(BLTBMOD, 1);
    custom_write(BLTCMOD, 1);
    custom_write(BLTDMOD, 1);
    {
        uint32_t plane = rd_u32(table) + 0x28u;
        custom_write_ptr(BLTCPT, plane);
        custom_write_ptr(BLTDPT, plane);
        custom_write(BLTSIZE, size);
    }
#endif
    submit_plane(table, 1, 0x03FA, size, ACTIVE_PLANE_BUSY_1);
    submit_plane(table, 2, rd_u8(CONDITION_MET_A) ? 0x03FA : 0x0100,
                 size, ACTIVE_PLANE_BUSY_2);
    submit_plane(table, 3, 0x0100, size, ACTIVE_PLANE_BUSY_3);

    saved_mask = rd_u32(POLY_MASK_PLANE);
    wr_u32(POLY_MASK_PLANE, rd_u32(table + 12));
    selected = select_record(0, hooks);
    if (!selected) {
        polygon = hooks && hooks->prepare_polygon
                ? hooks->prepare_polygon(hooks->context) : prepare_polygon();
        if (!polygon && rd_u8(CONDITION_MET_A)) {
            if (hooks && hooks->composite) hooks->composite(hooks->context);
            else composite_polygon_plane(2, PLANE_CLEAR);
        }
    }
    wr_u32(POLY_MASK_PLANE, saved_mask);

    if (rd_u8(CONTEXT_SELECT) || select_record(1, hooks)) {
        wr_u16(DISPLAY_SECONDARY_RECORD, 0);
        return;
    }
    wr_u16(DISPLAY_SECONDARY_RECORD, rd_u16(CORNER_RECORDS));
    for (i = 0; i < 5; ++i)
        wr_u32(DISPLAY_SECONDARY_RECORD + 2 + (gaddr)(i * 4),
               rd_u32(CORNER_RECORDS + 2 + (gaddr)(i * 4)));
}
