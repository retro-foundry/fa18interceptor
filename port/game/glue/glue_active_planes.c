/* Register flow of the four plane submissions and display stage ($C2FD8C). */
#include "glue.h"
#include "ports_glue.h"

#include "active_planes.h"
#include "globals.h"
#include "hardware.h"
#include "memory.h"
#include "render_polygon.h"

int display_records_with_registers(int wide);
int prepare_registers(uint16_t last_size, int16_t last_row);
void composite_registers(void);

typedef struct PlaneGlueContext { int copied; } PlaneGlueContext;

static int select_with_registers(int wide, void *context) {
    int result;
    if (!wide) {
        uint16_t count = rd_u16(ACTIVE_PLANE_BUSY_3 + 2);
        D(0) = count;
        SET_W(D(3), count);
    }
    result = display_records_with_registers(wide);
    if (wide) ((PlaneGlueContext *)context)->copied = !result;
    return result;
}

static int polygon_with_registers(void *context) {
    uint16_t last_size = custom_written(BLTSIZE);
    int result;
    (void)context;
    result = prepare_polygon();
    prepare_registers(last_size, rd_s16(LINE_LAST_ROW));
    return result;
}

static void composite_with_registers(void *context) {
    (void)context;
    D(0) = 8;
    D(3) = 0;
    D(4) = 0;
    composite_polygon_plane(2, PLANE_CLEAR);
    composite_registers();
}

int glue_C2FD8C(void) {
    ActivePlaneHooks hooks = {select_with_registers, polygon_with_registers,
                              composite_with_registers, 0};
    PlaneGlueContext context = {0};
    gaddr table = rd_u32(PAGE_PLANE_TABLE);
    uint32_t saved_mask = rd_u32(POLY_MASK_PLANE);
    uint16_t size = (uint16_t)((rd_u16(LINE_LAST_ROW) << 6) + 0x14u);
    hooks.context = &context;

    A(2) = table;
    A(0) = 0xDFF000u;
    SET_W(D(6), size);
    D(4) = rd_u32(table + 12) + 0x28u;
    SET_W(D(2), 0x0100);
    D(5) = 0xFFFFFFFFu;
    /* The 68000 saves the mask pointer on the stack across both selectors. */
    A(7) -= 4;
    wr_u32(A(7), saved_mask);
    submit_active_planes(&hooks);
    A(7) += 4;
    if (context.copied) {
        A(0) = CORNER_RECORDS + 22;
        A(4) = DISPLAY_SECONDARY_RECORD + 22;
    }
    return glue_return();
}
