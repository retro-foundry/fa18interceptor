/* Glue for update_message $C11BFC. Its caller reads D0, D1 and A0, which
 * hold whatever the compiled code last loaded on the path it took, so the
 * path is replayed on a copy of the state taken before the C runs: MOVE.W
 * and MOVE.B into D0/D1 keep the upper bits, EXT.L clears D0's high word
 * when an entry is loaded, and MOVEQ #0 clears it when a timed message
 * runs out. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "messages.h"

typedef struct {
    uint16_t code, loaded, latched, cockpit, crash, posted, state;
    uint32_t causes;
    uint8_t threats, notify, post, flags, sound, time, countdown, posted_time;
} MessageCopy;

static uint32_t d0, d1;

static void w0(uint32_t v) { d0 = (d0 & 0xFFFF0000u) | (uint16_t)v; }
static void b0(uint32_t v) { d0 = (d0 & 0xFFFFFF00u) | (uint8_t)v; }
static void w1(uint32_t v) { d1 = (d1 & 0xFFFF0000u) | (uint16_t)v; }
static void b1(uint32_t v) { d1 = (d1 & 0xFFFFFF00u) | (uint8_t)v; }

static void load(MessageCopy *m) {
    m->code = rd_u16(MESSAGE_CODE);
    m->loaded = rd_u16(MESSAGE_LOADED);
    m->latched = rd_u16(MESSAGE_SHOWN);
    m->cockpit = rd_u16(COCKPIT_FLAGS);
    m->crash = rd_u16(CRASH_FLAGS);
    m->posted = rd_u16(POSTED_FLAGS);
    m->state = rd_u16(MESSAGE_STATE);
    m->causes = rd_u32(WARNING_CAUSES);
    m->threats = rd_u8(THREAT_EVENTS);
    m->notify = rd_u8(NOTIFY_CODE);
    m->post = rd_u8(POST_INPUT_EVENT);
    m->flags = rd_u8(MESSAGE_FLAGS);
    m->sound = rd_u8(MESSAGE_SOUND);
    m->time = rd_u8(MESSAGE_TIME);
    m->countdown = rd_u8(MESSAGE_COUNTDOWN);
    m->posted_time = rd_u8(POSTED_TIME);
}

/* $C11C6A-$C11D58, from the threats and warnings. */
static uint16_t threat_regs(MessageCopy *m, uint16_t code) {
    uint8_t causes = (uint8_t)m->causes, t = m->threats;
    m->cockpit |= 0x80;
    w0(m->cockpit);
    b0(t);
    if (t & 0x10) return 0x8010;
    if (t & 0x20) return 0x8012;
    if (causes & 0x40) return 0xC004;
    if (causes & 0x80) return 0xC001;
    if (causes & 0x02) return 0x8002;
    if (t & 0x08) { m->cockpit &= 0xFF7F; w0(m->cockpit); return 0x8024; }
    if (t & 0x04) { m->cockpit &= 0xFF7F; w0(m->cockpit); return 0x800E; }
    if (causes & 0x01) return 0x0C09;
    if (t & 0x02) { m->cockpit &= 0xFF7F; w0(m->cockpit); return 0x400D; }
    return code;
}

static uint16_t wanted_regs(MessageCopy *m) {
    uint16_t code = m->code;
    w0(m->code);
    w1(m->cockpit);
    if (m->cockpit & 1) {
        w0(m->code);
        if (!m->code) {
            m->cockpit &= 0xFFFE;
            w0(m->cockpit);
            code = 0;
        }
        return code;
    }
    w0(m->crash & 0x8000);
    if (m->crash & 0x8000) return 0x9C06;
    w0(m->posted);
    if (m->posted & 0x20) return 0x4000;
    if (m->causes || m->threats) return threat_regs(m, code);
    w0(m->cockpit);
    if (!(m->cockpit & 4)) code = 0;
    m->cockpit &= 0xFF7F;
    w0(m->cockpit);
    return code;
}

static void start_regs(MessageCopy *m) {
    m->state |= 0x2000;
    w0(m->state);
    w0(m->posted);
    m->countdown = (m->posted & 0x20) ? m->posted_time : m->time;
}

static void end_regs(MessageCopy *m) {
    m->state &= 0xDFFF;
    w0(m->state);
    m->posted &= 0xFFDF;
    w0(m->posted);
    m->cockpit &= 0xFFFE;
    w0(m->cockpit);
}

static void message_registers(MessageCopy *m) {
    uint16_t code = wanted_regs(m);

    w0(m->loaded & 0xFF);
    w1(code & 0xFF);
    if ((m->loaded & 0xFF) != (code & 0xFF)) {
        uint16_t index = code & 0xFF;
        m->code = m->loaded = code;
        w0(code);
        m->state &= 0xDFFF;
        w0(m->state);
        m->cockpit &= 0xFFFB;
        w0(m->cockpit);
        w0(index);
        w0(index << 4);
        w1(index << 3);
        w0(d0 + d1);
        w1(index << 2);
        w0(d0 + d1);                             /* index * 28 */
        d0 = (uint32_t)(int32_t)(int16_t)d0;     /* EXT.L */
        A(0) = d0 + MESSAGE_TABLE;
        b1(rd_u8(A(0)));
        m->flags = (uint8_t)d1;
        d0 += 0x1B;
        A(0) = d0 + MESSAGE_TABLE;
        m->sound = rd_u8(A(0));
        b0(m->flags);
        b0(d0 & 0x3F);
        m->time = (uint8_t)d0;
    }

    code = m->code;
    w0(code & 0x8000);
    if (code & 0x8000) {
        uint8_t mask;
        w0(m->state);
        if (!(m->state & 0x2000)) start_regs(m);
        w0(code);
        mask = (code & 0x1000) ? 4 : 2;
        b0(m->notify);
        b0(d0 & mask);
        if (m->notify & mask) {
            int first = 1;
            w0(m->state);
            if (m->state & 0x20) {
                b0(m->post);
                if (!m->post) {
                    first = 0;
                    m->state &= 0xFFDF;
                    w0(m->state);
                    w0(code & 0xC000);
                    if ((code & 0xC000) != 0xC000) w0(code + 1);
                }
            }
            if (first) {
                m->state |= 0x20;
                w0(m->state);
                m->latched = code;
                if (m->time) {
                    b0(m->countdown - 1);
                    m->countdown = (uint8_t)d0;
                    if ((int8_t)m->countdown < 0) {
                        w0(m->latched & 0xFF);
                        end_regs(m);
                    }
                }
            }
        }
    } else {
        w0(code);
        if (code & 0x4000) {
            w0(m->state);
            if (!(m->state & 0x2000)) {
                w0(m->state);
                start_regs(m);
            }
            b0(m->notify);
            if (m->notify & 2) {
                b0(m->countdown - 1);
                m->countdown = (uint8_t)d0;
                if ((int8_t)m->countdown < 0) {
                    d0 = 0;
                    end_regs(m);
                }
            }
        }
    }
    b0(m->flags);
    b0(d0 & 0xC0);
    b1(rd_u8(MESSAGE_KIND));
}

int glue_C11BFC(void) {
    MessageCopy copy;

    load(&copy);
    d0 = D(0);
    d1 = D(1);
    message_registers(&copy);
    update_message();
    D(0) = d0;
    D(1) = d1;
    return glue_return();
}
