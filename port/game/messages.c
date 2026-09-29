/* The cockpit message line. */
#include "messages.h"

#include "globals.h"
#include "memory.h"

#define FLASH   0x8000u
#define TIMED   0x4000u
#define SLOW    0x1000u
#define ENTRY   0xFFu

#define ENTRY_SIZE   28
#define ENTRY_SOUND  27

/* COCKPIT_FLAGS bits. */
#define POSTED_HELD   0x01u
#define KEEP_MESSAGE  0x04u
#define THREAT_SHOWN  0x80u

/* MESSAGE_STATE bits. */
#define TIMING        0x2000u
#define SECOND_PHASE  0x0020u

static void set_bits(gaddr a, uint16_t bits) { wr_u16(a, (uint16_t)(rd_u16(a) | bits)); }
static void clear_bits(gaddr a, uint16_t bits) { wr_u16(a, (uint16_t)(rd_u16(a) & ~bits)); }

/* The threat or warning to show, in priority order; `code` when none. */
static uint16_t threat_message(uint16_t code) {
    uint8_t threats = rd_u8(THREAT_EVENTS), causes = (uint8_t)rd_u32(WARNING_CAUSES);

    set_bits(COCKPIT_FLAGS, THREAT_SHOWN);
    if (threats & 0x10) return 0x8010;     /* ALERT: IR MISSILE, flashing */
    if (threats & 0x20) return 0x8012;     /* ALERT: RH MISSILE */
    if (causes & 0x40) return 0xC004;      /* FUEL EXHAUSTED, flashing off */
    if (causes & 0x80) return 0xC001;      /* STALL */
    if (causes & 0x02) return 0x8002;      /* LOW FUEL - CRITICAL */
    if (threats & 0x08) { clear_bits(COCKPIT_FLAGS, THREAT_SHOWN); return 0x8024; } /* CRUISE MISSILE */
    if (threats & 0x04) { clear_bits(COCKPIT_FLAGS, THREAT_SHOWN); return 0x800E; } /* ENEMY IN VICINITY */
    if (causes & 0x01) return 0x0C09;      /* LOW FUEL */
    if (threats & 0x02) { clear_bits(COCKPIT_FLAGS, THREAT_SHOWN); return 0x400D; } /* FRIENDLY IN VICINITY */
    return code;
}

static uint16_t wanted_message(void) {
    uint16_t code = rd_u16(MESSAGE_CODE);

    if (rd_u16(COCKPIT_FLAGS) & POSTED_HELD) {
        if (!rd_u16(MESSAGE_CODE)) {
            clear_bits(COCKPIT_FLAGS, POSTED_HELD);
            code = 0;
        }
        return code;
    }
    if (rd_u16(CRASH_FLAGS) & 0x8000) return 0x9C06;      /* CRASH IMMINENT */
    if (rd_u16(POSTED_FLAGS) & 0x20) return TIMED;
    if (rd_u32(WARNING_CAUSES) || rd_u8(THREAT_EVENTS)) return threat_message(code);
    if (!(rd_u16(COCKPIT_FLAGS) & KEEP_MESSAGE)) code = 0;
    clear_bits(COCKPIT_FLAGS, THREAT_SHOWN);
    return code;
}

/* Load a new entry's flags, sound and time. */
static void load_message(uint16_t code) {
    gaddr entry = MESSAGE_TABLE + (gaddr)((code & ENTRY) * ENTRY_SIZE);

    wr_u16(MESSAGE_CODE, code);
    wr_u16(MESSAGE_LOADED, code);
    clear_bits(MESSAGE_STATE, TIMING);
    clear_bits(COCKPIT_FLAGS, KEEP_MESSAGE);
    wr_u8(MESSAGE_FLAGS, rd_u8(entry));
    wr_u8(MESSAGE_SOUND, rd_u8(entry + ENTRY_SOUND));
    wr_u8(MESSAGE_TIME, (uint8_t)(rd_u8(MESSAGE_FLAGS) & 0x3F));
}

static void start_timing(void) {
    set_bits(MESSAGE_STATE, TIMING);
    wr_u8(MESSAGE_COUNTDOWN, rd_u8((rd_u16(POSTED_FLAGS) & 0x20) ? POSTED_TIME : MESSAGE_TIME));
}

/* One step off the countdown; true when it has run out. */
static int count_down(void) {
    uint8_t left = (uint8_t)(rd_u8(MESSAGE_COUNTDOWN) - 1);
    wr_u8(MESSAGE_COUNTDOWN, left);
    return (int8_t)left < 0;
}

/* A flash that has run out settles on its entry; a timed message on none,
 * leaving MESSAGE_LOADED. */
static void end_message(uint16_t code, int settle) {
    wr_u16(MESSAGE_SHOWN, code);
    if (settle) wr_u16(MESSAGE_LOADED, code);
    wr_u16(MESSAGE_CODE, code);
    clear_bits(MESSAGE_STATE, TIMING);
    clear_bits(POSTED_FLAGS, 0x20);
    clear_bits(COCKPIT_FLAGS, POSTED_HELD);
}

static void flash_message(uint16_t code) {
    if (!(rd_u16(MESSAGE_STATE) & TIMING)) start_timing();
    if (!(rd_u8(NOTIFY_CODE) & ((code & SLOW) ? 4 : 2))) return;
    if ((rd_u16(MESSAGE_STATE) & SECOND_PHASE) && !rd_u8(POST_INPUT_EVENT)) {
        /* Second phase: the next entry, or none for a timed flash. */
        clear_bits(MESSAGE_STATE, SECOND_PHASE);
        wr_u16(MESSAGE_SHOWN, (code & (FLASH | TIMED)) == (FLASH | TIMED) ? 0 : (uint16_t)(code + 1));
        if (rd_u8(MESSAGE_SOUND)) wr_u8(EVENT_BITS + 3, (uint8_t)(rd_u8(EVENT_BITS + 3) | 0x08));
        return;
    }
    set_bits(MESSAGE_STATE, SECOND_PHASE);
    wr_u16(MESSAGE_SHOWN, code);
    if (rd_u8(MESSAGE_SOUND)) wr_u8(EVENT_BITS + 3, (uint8_t)(rd_u8(EVENT_BITS + 3) | 0x04));
    if (rd_u8(MESSAGE_TIME) && count_down()) end_message(rd_u16(MESSAGE_SHOWN) & ENTRY, 1);
}

static void time_message(uint16_t code) {
    if (!(rd_u16(MESSAGE_STATE) & TIMING)) {
        start_timing();
        wr_u16(MESSAGE_SHOWN, code);
    }
    if ((rd_u8(NOTIFY_CODE) & 2) && count_down()) end_message(0, 0);
}

void update_message(void) {
    uint16_t code = wanted_message();
    uint8_t kind;

    if ((rd_u16(MESSAGE_LOADED) & ENTRY) != (code & ENTRY)) load_message(code);
    code = rd_u16(MESSAGE_CODE);
    if (code & FLASH) flash_message(code);
    else if (code & TIMED) time_message(code);
    else wr_u16(MESSAGE_SHOWN, code);

    kind = (uint8_t)(rd_u8(MESSAGE_FLAGS) & 0xC0);
    wr_u8(MESSAGE_FLAGS, kind);
    if (rd_u8(MESSAGE_KIND) != kind) {
        wr_u8(MESSAGE_KIND, kind);
        wr_u8(MESSAGE_REDRAWS, 2);
    }
    wr_u8(NOTIFY_CODE, 0);
}

void take_warning_events(void) {
    if (!(rd_u32(WARNING_CAUSES) & 0x4200)) return;
    wr_u32(WARNING_CAUSES, rd_u32(WARNING_CAUSES) & ~0x4200u);
    wr_u32(EVENT_BITS, rd_u32(EVENT_BITS) | 8);
}

void post_message(uint16_t code) {
    uint16_t kind = (uint16_t)(code & 0xFF00);
    wr_u16(MESSAGE_CODE, code);
    set_bits(COCKPIT_FLAGS, POSTED_HELD);
    if (kind & 0x2000) {
        clear_bits(MESSAGE_STATE, 0x8000);
        set_bits(MESSAGE_STATE, TIMING);
    } else if (kind == 0x4000 || kind == 0x4800) {
        clear_bits(MESSAGE_STATE, TIMING);
    }
}
