/* Copper and bitplane display, resolved per scanline. Copper instructions
 * whose WAIT is satisfied anywhere on a line take effect before that line is
 * drawn; bitplane words are fetched from the live Chip RAM at draw time. */
#include "machine.h"
#include "bus.h"

#include <string.h>

#define MAX_COPPER_PER_LINE 256

void fa18_copper_restart(FA18Machine *m) {
    m->copper_pc = m->cop1lc;
    m->copper_waiting = 0;
}

static int beam_reached(int vpos, int hpos, uint16_t w1, uint16_t w2) {
    int ve = ((w2 >> 8) & 0x7F) | 0x80, he = w2 & 0xFE;
    int vp = w1 >> 8, hp = w1 & 0xFE;
    int beam = ((vpos & 0xFF) & ve) << 8 | (hpos & he);
    int target = (vp & ve) << 8 | (hp & he);
    return beam >= target;
}

/* Count one executed instruction for the line's DMA slots; a run starts
 * where the WAIT that released it was satisfied. */
static void copper_slot(FA18Machine *m, int vpos, int resumed) {
    int n = m->copper_segments;
    if (resumed || n == 0) {
        int start = 0;
        /* The WAIT's first word holds the beam position it waited for. */
        if (resumed && (m->copper_wait_v >> 8) == (vpos & 0xFF)) start = m->copper_wait_v & 0xFE;
        if (n == 16) n = 15;
        else m->copper_segments = n + 1;
        m->copper_segment_start[n] = start;
        m->copper_segment_count[n] = 0;
    } else {
        n--;
    }
    m->copper_segment_count[n]++;
}

void fa18_copper_run_until(FA18Machine *m, int vpos, int hpos) {
    int budget = MAX_COPPER_PER_LINE;
    m->copper_segments = 0;
    if (!(m->dmacon & 0x0200) || !(m->dmacon & 0x0080)) return;
    while (budget-- > 0) {
        uint16_t w1, w2;
        int resumed = 0;
        if (m->copper_waiting) {
            if (!beam_reached(vpos, hpos - 1, m->copper_wait_v, m->copper_wait_h)) return;
            m->copper_waiting = 0;
            resumed = 1;
        }
        copper_slot(m, vpos, resumed);
        if (fa18_meter_enabled) ++fa18_emulation_meter.copper_instructions;
        w1 = fa18_chip16(m, m->copper_pc);
        w2 = fa18_chip16(m, m->copper_pc + 2);
        m->copper_pc = (m->copper_pc + 4) & (FA18_CHIP_SIZE - 1);
        if (!(w1 & 1)) {
            uint16_t reg = w1 & 0x1FE;
            if (reg < 0x40 || (reg < 0x80 && !m->copper_danger)) {
                /* Illegal MOVE stops the Copper until the next frame. */
                m->copper_waiting = 1;
                m->copper_wait_v = 0xFFFF;
                m->copper_wait_h = 0xFFFE;
                return;
            }
            fa18_custom_write(m, reg, w2);
            continue;
        }
        if (!(w2 & 1)) {
            m->copper_wait_v = w1;
            m->copper_wait_h = w2;
            if ((w1 & 0xFFFE) == 0xFFFE && (w2 & 0xFFFE) == 0xFFFE) {
                m->copper_waiting = 1; /* end of list */
                return;
            }
            m->copper_waiting = 1;
            continue;
        }
        if (beam_reached(vpos, hpos - 1, w1, w2)) m->copper_pc = (m->copper_pc + 4) & (FA18_CHIP_SIZE - 1);
    }
}

void fa18_display_line(FA18Machine *m, int vpos) {
    uint16_t *row;
    uint8_t *indices = NULL;
    uint16_t con0 = m->custom[0x100 >> 1], con1 = m->custom[0x102 >> 1];
    uint16_t con2 = m->custom[0x104 >> 1];
    uint16_t diwstrt = m->custom[0x08E >> 1], diwstop = m->custom[0x090 >> 1];
    uint16_t ddfstrt = m->custom[0x092 >> 1] & 0xFC, ddfstop = m->custom[0x094 >> 1] & 0xFC;
    int vstart = diwstrt >> 8, vstop = (diwstop >> 8) | ((diwstop & 0x8000) ? 0 : 0x100);
    int hstart = diwstrt & 0xFF, hstop = (diwstop & 0xFF) | 0x100;
    int planes = (con0 >> 12) & 7, hires = (con0 & 0x8000) != 0, dual = (con0 & 0x0400) != 0;
    int words, plane, x, y = vpos - FA18_SCREEN_VSTART;
    const uint16_t *colors = &m->custom[0x180 >> 1];
    int in_window = vpos >= vstart && vpos < vstop;
    int dma = (m->dmacon & 0x0300) == 0x0300;
    uint8_t pixels[FA18_SCREEN_W * 2 + 32];

    if (y >= 0 && y < FA18_SCREEN_H) {
        row = m->screen + y * FA18_SCREEN_W;
        for (x = 0; x < FA18_SCREEN_W; x++) row[x] = colors[0] & 0xFFF;
        if (m->capture_indices) {
            indices = m->screen_indices + y * FA18_SCREEN_W;
            memset(indices, 0, FA18_SCREEN_W);
        }
    } else {
        row = NULL;
    }
    if (!in_window || !dma || planes == 0 || ddfstop < ddfstrt) return;
    if (hires && planes > 4) planes = 4;
    if (!hires && planes > 6) planes = 6;

    words = hires ? ((ddfstop - ddfstrt) >> 2) + 2 : ((ddfstop - ddfstrt) >> 3) + 1;
    if (words * 16 > (int)sizeof pixels - 16) words = ((int)sizeof pixels - 16) / 16;
    memset(pixels, 0, sizeof pixels);
    for (plane = 0; plane < planes; plane++) {
        uint32_t pt = m->bplpt[plane];
        int16_t mod = (int16_t)m->custom[(plane & 1) ? 0x10A >> 1 : 0x108 >> 1];
        int delay = (plane & 1) ? (con1 >> 4) & 15 : con1 & 15;
        int w;
        for (w = 0; w < words; w++) {
            if (fa18_meter_enabled) ++fa18_emulation_meter.bitplane_words;
            uint16_t data = fa18_chip16(m, pt + (uint32_t)w * 2);
            int bit;
            for (bit = 0; bit < 16; bit++) {
                int px = w * 16 + bit + delay * (hires ? 2 : 1);
                if (px < (int)sizeof pixels && (data & (0x8000 >> bit))) pixels[px] |= (uint8_t)(1 << plane);
            }
        }
        m->bplpt[plane] = (pt + (uint32_t)words * 2 + (uint32_t)(int32_t)mod) & (FA18_CHIP_SIZE - 1);
    }
    if (!row) return;
    {
        /* The first fetched lowres pixel appears at DIW position ddfstrt*2+17. */
        int first = hires ? ddfstrt * 2 + 9 : ddfstrt * 2 + 17;
        int count = words * 16;
        int i;
        for (i = 0; i < count; i++) {
            int h = hires ? first + i / 2 : first + i;
            int sx = h - FA18_SCREEN_HSTART;
            uint8_t p;
            int index;
            if (hires && (i & 1)) continue;
            if (h < hstart || h >= hstop || sx < 0 || sx >= FA18_SCREEN_W) continue;
            p = pixels[i];
            if (dual) {
                int pf1 = (p & 1) | ((p >> 1) & 2) | ((p >> 2) & 4);
                int pf2 = ((p >> 1) & 1) | ((p >> 2) & 2) | ((p >> 3) & 4);
                int pf2pri = (con2 & 0x40) != 0;
                if (pf2pri) index = pf2 ? pf2 + 8 : pf1;
                else index = pf1 ? pf1 : (pf2 ? pf2 + 8 : 0);
            } else {
                index = p & 31;
            }
            row[sx] = colors[index] & 0xFFF;
            if (indices) indices[sx] = (uint8_t)index;
        }
    }
}
