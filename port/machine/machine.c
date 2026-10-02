#include "machine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bus.h"
#include "m68kcpu.h"
#include "recomp_runtime.h"

FA18Machine *fa18_machine;

extern int fa18_write_log_active, fa18_write_log_hardware;
void fa18_write_log_before(uint32_t address, int size);
#define LOG_WRITE(a, n) do { if (fa18_write_log_active) fa18_write_log_before((a), (n)); } while (0)
/* Sandboxed (1): hardware is left alone and the call marked; live (2): it
 * is used, and the call marked. */
#define HARDWARE_BLOCKED() (fa18_write_log_active ? (fa18_write_log_hardware = 1, fa18_write_log_active == 1) : 0)
void fa18_write_log_custom(uint32_t reg, uint16_t value);
uint16_t fa18_shadow_dmaconr(uint16_t value);
uint16_t fa18_shadow_mouse(uint32_t address,uint16_t value);
int fa18_shadow_mouse_enabled(void);
int fa18_shadow_inputs_replaying(void);
#define CUSTOM_LOGGED(reg, v)     (fa18_write_log_active ? (fa18_write_log_custom((reg), (v)), fa18_write_log_active == 1) : 0)
/* Taking an interrupt reads its autovector ($64-$7C). */
#define VECTOR_READ(a) do { if (fa18_write_log_active == 2 && (a) >= 0x60 && (a) < 0x80) fa18_write_log_hardware = 1; } while (0)

/* One timeline for interpreter and generated code. While Musashi runs, the
 * current CPU cycle is fa18_cycle_origin - GET_CYCLES(). Chipset work (line
 * ends, Copper, display, CIAs, interrupt acceptance) happens only at
 * instruction boundaries, in fa18_machine_service(), once the cycle reaches
 * fa18_next_event. The interpreter reaches it through the instruction hook;
 * generated code checks the same deadline before every instruction. */
int64_t fa18_cycle_origin;
int64_t fa18_next_event;
static int64_t line_start;
static int64_t last_boundary;
static int frame_done;
static int line_started;
static int in_execute;
static int64_t blit_end;
static int blit_pending;

static int64_t now_cycle(void) {
    return in_execute ? fa18_cycle_origin - GET_CYCLES() : fa18_machine->cycle;
}

int64_t fa18_machine_now(void) { return now_cycle(); }

void fa18_machine_beam(int *vpos, int *hpos) {
    FA18Machine *m = fa18_machine;
    int64_t h = (fa18_bus_now() - line_start) / 2;
    if (h < 0) h = 0;
    if (h >= FA18_LINE_CCKS) h = FA18_LINE_CCKS - 1;
    *vpos = m->vpos;
    *hpos = (int)h;
}

/* ---- interrupts ---------------------------------------------------------- */

static int interrupt_level(const FA18Machine *m) {
    static const int level[14] = {1, 1, 1, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 6};
    uint16_t pending;
    int bit;
    if (!(m->intena & 0x4000)) return 0;
    pending = (uint16_t)(m->intena & m->intreq & 0x3FFF);
    for (bit = 13; bit >= 0; bit--) {
        if (pending & (1u << bit)) {
            int best = level[bit], other;
            for (other = bit - 1; other >= 0; other--)
                if ((pending & (1u << other)) && level[other] > best) best = level[other];
            return best;
        }
    }
    return 0;
}

static void update_irq(FA18Machine *m) {
    m68k_set_irq((unsigned)interrupt_level(m));
    fa18_next_event = 0; /* accept at the next instruction boundary */
}

void fa18_raise_interrupt(FA18Machine *m, int bit) {
    m->intreq |= (uint16_t)(1u << bit);
    update_irq(m);
}

/* ---- CIAs ---------------------------------------------------------------- */

static void cia_interrupt(FA18Machine *m, int which, uint8_t bit) {
    FA18Cia *c = &m->cia[which];
    c->icr |= bit;
    if (c->imask & bit) fa18_raise_interrupt(m, which == 0 ? 3 : 13);
}

static void cia_tick_timers(FA18Machine *m, int which, int eclocks) {
    FA18Cia *c = &m->cia[which];
    int n;
    for (n = 0; n < eclocks; n++) {
        int a_underflow = 0;
        if (c->cra & 1) {
            if (c->ta == 0) {
                a_underflow = 1;
                c->ta = c->ta_latch;
                if (c->cra & 8) c->cra &= (uint8_t)~1;
                cia_interrupt(m, which, 1);
            } else {
                c->ta--;
            }
        }
        if (c->crb & 1) {
            int inmode = (c->crb >> 5) & 3;
            int count = inmode == 0 || (inmode == 2 && a_underflow);
            if (count) {
                if (c->tb == 0) {
                    c->tb = c->tb_latch;
                    if (c->crb & 8) c->crb &= (uint8_t)~1;
                    cia_interrupt(m, which, 2);
                } else {
                    c->tb--;
                }
            }
        }
    }
}

static void cia_tod_tick(FA18Machine *m, int which) {
    FA18Cia *c = &m->cia[which];
    if (c->tod_stopped) return;
    c->tod = (c->tod + 1) & 0xFFFFFF;
    if (c->tod == c->tod_alarm) cia_interrupt(m, which, 4);
}

static uint8_t cia_read(FA18Machine *m, int which, int reg) {
    FA18Cia *c = &m->cia[which];
    uint8_t v;
    switch (reg) {
    case 0:
        if (which == 0) {
            /* /FIR1 joystick fire, /FIR0 left mouse; floppy status lines high. */
            uint8_t inputs = (uint8_t)(0x3C | (m->joy_fire ? 0 : 0x80) | (m->mouse_left ? 0 : 0x40));
            return (uint8_t)((c->pra & c->ddra) | (inputs & ~c->ddra));
        }
        return (uint8_t)((c->pra & c->ddra) | (0xFF & ~c->ddra));
    case 1: return (uint8_t)((c->prb & c->ddrb) | (0xFF & ~c->ddrb));
    case 2: return c->ddra;
    case 3: return c->ddrb;
    case 4: return (uint8_t)c->ta;
    case 5: return (uint8_t)(c->ta >> 8);
    case 6: return (uint8_t)c->tb;
    case 7: return (uint8_t)(c->tb >> 8);
    case 8:
        v = (uint8_t)((c->tod_latched ? c->tod_latch : c->tod));
        c->tod_latched = 0;
        return v;
    case 9: return (uint8_t)((c->tod_latched ? c->tod_latch : c->tod) >> 8);
    case 10:
        c->tod_latch = c->tod;
        c->tod_latched = 1;
        return (uint8_t)(c->tod >> 16);
    case 12: return c->sdr;
    case 13:
        v = c->icr;
        if (v & c->imask) v |= 0x80;
        c->icr = 0;
        return v;
    case 14: return c->cra;
    case 15: return c->crb;
    default: return 0xFF;
    }
}

static void cia_write(FA18Machine *m, int which, int reg, uint8_t v) {
    FA18Cia *c = &m->cia[which];
    switch (reg) {
    case 0: c->pra = v; break;
    case 1: c->prb = v; break;
    case 2: c->ddra = v; break;
    case 3: c->ddrb = v; break;
    case 4: c->ta_latch = (uint16_t)((c->ta_latch & 0xFF00) | v); break;
    case 5:
        c->ta_latch = (uint16_t)((c->ta_latch & 0x00FF) | v << 8);
        if (!(c->cra & 1)) c->ta = c->ta_latch;
        if (c->cra & 8) { c->ta = c->ta_latch; c->cra |= 1; }
        break;
    case 6: c->tb_latch = (uint16_t)((c->tb_latch & 0xFF00) | v); break;
    case 7:
        c->tb_latch = (uint16_t)((c->tb_latch & 0x00FF) | v << 8);
        if (!(c->crb & 1)) c->tb = c->tb_latch;
        if (c->crb & 8) { c->tb = c->tb_latch; c->crb |= 1; }
        break;
    case 8:
        if (c->crb & 0x80) c->tod_alarm = (c->tod_alarm & 0xFFFF00) | v;
        else { c->tod = (c->tod & 0xFFFF00) | v; c->tod_stopped = 0; }
        break;
    case 9:
        if (c->crb & 0x80) c->tod_alarm = (c->tod_alarm & 0xFF00FF) | (uint32_t)v << 8;
        else c->tod = (c->tod & 0xFF00FF) | (uint32_t)v << 8;
        break;
    case 10:
        if (c->crb & 0x80) c->tod_alarm = (c->tod_alarm & 0x00FFFF) | (uint32_t)v << 16;
        else { c->tod = (c->tod & 0x00FFFF) | (uint32_t)v << 16; c->tod_stopped = 1; }
        break;
    case 12: c->sdr = v; break;
    case 13:
        if (v & 0x80) c->imask |= (uint8_t)(v & 0x7F);
        else c->imask &= (uint8_t)~v;
        if (c->icr & c->imask) fa18_raise_interrupt(m, which == 0 ? 3 : 13);
        break;
    case 14:
        if (v & 0x10) c->ta = c->ta_latch;
        c->cra = (uint8_t)(v & ~0x10);
        break;
    case 15:
        if (v & 0x10) c->tb = c->tb_latch;
        c->crb = (uint8_t)(v & ~0x10);
        break;
    default: break;
    }
}

/* ---- custom registers ---------------------------------------------------- */

static uint32_t custom_long(const FA18Machine *m, uint32_t reg) {
    return ((uint32_t)m->custom[reg >> 1] << 16 | m->custom[(reg >> 1) + 1]) & 0x7FFFE;
}

void fa18_blitter_busy(FA18Machine *m, const uint8_t *diagram, int steps_per_word, int64_t words);
void fa18_blitter_busy(FA18Machine *m, const uint8_t *diagram, int steps_per_word, int64_t words) {
    (void)m;
    blit_end = fa18_bus_blit(fa18_bus_now(), diagram, steps_per_word, words);
    if (getenv("FA18_BLIT_LOG")) {
        int v, h;
        fa18_machine_beam(&v, &h);
        fprintf(stderr, "BLIT v=%d h=%d con0=%04X con1=%04X size=%04X steps=%d words=%lld ccks=%lld\n", v, h,
                m->custom[0x040 >> 1], m->custom[0x042 >> 1], m->custom[0x058 >> 1], steps_per_word,
                (long long)words, (long long)((blit_end - fa18_bus_now()) / 2));
    }
    blit_pending = 1;
    if (blit_end < fa18_next_event) fa18_next_event = blit_end;
}

static int blit_zero = 1;

/* The CPU took a cycle from the running blit (bus.c). */
void fa18_blitter_delayed(int cycles);
void fa18_blitter_delayed(int cycles) { blit_end += cycles; }

void fa18_blitter_zero_flag(int zero);
void fa18_blitter_zero_flag(int zero) { blit_zero = zero; }

/* ---- mouse (UAE inputdevice.c) ------------------------------------------ */

/* Recorded mouse motion does not reach the counters at once. It is held as a
 * pending delta and released when the game reads JOYxDAT, in proportion to
 * the lines since the previous read (at least one count), and by one count
 * at each vertical blank: UAE's readinput/mouseupdate/getvelocity. */
static int64_t total_lines, last_input_line;

static int mouse_velocity(int *delta, int pct) {
    int value = *delta, v;
    if (pct > 1000) pct = 1000;
    if (pct < 0) pct = 0;
    v = value * pct / 1000;
    if (!v) {
        if (value < -FA18_PAL_LINES / 2) v = -2;
        else if (value < 0) v = -1;
        else if (value > FA18_PAL_LINES / 2) v = 2;
        else if (value > 0) v = 1;
    }
    *delta -= v;
    return v;
}

static void mouse_update(FA18Machine *m, int pct) {
    m->mouse_x = (m->mouse_x + mouse_velocity(&m->mouse_dx, pct)) & 0xFF;
    m->mouse_y = (m->mouse_y + mouse_velocity(&m->mouse_dy, pct)) & 0xFF;
    m->joy0dat = (uint16_t)(m->mouse_y << 8 | m->mouse_x);
}

static void read_input(FA18Machine *m) {
    int v, h;
    int64_t line, diff;
    fa18_machine_beam(&v, &h);
    line = total_lines + (v - m->vpos); /* the beam may already be past a line end */
    diff = line - last_input_line;
    if (diff > 0) mouse_update(m, diff < 10 ? 0 : (int)(diff * 1000 / FA18_PAL_LINES));
    last_input_line = line;
}

uint16_t fa18_custom_read(FA18Machine *m, uint32_t reg) {
    int v, h;
    reg &= 0x1FE;
    switch (reg) {
    case 0x002:
        return fa18_shadow_dmaconr((uint16_t)((m->dmacon & 0x07FF) | (blit_zero ? 0x2000 : 0) |
                          (blit_pending && fa18_bus_now() < blit_end ? 0x4000 : 0)));
    case 0x004:
        fa18_machine_beam(&v, &h);
        return (uint16_t)(0x8000 | ((v >> 8) & 1));
    case 0x006:
        fa18_machine_beam(&v, &h);
        return (uint16_t)((v & 0xFF) << 8 | (h & 0xFF));
    case 0x00A:
    case 0x00C:
        /* Reading moves the mouse counters: not repeatable inside a shadow
         * comparison. */
        if(fa18_shadow_mouse_enabled()) {
            if(!fa18_shadow_inputs_replaying()) read_input(m);
        } else if (!HARDWARE_BLOCKED()) read_input(m);
        return fa18_shadow_mouse(0xdff000u+reg,reg == 0x00A ? m->joy0dat : m->joy1dat);
    case 0x010: return m->adkcon;
    case 0x012: case 0x014: return 0;
    case 0x016: /* DATLY/DATLX/DATRY/DATRX; DATLY low = right mouse button */
        return fa18_shadow_mouse(0xdff016u,(uint16_t)(m->mouse_right ? 0x5100 : 0x5500));
    case 0x018: return 0x3000;
    case 0x01A: return 0;
    case 0x01C: return m->intena;
    case 0x01E: return m->intreq;
    case 0x07C: return 0xFFFF; /* OCS Denise has no ID register */
    default: return 0xFFFF;
    }
}

void fa18_custom_write(FA18Machine *m, uint32_t reg, uint16_t value) {
    reg &= 0x1FE;
    m->custom[reg >> 1] = value;
    switch (reg) {
    case 0x058: fa18_blitter_start(m); break;
    case 0x080: case 0x082: m->cop1lc = custom_long(m, 0x080); break;
    case 0x084: case 0x086: m->cop2lc = custom_long(m, 0x084); break;
    case 0x088:
        m->copper_pc = m->cop1lc;
        m->copper_waiting = 0;
        break;
    case 0x08A:
        m->copper_pc = m->cop2lc;
        m->copper_waiting = 0;
        break;
    case 0x02E: m->copper_danger = value & 2; break;
    case 0x096:
        if (value & 0x8000) m->dmacon |= (uint16_t)(value & 0x07FF);
        else m->dmacon &= (uint16_t)~(value & 0x07FF);
        break;
    case 0x09A:
        if (value & 0x8000) m->intena |= (uint16_t)(value & 0x7FFF);
        else m->intena &= (uint16_t)~(value & 0x7FFF);
        update_irq(m);
        break;
    case 0x09C:
        if (value & 0x8000) m->intreq |= (uint16_t)(value & 0x7FFF);
        else m->intreq &= (uint16_t)~(value & 0x7FFF);
        update_irq(m);
        break;
    case 0x09E:
        if (value & 0x8000) m->adkcon |= (uint16_t)(value & 0x7FFF);
        else m->adkcon &= (uint16_t)~(value & 0x7FFF);
        break;
    default:
        if (reg >= 0x0E0 && reg < 0x0F8) {
            int plane = (int)(reg - 0x0E0) >> 2;
            m->bplpt[plane] = custom_long(m, 0x0E0 + (uint32_t)plane * 4);
        }
        break;
    }
}

/* ---- bus ----------------------------------------------------------------- */

static int is_custom(uint32_t a) { return a >= 0xDFF000 && a < 0xDFF200; }
static int is_cia(uint32_t a) { return (a & 0xFF0000) == 0xBF0000; }

uint8_t fa18_bus_read8(uint32_t a) {
    FA18Machine *m = fa18_machine;
    a &= 0xFFFFFF;
    if (a < 0x200000) {
        VECTOR_READ(a);
        return m->chip[a & (FA18_CHIP_SIZE - 1)];
    }
    if (a >= FA18_SLOW_BASE && a < FA18_SLOW_BASE + FA18_SLOW_SIZE) return m->slow[a - FA18_SLOW_BASE];
    if (a >= 0xF80000) return m->rom[a & (FA18_ROM_SIZE - 1)];
    if ((a & 0xFF0000) == 0xF00000) return m->rtarea[a & 0xFFFF];
    if (is_cia(a)) {
        uint8_t v = 0xFF;
        if (HARDWARE_BLOCKED()) return v; /* CIA reads have side effects */
        if (!(a & 0x1000) && (a & 1)) v = cia_read(m, 0, (int)(a >> 8) & 15);
        if (!(a & 0x2000) && !(a & 1)) v = cia_read(m, 1, (int)(a >> 8) & 15);
        return v;
    }
    if (is_custom(a)) {
        uint16_t w = fa18_custom_read(m, a & 0x1FE);
        return (uint8_t)((a & 1) ? w : w >> 8);
    }
    m->unmapped_reads++;
    return 0;
}

uint16_t fa18_bus_read16(uint32_t a) {
    FA18Machine *m = fa18_machine;
    a &= 0xFFFFFF;
    if (a < 0x200000) {
        a &= FA18_CHIP_SIZE - 1;
        VECTOR_READ(a);
        return (uint16_t)(m->chip[a] << 8 | m->chip[(a + 1) & (FA18_CHIP_SIZE - 1)]);
    }
    if (a >= FA18_SLOW_BASE && a + 1 < FA18_SLOW_BASE + FA18_SLOW_SIZE) {
        const uint8_t *p = m->slow + (a - FA18_SLOW_BASE);
        return (uint16_t)(p[0] << 8 | p[1]);
    }
    if (a >= 0xF80000) {
        const uint8_t *p = m->rom + (a & (FA18_ROM_SIZE - 1));
        return (uint16_t)(p[0] << 8 | p[1]);
    }
    if (is_custom(a)) return fa18_custom_read(m, a & 0x1FE);
    return (uint16_t)(fa18_bus_read8(a) << 8 | fa18_bus_read8(a + 1));
}

uint32_t fa18_bus_read32(uint32_t a) {
    return (uint32_t)fa18_bus_read16(a) << 16 | fa18_bus_read16(a + 2);
}

/* The BLTSIZE the program last wrote, and its value when a polygon draw
 * last began (DMACON $8400, blitter priority on), for glue that rebuilds a
 * draw's registers after several draws. Kept from the writes as issued,
 * whether or not a shadow comparison holds them back. */
uint16_t fa18_bltsize_at_draw_start;
static uint16_t bltsize_issued;

static void note_draw_start(uint32_t reg, uint16_t v) {
    if (reg == 0x058) bltsize_issued = v;
    else if (reg == 0x096 && v == 0x8400) fa18_bltsize_at_draw_start = bltsize_issued;
}

/* FA18_WATCH=lo-hi (hex): log CPU writes into that range (debugging). */
static void watch_write(uint32_t a, uint32_t v, int size) {
    static int init;
    static uint32_t lo = 1, hi = 0;
    if (!init) {
        const char *env = getenv("FA18_WATCH");
        init = 1;
        if (env) sscanf(env, "%x-%x", &lo, &hi);
    }
    if (a + (uint32_t)size > lo && a < hi) {
        int vp, h;
        fa18_machine_beam(&vp, &h);
        fprintf(stderr, "WATCH frame=%llu v=%d h=%d pc=%06X %06X <- %0*X\n", (unsigned long long)fa18_machine->frame,
                vp, h, (unsigned)m68k_get_reg(NULL, M68K_REG_PPC), a, size * 2, v);
    }
}

void fa18_bus_write8(uint32_t a, uint8_t v) {
    FA18Machine *m = fa18_machine;
    a &= 0xFFFFFF;
    watch_write(a, v, 1);
    if (a < 0x200000) {
        a &= FA18_CHIP_SIZE - 1;
        LOG_WRITE(a, 1);
        m->chip[a] = v;
        fa18_recomp_note_write(a, 1);
        return;
    }
    if (a >= FA18_SLOW_BASE && a < FA18_SLOW_BASE + FA18_SLOW_SIZE) {
        LOG_WRITE(a, 1);
        m->slow[a - FA18_SLOW_BASE] = v;
        fa18_recomp_note_write(a, 1);
        return;
    }
    if ((a & 0xFF0000) == 0xF00000) {
        LOG_WRITE(a, 1);
        m->rtarea[a & 0xFFFF] = v;
        return;
    }
    if (is_cia(a)) {
        if (HARDWARE_BLOCKED()) return;
        if (!(a & 0x1000) && (a & 1)) cia_write(m, 0, (int)(a >> 8) & 15, v);
        if (!(a & 0x2000) && !(a & 1)) cia_write(m, 1, (int)(a >> 8) & 15, v);
        return;
    }
    if (is_custom(a)) {
        if (CUSTOM_LOGGED(a & 0x1FE, (uint16_t)(v << 8 | v))) return;
        fa18_custom_write(m, a & 0x1FE, (uint16_t)(v << 8 | v));
        return;
    }
    if (a < 0xF80000) m->unmapped_writes++;
}

void fa18_bus_write16(uint32_t a, uint16_t v) {
    FA18Machine *m = fa18_machine;
    a &= 0xFFFFFF;
    watch_write(a, v, 2);
    if (a < 0x200000) {
        a &= FA18_CHIP_SIZE - 1;
        LOG_WRITE(a, 2);
        m->chip[a] = (uint8_t)(v >> 8);
        m->chip[(a + 1) & (FA18_CHIP_SIZE - 1)] = (uint8_t)v;
        fa18_recomp_note_write(a, 2);
        return;
    }
    if (a >= FA18_SLOW_BASE && a + 1 < FA18_SLOW_BASE + FA18_SLOW_SIZE) {
        uint8_t *p = m->slow + (a - FA18_SLOW_BASE);
        LOG_WRITE(a, 2);
        p[0] = (uint8_t)(v >> 8);
        p[1] = (uint8_t)v;
        fa18_recomp_note_write(a, 2);
        return;
    }
    if (is_custom(a)) {
        note_draw_start(a & 0x1FE, v);
        if (CUSTOM_LOGGED(a & 0x1FE, v)) return;
        fa18_custom_write(m, a & 0x1FE, v);
        return;
    }
    fa18_bus_write8(a, (uint8_t)(v >> 8));
    fa18_bus_write8(a + 1, (uint8_t)v);
}

void fa18_bus_write32(uint32_t a, uint32_t v) {
    fa18_bus_write16(a, (uint16_t)(v >> 16));
    fa18_bus_write16(a + 2, (uint16_t)v);
}

/* Musashi memory interface: CPU accesses, timed by the bus (bus.c). A long
 * access is two word accesses. */
static void cpu_words(uint32_t a, int words) {
    fa18_bus_access(a);
    if (words > 1) fa18_bus_access(a + 2);
}
unsigned int m68k_read_memory_8(unsigned int a) { cpu_words(a, 1); return fa18_bus_read8(a); }
unsigned int m68k_read_memory_16(unsigned int a) { cpu_words(a, 1); return fa18_bus_read16(a); }
unsigned int m68k_read_memory_32(unsigned int a) { cpu_words(a, 2); return fa18_bus_read32(a); }
void m68k_write_memory_8(unsigned int a, unsigned int v) { cpu_words(a, 1); fa18_bus_write8(a, (uint8_t)v); }
void m68k_write_memory_16(unsigned int a, unsigned int v) { cpu_words(a, 1); fa18_bus_write16(a, (uint16_t)v); }
void m68k_write_memory_32(unsigned int a, unsigned int v) { cpu_words(a, 2); fa18_bus_write32(a, v); }
unsigned int m68k_read_immediate_16(unsigned int a) { fa18_bus_fetch(a); return fa18_bus_read16(a); }
unsigned int m68k_read_immediate_32(unsigned int a) {
    fa18_bus_fetch(a);
    fa18_bus_fetch(a + 2);
    return fa18_bus_read32(a);
}
unsigned int m68k_read_pcrelative_8(unsigned int a) { cpu_words(a, 1); return fa18_bus_read8(a); }
unsigned int m68k_read_pcrelative_16(unsigned int a) { cpu_words(a, 1); return fa18_bus_read16(a); }
unsigned int m68k_read_pcrelative_32(unsigned int a) { cpu_words(a, 2); return fa18_bus_read32(a); }
unsigned int m68k_read_disassembler_16(unsigned int a) { return fa18_bus_read16(a); }
unsigned int m68k_read_disassembler_32(unsigned int a) { return fa18_bus_read32(a); }

/* ---- input --------------------------------------------------------------- */

void fa18_machine_key(FA18Machine *m, int rawkey, int down) {
    int next = (m->keyboard_tail + 1) % (int)sizeof m->keyboard_queue;
    if (next == m->keyboard_head) return;
    m->keyboard_queue[m->keyboard_tail] = (uint8_t)((rawkey & 0x7F) | (down ? 0 : 0x80));
    m->keyboard_tail = next;
}

void fa18_machine_mouse(FA18Machine *m, int dx, int dy) {
    m->mouse_dx += dx;
    m->mouse_dy += dy;
}

void fa18_machine_button(FA18Machine *m, int button, int down) {
    if (button == 0) m->mouse_left = down;
    else if (button == 1) m->mouse_right = down;
    else m->joy_fire = down;
}

void fa18_machine_joystick(FA18Machine *m, int up, int down, int left, int right) {
    /* JOY1DAT: bit 9 left, bit 8 up ^ left, bit 1 right, bit 0 down ^ right. */
    m->joy1dat = (uint16_t)((left ? 0x200 : 0) | ((up ^ left) ? 0x100 : 0) | (right ? 0x002 : 0) |
                            ((down ^ right) ? 0x001 : 0));
}

static void keyboard_line(FA18Machine *m) {
    uint8_t k;
    if (m->keyboard_cooldown > 0) { m->keyboard_cooldown--; return; }
    if (m->keyboard_head == m->keyboard_tail) return;
    k = m->keyboard_queue[m->keyboard_head];
    m->keyboard_head = (m->keyboard_head + 1) % (int)sizeof m->keyboard_queue;
    m->cia[0].sdr = (uint8_t)~((k << 1) | (k >> 7));
    cia_interrupt(m, 0, 8);
    m->keyboard_cooldown = 20;
}

/* ---- frame loop ---------------------------------------------------------- */

static void start_line(FA18Machine *m) {
    if (m->vpos == 0) {
        fa18_copper_restart(m);
        fa18_raise_interrupt(m, 5);
        mouse_update(m, 0); /* UAE inputdevice_vsync */
    }
    /* CIA-A TOD counts the vertical sync pulse, which follows the interrupt
     * by a few lines on PAL. */
    if (m->vpos == 3) cia_tod_tick(m, 0);
    fa18_copper_run_until(m, m->vpos, FA18_LINE_CCKS);
    fa18_display_line(m, m->vpos);
    fa18_bus_line(m, m->vpos, line_start);
    line_started = 1;
}

/* End the current line; start the next unless that completes the frame. */
static void advance_line(FA18Machine *m) {
    int eclocks;
    m->eclock_frac += FA18_LINE_CYCLES;
    eclocks = m->eclock_frac / 10;
    m->eclock_frac %= 10;
    cia_tick_timers(m, 0, eclocks);
    cia_tick_timers(m, 1, eclocks);
    cia_tod_tick(m, 1);
    keyboard_line(m);
    line_start += FA18_LINE_CYCLES;
    total_lines++;
    line_started = 0;
    m->vpos++;
    if (m->vpos >= FA18_PAL_LINES) m->vpos = 0;
    if (m->vpos == FA18_FRAME_END_LINE) {
        memcpy(m->last_screen, m->screen, sizeof m->screen);
        m->frame++;
        frame_done = 1;
        return;
    }
    start_line(m);
}

/* Advance the CPU to the end of the current blit (a BBUSY wait loop). */
void fa18_machine_wait_blitter(void) {
    int64_t now = now_cycle();
    if (blit_pending && now < blit_end) USE_CYCLES((int)(blit_end - now));
}

uint16_t fa18_machine_count_blitter_polls(void) {
    uint16_t polls = 0;
    while (blit_pending && now_cycle() < blit_end) {
        ++polls;
        USE_CYCLES(56);
    }
    return polls;
}

int fa18_machine_event_due(void) {
    return frame_done || fa18_cycle_origin - GET_CYCLES() >= fa18_next_event;
}

/* Called at an instruction boundary inside m68k_execute. Returns 1 when the
 * frame is complete and the slice has been ended. */
int fa18_machine_service(void) {
    FA18Machine *m = fa18_machine;
    int64_t now = fa18_cycle_origin - GET_CYCLES();
    uint32_t source_pc = REG_PC;
    last_boundary = now;
    if (frame_done) return 1;
    if (now < fa18_next_event) return 0;
    fa18_bus_trace_boundary("event_before", source_pc);
    while (now >= line_start + FA18_LINE_CYCLES) {
        advance_line(m);
        if (frame_done) {
            /* Keep the cycle count continuous across the end of the slice. */
            fa18_cycle_origin -= GET_CYCLES();
            SET_CYCLES(0);
            fa18_bus_trace_boundary("frame_end", source_pc);
            return 1;
        }
    }
    if (blit_pending && now >= blit_end) {
        blit_pending = 0;
        fa18_raise_interrupt(m, 6);
    }
    m68ki_check_interrupts();
    fa18_next_event = line_start + FA18_LINE_CYCLES;
    if (blit_pending && blit_end < fa18_next_event) fa18_next_event = blit_end;
    fa18_bus_trace_boundary("event_after", source_pc);
    return 0;
}

#define EXECUTE_BUDGET 0x3FFFFFFF

void fa18_machine_run_frame(FA18Machine *m) {
    frame_done = 0;
    if (!line_started) start_line(m);
    fa18_next_event = 0;
    while (!frame_done) {
        if (CPU_STOPPED) {
            /* STOP: only interrupts wake the CPU; advance whole lines. */
            m->cycle = line_start + FA18_LINE_CYCLES;
            if (blit_pending && blit_end <= m->cycle) {
                blit_pending = 0;
                fa18_raise_interrupt(m, 6);
            }
            advance_line(m);
            if (frame_done) break;
            m68ki_check_interrupts();
            continue;
        }
        fa18_cycle_origin = m->cycle + EXECUTE_BUDGET;
        in_execute = 1;
        m68k_execute(EXECUTE_BUDGET);
        in_execute = 0;
        m->cycle = fa18_cycle_origin - GET_CYCLES();
        if (CPU_STOPPED) m->cycle = last_boundary + 4; /* STOP discards the slice */
    }
}

/* Predict every line's DMA slots for the first frame after a restore by
 * running one Copper frame on a scratch copy of the machine (the restored
 * display registers are usually whatever the Copper last left). The copy
 * cannot start blits: the Copper danger bit is cleared. */
static void seed_dma_maps(FA18Machine *m) {
    FA18Machine *copy = malloc(sizeof *copy);
    int64_t saved_end = blit_end, saved_event = fa18_next_event;
    int saved_pending = blit_pending, v;
    if (!copy) return;
    memcpy(copy, m, sizeof *copy);
    copy->copper_danger = 0;
    fa18_machine = copy;
    fa18_copper_restart(copy);
    for (v = 0; v < FA18_PAL_LINES; v++) {
        fa18_copper_run_until(copy, v, FA18_LINE_CCKS);
        fa18_bus_line(copy, v, 0);
    }
    fa18_machine = m;
    free(copy);
    blit_end = saved_end;
    blit_pending = saved_pending;
    fa18_next_event = saved_event;
    update_irq(m);
}

/* ---- UAE savestate ------------------------------------------------------- */

static uint32_t be32(const uint8_t *p) {
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}
static uint16_t be16(const uint8_t *p) { return (uint16_t)(p[0] << 8 | p[1]); }

static const uint8_t *find_chunk(const uint8_t *s, size_t size, const char *name, size_t *len) {
    size_t p = 0;
    while (p + 12 <= size) {
        uint32_t n;
        if (!memcmp(s + p, "\0\0\0\0", 4)) { p += 4; continue; }
        n = be32(s + p + 4);
        if (n < 12 || p + n > size) return NULL;
        if (!memcmp(s + p, name, 4)) {
            if (be32(s + p + 8) & 1) return NULL; /* compressed chunks are not supported */
            *len = n - 12;
            return s + p + 12;
        }
        if (!memcmp(s + p, "END ", 4)) return NULL;
        p += (n + 3) & ~3u;
    }
    return NULL;
}

static void load_cia(FA18Cia *c, const uint8_t *b) {
    memset(c, 0, sizeof *c);
    c->pra = b[0]; c->prb = b[1]; c->ddra = b[2]; c->ddrb = b[3];
    c->ta = be16(b + 4); c->tb = be16(b + 6);
    c->tod = (uint32_t)b[8] | (uint32_t)b[9] << 8 | (uint32_t)b[10] << 16;
    c->sdr = b[12];
    c->icr = b[13];
    c->cra = b[14]; c->crb = b[15];
    c->imask = b[16];
    c->ta_latch = (uint16_t)(b[17] | b[18] << 8);
    c->tb_latch = (uint16_t)(b[19] | b[20] << 8);
    c->tod_alarm = (uint32_t)b[24] | (uint32_t)b[25] << 8 | (uint32_t)b[26] << 16;
    c->tod_stopped = !(b[27] & 2);
}

int fa18_machine_load_state(FA18Machine *m, const uint8_t *s, size_t size,
                            const uint8_t *rom, size_t rom_size, char *error, size_t error_size) {
    size_t len;
    const uint8_t *cpu, *chip, *cram, *bram, *ciaa, *ciab;
    int i;
    uint32_t p;
    if (size < 12 || memcmp(s, "ASF ", 4)) {
        snprintf(error, error_size, "not a UAE savestate");
        return 0;
    }
    if (rom_size != FA18_ROM_SIZE) {
        snprintf(error, error_size, "Kickstart image must be 256 KiB");
        return 0;
    }
    memset(m, 0, sizeof *m);
    fa18_machine = m;
    fa18_bus_reset();
    fa18_bus_timing = 0; /* reset vectors and restore are not CPU time */
    memcpy(m->rom, rom, FA18_ROM_SIZE);
    cram = find_chunk(s, size, "CRAM", &len);
    if (!cram || len < FA18_CHIP_SIZE) { snprintf(error, error_size, "missing CRAM"); return 0; }
    memcpy(m->chip, cram, FA18_CHIP_SIZE);
    bram = find_chunk(s, size, "BRAM", &len);
    if (!bram || len < FA18_SLOW_SIZE) { snprintf(error, error_size, "missing BRAM"); return 0; }
    memcpy(m->slow, bram, FA18_SLOW_SIZE);
    {
        const uint8_t *boro = find_chunk(s, size, "BORO", &len);
        if (boro) memcpy(m->rtarea, boro, len < sizeof m->rtarea ? len : sizeof m->rtarea);
    }

    chip = find_chunk(s, size, "CHIP", &len);
    if (!chip || len < 360) { snprintf(error, error_size, "missing CHIP"); return 0; }
    p = 4;
    for (i = 0; i < 0xA0 / 2; i++, p += 2) m->custom[i] = be16(chip + p);
    for (i = 0; i < 16; i++, p += 2) m->custom[0xE0 / 2 + i] = be16(chip + p);
    for (i = 0; i < 16; i++, p += 2) m->custom[0x100 / 2 + i] = be16(chip + p);
    for (i = 0; i < 64; i++, p += 2) m->custom[0x180 / 2 + i] = be16(chip + p);
    m->dmacon = m->custom[0x096 / 2] & 0x07FF;
    m->intena = m->custom[0x09A / 2];
    m->intreq = m->custom[0x09C / 2];
    m->adkcon = m->custom[0x09E / 2];
    m->joy0dat = m->custom[0x00A / 2];
    m->joy1dat = m->custom[0x00C / 2];
    m->mouse_x = m->joy0dat & 0xFF;
    m->mouse_y = m->joy0dat >> 8;
    m->cop1lc = custom_long(m, 0x080);
    m->cop2lc = custom_long(m, 0x084);
    for (i = 0; i < 6; i++) m->bplpt[i] = custom_long(m, 0x0E0 + (uint32_t)i * 4);
    m->vpos = (m->custom[0x004 / 2] & 1) << 8 | m->custom[0x006 / 2] >> 8;
    m->cycle = 0;
    total_lines = last_input_line = 0;
    line_start = -(int64_t)(m->custom[0x006 / 2] & 0xFF) * 2;
    line_started = 0;
    frame_done = 0;
    blit_pending = 0;
    /* The copper resumes from COP1LC at the next frame; lines before it keep
     * the restored register values. */
    m->copper_pc = m->cop1lc;
    m->copper_waiting = 1;
    m->copper_wait_v = 0x1FF;
    m->copper_wait_vmask = 0x1FF;
    m->copper_wait_h = 0;
    m->copper_wait_hmask = 0;

    ciaa = find_chunk(s, size, "CIAA", &len);
    ciab = find_chunk(s, size, "CIAB", &len);
    if (!ciaa || !ciab) { snprintf(error, error_size, "missing CIA state"); return 0; }
    load_cia(&m->cia[0], ciaa);
    load_cia(&m->cia[1], ciab);

    cpu = find_chunk(s, size, "CPU ", &len);
    if (!cpu || len < 8 + 15 * 4 + 4 + 4 + 8 + 2 + 4) { snprintf(error, error_size, "missing CPU"); return 0; }
    if (be32(cpu) != 68000) { snprintf(error, error_size, "savestate CPU is not a 68000"); return 0; }
    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    fa18_cpu_timing_init();
    m68k_pulse_reset();
    {
        const uint8_t *r = cpu + 8;
        uint32_t pc = be32(r + 60);
        uint32_t usp = be32(r + 68), isp = be32(r + 72);
        uint16_t sr = be16(r + 76);
        uint32_t cpumode = be32(r + 78);
        m68k_set_reg(M68K_REG_SR, sr);
        m68k_set_reg(M68K_REG_USP, usp);
        m68k_set_reg(M68K_REG_ISP, isp);
        for (i = 0; i < 8; i++) m68k_set_reg((m68k_register_t)(M68K_REG_D0 + i), be32(r + 4 * i));
        for (i = 0; i < 7; i++) m68k_set_reg((m68k_register_t)(M68K_REG_A0 + i), be32(r + 32 + 4 * i));
        m68k_set_reg(M68K_REG_A7, (sr & 0x2000) ? isp : usp);
        m68k_set_reg(M68K_REG_PC, pc);
        if (cpumode & 1) {
            snprintf(error, error_size, "savestate CPU is halted/stopped; not supported yet");
            return 0;
        }
    }
    update_irq(m);
    seed_dma_maps(m);
    fa18_bus_line(m, m->vpos, line_start);
    fa18_bus_timing = 1;
    return 1;
}
