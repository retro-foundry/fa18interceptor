/* Glue for draw_polygon $C2FF48: every register is live after it. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "hardware.h"
#include "memory.h"
#include "render_polygon.h"

int prepare_registers(uint16_t last_size); /* glue_batch34.c */
void composite_registers(void);            /* glue_render_polygon.c */
void clear_mask_registers(void);           /* glue_render_polygon.c */
void mask_between_registers(void);         /* glue_batch25.c */

int glue_C2FF48(void) {
    uint16_t last_size = custom_written(BLTSIZE);
    uint16_t colour = rd_u16(CURRENT_COLOUR);
    int bit, given;

    draw_polygon();
    if (prepare_registers(last_size)) return glue_return();
    given = rd_s16(LINE_COLOUR) >= 0;
    if (given) SET_W(D(5), rd_u16(POLY_MASK_BLIT));
    if (given && rd_u16(POLY_MASK_BLIT)) {
        mask_between_registers();
    } else {
        uint8_t planes = rd_u8(LINE_PLANES);
        for (bit = 0; bit < 4; bit++) {
            if (!(planes & (1 << bit))) continue;
            D(0) = (uint32_t)(12 - 4 * bit);
            if (bit == 0) SET_W(D(4), rd_u16(POLY_COMPLEMENT));
            if (given) {
                SET_W(D(3), rd_u16(LINE_COLOUR));
                if (bit) {
                    SET_W(D(4), rd_s16(POLY_COMPLEMENT) >> bit);
                    SET_W(D(3), (int16_t)D(3) >> bit);
                }
            } else {
                SET_W(D(3), colour >> bit); /* POLY_PLANE_BITS at that plane */
            }
            composite_registers();
        }
    }
    clear_mask_registers();
    return glue_return();
}
