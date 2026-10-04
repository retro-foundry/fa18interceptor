/* Remaining postflight helpers. Complete history/display/render owners are
 * in hud_history_stream.c, hud_projection_parents.c and render_parents.c. */
#include "glue.h"
#include "ports_glue.h"
#include "globals.h"
#include "memory.h"
#include "glue_text.h"

int glue_C28E28(void) {
    return glue_complete_zone_exit();
}

/* Legacy register tail retained for C3003A's enclosing adapter. Its
 * complete original-child integration remains function-porting work. */
void mark_polygon_registers(uint16_t last_size) {
    int16_t count = rd_s16(0xC4B432u);

    A(0) = 0xC4B432u + 2;
    A(4) = POLY_VERTICES;
    SET_W(D(7), (uint16_t)count);
    if (count <= 0) return;
    A(4) += 2 + (gaddr)(4 * count);
    A(0) += (gaddr)(4 * count);
    SET_W(D(0), rd_u16(POLY_VERTICES + (gaddr)(4 * count)));
    SET_W(D(7), 0xFFFF);
    if (prepare_polygon_to_row_registers(last_size)) return;
    D(0) = 4;
    D(3) = 1;
    blit_lane_registers();
    clear_mask_registers();
}
