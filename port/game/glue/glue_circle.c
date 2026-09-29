/* Register effects of the filled circle for translated callers. The C
 * renderer has already written spans and submitted the blits. */
#include "glue.h"
#include "ports_glue.h"

#include "circle.h"
#include "globals.h"
#include "memory.h"

void pair_registers(void); /* glue_batch33.c */

#define W(n) ((int16_t)D(n))

int glue_C2F1C0(void) {
    int16_t x = W(0), y = W(1), radius = W(6);
    gaddr table, near, far;
    int16_t px, py, error, top, limit;

    draw_filled_circle(x, y, radius);
    SET_W(D(5), (uint16_t)(radius - 1));
    if (radius <= 0) {
        pair_registers();
        return glue_return();
    }
    table = rd_u32(CIRCLE_SPANS_PTR);
    SET_W(D(5), (uint16_t)(radius - 2));
    if (radius != 1) {
        if (radius > 127) {
            radius = 127;
            SET_W(D(6), 127);
        }
        near = table + 2;
        far = near + (gaddr)(radius * 4);
        px = radius;
        py = 0;
        error = (int16_t)(3 - 2 * radius);
        SET_W(D(2), (uint16_t)px);
        SET_W(D(3), 0);
        D(4) = (uint32_t)(uint16_t)error;
        SET_W(D(5), (uint16_t)(radius * 4));
        while (py < px) {
            if (error < 0) {
                error = (int16_t)(error + 4 * py + 6);
            } else {
                int16_t step = (int16_t)(4 * (py - px) + 10);
                SET_W(D(5), (uint16_t)step);
                error = (int16_t)(error + step);
                px--;
                near += 4;
            }
            far -= 4;
            py++;
        }
        SET_W(D(2), (uint16_t)px);
        SET_W(D(3), (uint16_t)py);
        SET_W(D(4), (uint16_t)error);
        A(3) = far;
    }
    A(4) = table + 2;
    SET_W(D(4), (uint16_t)(radius - 1));
    SET_W(D(6), (uint16_t)radius);
    top = (int16_t)(y - (radius - 1));
    SET_W(D(1), (uint16_t)top);
    if (top < 1) {
        int16_t rising;
        top--;
        rising = (int16_t)(W(4) + top);
        SET_W(D(4), (uint16_t)rising);
        if (rising < 0) {
            SET_W(D(6), (uint16_t)(W(6) + rising));
            if (W(6) <= 0) {
                SET_W(D(1), (uint16_t)top);
                return glue_return();
            }
        }
        A(4) += (gaddr)(uint16_t)((uint16_t)(-top) * 4u);
        top = 1;
        SET_W(D(1), 1);
    }
    limit = rd_s16(LINE_LAST_ROW);
    if ((int16_t)(top + W(4) + W(6)) >= limit) {
        int16_t cut = (int16_t)(top + W(4) + W(6) - limit);
        SET_W(D(1), (uint16_t)top);
        SET_W(D(5), (uint16_t)cut);
        if (top > limit) return glue_return();
        SET_W(D(6), (uint16_t)(W(6) - cut));
        if (W(6) < 0) SET_W(D(4), (uint16_t)(W(4) + W(6)));
    } else {
        SET_W(D(1), (uint16_t)top);
    }
    SET_W(D(5), (uint16_t)(top * 8));
    /* Each pass leaves D1 with the residual span length and D7 with BLTSIZE. */
    for (;;) {
        int16_t left = (int16_t)(rd_s16(A(4)) + x);
        int16_t right = (int16_t)(rd_s16(A(4) + 2) + x);
        int16_t span, rem, first, extra;
        uint16_t mask;
        if (left < 0) left = 0;
        if (right >= 320) right = 319;
        span = (int16_t)(right - left);
        rem = (int16_t)(left & 15);
        first = (int16_t)(16 - rem);
        if (first > span) first = (int16_t)(span + 1);
        extra = (int16_t)(span - first);
        mask = rd_u16(0xC2F342u + 2u * (uint16_t)first);
        SET_W(D(0), (uint16_t)((mask >> rem) | (mask << (16 - rem))));
        SET_W(D(7), 0x41);
        if (extra >= 15) {
            int16_t words = (int16_t)((extra + 1) & (int16_t)0xFFF0);
            extra = (int16_t)(extra - words);
            SET_W(D(7), (uint16_t)(W(7) + (words >> 4)));
        }
        if (extra >= 0) {
            SET_W(D(7), (uint16_t)(W(7) + 1));
            SET_W(D(1), (uint16_t)(extra * 2));
        } else {
            SET_W(D(1), (uint16_t)extra);
        }
        SET_W(D(0), (rd_u16(CURRENT_COLOUR) & 8) ? 0x3FA : 0x30A);
        if (W(4) >= 0) {
            A(4) += 4;
            SET_W(D(4), (uint16_t)(W(4) - 1));
            if (W(4) >= 0) continue;
        }
        A(4) -= 4;
        SET_W(D(6), (uint16_t)(W(6) - 1));
        if (W(6) < 0) break;
    }
    return glue_return();
}
