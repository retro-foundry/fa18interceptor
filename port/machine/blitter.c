/* Synchronous OCS blitter. The whole blit runs when BLTSIZE is written; the
 * result, pointer registers, BZERO and the BLIT interrupt match the order of
 * UAE's non-cycle-exact paths (blitter_dofast, blitter_dofast_desc and the
 * line-mode loop of actually_do_blit). */
#include "machine.h"

#include "recomp_runtime.h"

void fa18_blitter_zero_flag(int zero);
void fa18_blitter_busy(FA18Machine *m, int cycles);

/* Colour clocks per word by channel mask ABCD (UAE blit_cycle_diagram and
 * blit_cycle_diagram_fill). */
static const uint8_t cycles_per_word[16] = {2, 2, 2, 3, 3, 3, 3, 4, 2, 2, 2, 3, 3, 3, 3, 4};
static const uint8_t fill_cycles_per_word[16] = {2, 3, 2, 3, 3, 4, 3, 4, 2, 3, 2, 3, 3, 4, 3, 4};

enum {
    CON1_LINE = 0x0001, CON1_DESC = 0x0002, CON1_FCI = 0x0004, CON1_IFE = 0x0008,
    CON1_EFE = 0x0010, CON1_SING = 0x0002, CON1_AUL = 0x0004, CON1_SUL = 0x0008,
    CON1_SUD = 0x0010, CON1_SIGN = 0x0040,
    CON0_USEA = 0x0800, CON0_USEB = 0x0400, CON0_USEC = 0x0200, CON0_USED = 0x0100
};

#define R(off) (m->custom[(off) >> 1])

static uint32_t reg_ptr(const FA18Machine *m, uint32_t off) {
    return ((uint32_t)m->custom[off >> 1] << 16 | m->custom[(off >> 1) + 1]) & 0x7FFFE;
}

static void set_ptr(FA18Machine *m, uint32_t off, uint32_t value) {
    value &= 0x7FFFE;
    m->custom[off >> 1] = (uint16_t)(value >> 16);
    m->custom[(off >> 1) + 1] = (uint16_t)value;
}

static uint16_t minterm(uint16_t a, uint16_t b, uint16_t c, uint8_t mt) {
    uint16_t d = 0;
    if (mt & 0x80) d |= (uint16_t)(a & b & c);
    if (mt & 0x40) d |= (uint16_t)(a & b & ~c);
    if (mt & 0x20) d |= (uint16_t)(a & ~b & c);
    if (mt & 0x10) d |= (uint16_t)(a & ~b & ~c);
    if (mt & 0x08) d |= (uint16_t)(~a & b & c);
    if (mt & 0x04) d |= (uint16_t)(~a & b & ~c);
    if (mt & 0x02) d |= (uint16_t)(~a & ~b & c);
    if (mt & 0x01) d |= (uint16_t)(~a & ~b & ~c);
    return d;
}

static uint8_t fill_byte(uint8_t data, int inclusive, int *fc) {
    unsigned mask;
    for (mask = 1; mask != 0x100; mask <<= 1) {
        unsigned original = data;
        if (*fc) {
            if (inclusive) data |= (uint8_t)mask;
            else data ^= (uint8_t)mask;
        }
        if (original & mask) *fc = !*fc;
    }
    return data;
}

static uint16_t chip_read(FA18Machine *m, uint32_t a) { return fa18_chip16(m, a); }

static void chip_write(FA18Machine *m, uint32_t a, uint16_t v) {
    fa18_chip16_set(m, a, v);
    fa18_recomp_note_write(a & (FA18_CHIP_SIZE - 2), 2);
}

/* Persistent blitter data registers across blits (UAE blt_info). */
static uint16_t bltaold, bltbold, bltbhold, bltddat;

static int area_blit(FA18Machine *m, int width, int height) {
    uint16_t con0 = R(0x040), con1 = R(0x042);
    uint8_t mt = (uint8_t)con0;
    int desc = (con1 & CON1_DESC) != 0;
    int fill = (con1 & (CON1_IFE | CON1_EFE)) != 0;
    int inclusive = (con1 & CON1_IFE) != 0;
    int ash = desc ? 16 - (con0 >> 12) : con0 >> 12;
    int bsh = desc ? 16 - (con1 >> 12) : con1 >> 12;
    int step = desc ? -2 : 2;
    int32_t amod = (int16_t)R(0x064), bmod = (int16_t)R(0x062), cmod = (int16_t)R(0x060),
            dmod = (int16_t)R(0x066);
    uint32_t apt = reg_ptr(m, 0x050), bpt = reg_ptr(m, 0x04C), cpt = reg_ptr(m, 0x048),
             dpt = reg_ptr(m, 0x054);
    uint32_t ap = apt, bp = bpt, cp = cpt, dp = dpt, dstp = 0;
    int dodst = 0, zero = 1, x, y;
    uint32_t bhold = bltbhold;
    int32_t span;

    bltaold = 0;
    bltbold = 0;
    for (y = 0; y < height; y++) {
        int fc = (con1 & CON1_FCI) != 0;
        for (x = 0; x < width; x++) {
            uint16_t adat, mask = 0xFFFF;
            uint32_t ahold;
            if (x == 0) mask &= R(0x044);
            if (x == width - 1) mask &= R(0x046);
            if (con0 & CON0_USEA) {
                adat = chip_read(m, ap);
                R(0x074) = adat;
                ap += (uint32_t)step;
            } else {
                adat = R(0x074);
            }
            adat &= mask;
            if (desc) ahold = (((uint32_t)adat << 16) | bltaold) >> ash;
            else ahold = (((uint32_t)bltaold << 16) | adat) >> ash;
            bltaold = adat;
            if (con0 & CON0_USEB) {
                uint16_t bdat = chip_read(m, bp);
                bp += (uint32_t)step;
                if (desc) bhold = (((uint32_t)bdat << 16) | bltbold) >> bsh;
                else bhold = (((uint32_t)bltbold << 16) | bdat) >> bsh;
                bltbold = bdat;
                R(0x072) = bdat;
            }
            if (con0 & CON0_USEC) {
                R(0x070) = chip_read(m, cp);
                if (desc) R(0x072) = R(0x070);
                cp += (uint32_t)step;
            }
            if (dodst) chip_write(m, dstp, bltddat);
            bltddat = minterm((uint16_t)ahold, (uint16_t)bhold, R(0x070), mt);
            if (fill) {
                uint16_t lo = fill_byte((uint8_t)bltddat, inclusive, &fc);
                uint16_t hi = fill_byte((uint8_t)(bltddat >> 8), inclusive, &fc);
                bltddat = (uint16_t)(hi << 8 | lo);
            }
            if (bltddat) zero = 0;
            if (con0 & CON0_USED) {
                dodst = 1;
                dstp = dp;
                dp += (uint32_t)step;
            }
        }
        if (desc) {
            ap -= (uint32_t)amod; bp -= (uint32_t)bmod; cp -= (uint32_t)cmod; dp -= (uint32_t)dmod;
        } else {
            ap += (uint32_t)amod; bp += (uint32_t)bmod; cp += (uint32_t)cmod; dp += (uint32_t)dmod;
        }
    }
    if (dodst) chip_write(m, dstp, bltddat);
    bltbhold = (uint16_t)bhold;

    span = desc ? -1 : 1;
    if (con0 & CON0_USEA) set_ptr(m, 0x050, apt + (uint32_t)(span * (width * 2 + amod) * height));
    if (con0 & CON0_USEB) set_ptr(m, 0x04C, bpt + (uint32_t)(span * (width * 2 + bmod) * height));
    if (con0 & CON0_USEC) set_ptr(m, 0x048, cpt + (uint32_t)(span * (width * 2 + cmod) * height));
    if (con0 & CON0_USED) set_ptr(m, 0x054, dpt + (uint32_t)(span * (width * 2 + dmod) * height));
    return zero;
}

static int line_blit(FA18Machine *m, int width, int height) {
    uint16_t con0 = R(0x040), con1 = R(0x042);
    uint32_t cpt = reg_ptr(m, 0x048), dpt = reg_ptr(m, 0x054);
    uint32_t apt = (uint32_t)m->custom[0x052 >> 1] | (uint32_t)m->custom[0x050 >> 1] << 16;
    int16_t amod = (int16_t)R(0x064), bmod = (int16_t)R(0x062), cmod = (int16_t)R(0x060);
    uint32_t bpt = reg_ptr(m, 0x04C);
    int onedot = 0, linepixel = 0, loop = 1, ovf = 0, zero = 1;
    int bshift = con1 >> 12;
    uint16_t bdat = R(0x072);
    uint16_t blineb = (uint16_t)((bdat >> bshift) | (bdat << (16 - bshift)));
    uint16_t bhold = bltbhold;

#define ASH() (con0 >> 12)
#define MINTERM(maskv)                                                                   \
    do {                                                                                 \
        uint16_t ahold_ = (uint16_t)((R(0x074) & (maskv)) >> ASH());                     \
        if (con0 & CON0_USEB) {                                                          \
            int bs_ = con1 >> 12;                                                        \
            blineb = (uint16_t)((((uint32_t)bltbold << 16) | R(0x072)) >> bs_);          \
        }                                                                                \
        bhold = (blineb & 1) ? 0xFFFF : 0;                                               \
        bltddat = minterm(ahold_, bhold, R(0x070), (uint8_t)con0);                       \
        if (bltddat) zero = 0;                                                           \
    } while (0)
#define INCX() do { if (ASH() == 15) cpt += 2; ovf = 1; } while (0)
#define DECX() do { if (ASH() == 0) cpt -= 2; ovf = -1; } while (0)
#define INCY() do { if (con0 & CON0_USEC) { cpt += (uint32_t)cmod; onedot = 0; } } while (0)
#define DECY() do { if (con0 & CON0_USEC) { cpt -= (uint32_t)cmod; onedot = 0; } } while (0)
#define CPT_Y()                                                                          \
    do {                                                                                 \
        int sign_ = (con1 & CON1_SIGN) != 0;                                             \
        if (!sign_ && (con1 & CON1_SUD)) { if (con1 & CON1_SUL) DECY(); else INCY(); }   \
        if (!(con1 & CON1_SUD)) { if (con1 & CON1_AUL) DECY(); else INCY(); }            \
    } while (0)
#define CPT_X()                                                                          \
    do {                                                                                 \
        int sign_ = (con1 & CON1_SIGN) != 0;                                             \
        if (!sign_ && !(con1 & CON1_SUD)) { if (con1 & CON1_SUL) DECX(); else INCX(); }  \
        if (con1 & CON1_SUD) { if (con1 & CON1_AUL) DECX(); else INCX(); }               \
    } while (0)
#define READ_C() do { if (con0 & CON0_USEC) R(0x070) = chip_read(m, cpt); } while (0)

    bltaold = 0;
    bltbold = 0;
    do {
        linepixel = !(con1 & CON1_SING) || !onedot;
        onedot = 1;
        if (con0 & CON0_USEA) apt += (uint32_t)(int32_t)((con1 & CON1_SIGN) ? bmod : amod);
        if (width > 1) {
            if (con0 & CON0_USEB) {
                R(0x072) = chip_read(m, bpt);
                bpt += (uint32_t)bmod;
            }
            READ_C();
            MINTERM(R(0x044));
            CPT_X();
        }
        if (width > 2) {
            if (loop && !(con1 & CON1_SUD)) {
                CPT_Y();
                loop = 0;
            }
            READ_C();
            bltddat = minterm((uint16_t)(R(0x074) >> ASH()), bhold, R(0x070), (uint8_t)con0);
        }
        bltaold = (uint16_t)((((uint32_t)bltaold << 16) | (R(0x074) & R(0x044))) >> ASH());
        {
            int ash = (ASH() + ovf) & 15;
            con0 = (uint16_t)((con0 & 0x0FFF) | ash << 12);
            ovf = 0;
        }
        if (width >= 2 && loop) {
            CPT_Y();
            loop = 0;
        }
        if ((int16_t)apt < 0) con1 |= CON1_SIGN;
        else con1 &= (uint16_t)~CON1_SIGN;
        {
            int bs = ((con1 >> 12) - 1) & 15;
            blineb = (uint16_t)((R(0x072) >> bs) | (R(0x072) << (16 - bs)));
            con1 = (uint16_t)((con1 & 0x0FFF) | bs << 12);
        }
        if (linepixel) {
            if (con0 & CON0_USEC) chip_write(m, dpt, bltddat);
            linepixel = 0;
        }
        dpt = cpt;
        MINTERM((uint16_t)(R(0x044) & R(0x046)));
        loop = 1;
    } while (--height > 0);

#undef ASH
#undef MINTERM
#undef INCX
#undef DECX
#undef INCY
#undef DECY
#undef CPT_Y
#undef CPT_X
#undef READ_C

    bltbhold = bhold;
    R(0x040) = con0;
    R(0x042) = con1;
    m->custom[0x050 >> 1] = (uint16_t)(apt >> 16);
    m->custom[0x052 >> 1] = (uint16_t)apt;
    set_ptr(m, 0x048, cpt);
    set_ptr(m, 0x054, dpt);
    if (con0 & CON0_USEB) set_ptr(m, 0x04C, bpt);
    return zero;
}

void fa18_blitter_start(FA18Machine *m) {
    uint16_t size = R(0x058);
    int height = size >> 6, width = size & 63, zero, ccks;
    int channels = (R(0x040) >> 8) & 15;
    if (!height) height = 1024;
    if (!width) width = 64;
    if (R(0x042) & CON1_LINE) {
        ccks = 4 * height + 2;
    } else {
        int fill = (R(0x042) & (CON1_IFE | CON1_EFE)) != 0;
        ccks = (fill ? fill_cycles_per_word : cycles_per_word)[channels] * width * height + 2;
    }
    if (R(0x042) & CON1_LINE) {
        zero = line_blit(m, width, height);
        m->line_blits++;
    } else {
        zero = area_blit(m, width, height);
    }
    m->blits++;
    fa18_blitter_zero_flag(zero);
    /* Memory is updated now; BBUSY and the BLIT interrupt follow the
     * hardware duration so CPU wait loops keep their timing. */
    fa18_blitter_busy(m, ccks * 2);
}
