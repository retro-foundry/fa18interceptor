#ifndef FA18_MACHINE_H
#define FA18_MACHINE_H

#include <stddef.h>
#include <stdint.h>

/* Minimal A500 PAL OCS machine for running the original game code: 512 KiB
 * Chip RAM at $000000, 512 KiB Slow RAM at $C00000, Kickstart 1.3 at
 * $FC0000 (mirrored at $F80000), custom chips at $DFF000 and the two CIAs.
 * The CPU is Musashi; translated game routines take over through the
 * instruction hook (see recomp_runtime.h). */

#define FA18_CHIP_SIZE 0x80000u
#define FA18_SLOW_BASE 0xC00000u
#define FA18_SLOW_SIZE 0x80000u
#define FA18_ROM_BASE 0xFC0000u
#define FA18_ROM_SIZE 0x40000u

#define FA18_PAL_LINES 313
#define FA18_LINE_CCKS 227
#define FA18_LINE_CYCLES (FA18_LINE_CCKS * 2)
/* A frame ends, and the next frame's input is applied, when line 3 starts:
 * where UAE (Engine9000) ends retro_run, after the vertical blank
 * interrupt. Its savestates are taken there too. */
#define FA18_FRAME_END_LINE 3

/* The native image covers the standard lowres PAL display area: DIW
 * horizontal $81 is x=0 and beam line $2A is y=0. */
#define FA18_SCREEN_W 320
#define FA18_SCREEN_H 256
#define FA18_SCREEN_HSTART 0x81
#define FA18_SCREEN_VSTART 0x2A

typedef struct {
    uint8_t pra, prb, ddra, ddrb;
    uint16_t ta, tb, ta_latch, tb_latch;
    uint8_t cra, crb;
    uint8_t icr, imask;
    uint8_t sdr;
    uint32_t tod, tod_alarm, tod_latch;
    int tod_latched, tod_stopped;
    int ta_pending_eclocks; /* fractional E-clock accumulator */
} FA18Cia;

typedef struct {
    uint8_t chip[FA18_CHIP_SIZE];
    uint8_t slow[FA18_SLOW_SIZE];
    uint8_t rom[FA18_ROM_SIZE];
    uint8_t rtarea[0x10000]; /* UAE boot ROM board at $F00000 (savestate BORO) */
    uint16_t custom[0x100]; /* last written custom register values */
    uint16_t dmacon, intena, intreq, adkcon;
    uint32_t cop1lc, cop2lc, copper_pc;
    int copper_waiting; /* 1 while a WAIT is unsatisfied */
    uint16_t copper_wait_v, copper_wait_h, copper_wait_vmask, copper_wait_hmask;
    int copper_danger;
    /* Copper activity on the current line, for DMA slot accounting: runs of
     * instructions and the CCK each run starts at. */
    int copper_segments;
    int copper_segment_start[16], copper_segment_count[16];
    uint32_t bplpt[6];
    int vpos, hpos;
    uint64_t frame;      /* completed frames since restore */
    uint64_t cycle;      /* CPU cycles since restore */
    int eclock_frac;
    FA18Cia cia[2];
    uint16_t joy0dat, joy1dat, pot;
    int mouse_x, mouse_y;
    int mouse_dx, mouse_dy; /* motion not yet seen by the mouse counters */
    int mouse_left, mouse_right, joy_fire; /* 1 = pressed */
    uint8_t keyboard_queue[64];
    int keyboard_head, keyboard_tail, keyboard_cooldown;
    uint16_t screen[FA18_SCREEN_W * FA18_SCREEN_H];      /* RGB444, frame in progress */
    uint16_t last_screen[FA18_SCREEN_W * FA18_SCREEN_H]; /* last completed frame */
    uint64_t unmapped_reads, unmapped_writes;
    uint64_t blits, line_blits;
} FA18Machine;

extern FA18Machine *fa18_machine;
/* BLTSIZE as written just before the last polygon draw began. */
extern uint16_t fa18_bltsize_at_draw_start;

int fa18_machine_load_state(FA18Machine *m, const uint8_t *state, size_t size,
                            const uint8_t *rom, size_t rom_size, char *error, size_t error_size);

/* Run whole frames. Output of each completed frame is in last_screen. */
void fa18_machine_run_frame(FA18Machine *m);

/* Keyboard: Amiga raw key code (0..$7F), down or up. */
void fa18_machine_key(FA18Machine *m, int rawkey, int down);
void fa18_machine_mouse(FA18Machine *m, int dx, int dy);
/* Port 0 mouse buttons (0 left, 1 right) and port 1 joystick fire. */
void fa18_machine_button(FA18Machine *m, int button, int down);
/* Port 1 joystick directions (1 = held). */
void fa18_machine_joystick(FA18Machine *m, int up, int down, int left, int right);

uint8_t fa18_bus_read8(uint32_t address);
uint16_t fa18_bus_read16(uint32_t address);
uint32_t fa18_bus_read32(uint32_t address);
void fa18_bus_write8(uint32_t address, uint8_t value);
void fa18_bus_write16(uint32_t address, uint16_t value);
void fa18_bus_write32(uint32_t address, uint32_t value);

/* Chipset internals shared between machine.c, blitter.c and display.c. */
void fa18_custom_write(FA18Machine *m, uint32_t reg, uint16_t value);
uint16_t fa18_custom_read(FA18Machine *m, uint32_t reg);
void fa18_blitter_start(FA18Machine *m);
void fa18_copper_restart(FA18Machine *m);
void fa18_copper_run_until(FA18Machine *m, int vpos, int hpos);
void fa18_display_line(FA18Machine *m, int vpos);
void fa18_raise_interrupt(FA18Machine *m, int bit);

/* Current CPU cycle (start of the executing instruction). */
int64_t fa18_machine_now(void);

/* Beam position including cycles spent in the current execution slice. */
void fa18_machine_beam(int *vpos, int *hpos);

/* Instruction-boundary chipset service (see machine.c). */
int fa18_machine_service(void);
int fa18_machine_event_due(void);
void fa18_machine_wait_blitter(void);
/* Provisional counted BBUSY loop for the inactive $C2FD8C port. */
uint16_t fa18_machine_count_blitter_polls(void);

static inline uint16_t fa18_chip16(const FA18Machine *m, uint32_t a) {
    a &= FA18_CHIP_SIZE - 2;
    return (uint16_t)(m->chip[a] << 8 | m->chip[a + 1]);
}
static inline void fa18_chip16_set(FA18Machine *m, uint32_t a, uint16_t v) {
    a &= FA18_CHIP_SIZE - 2;
    m->chip[a] = (uint8_t)(v >> 8);
    m->chip[a + 1] = (uint8_t)v;
}

#endif
