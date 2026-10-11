/* Reference-only harness. original_audio_startup.h is extracted verbatim from
 * the pinned original audio.c; it is never linked into the playable runner. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t uae_u8;
typedef int8_t sample8_t;
typedef uint16_t uae_u16;
typedef uint32_t uae_u32;
typedef uint32_t uaecptr;
#define STATIC_INLINE static inline
#define CYCLE_UNIT 512
#define MAX_EV UINT32_MAX
#define DMA_MASTER 0x200
#define AUDIO_CHANNELS_PAULA 4
#define SINC_QUEUE_LENGTH 256
#define VOLCNT_BUFFER_SIZE 4096
#define _T(x) x
#define M68K_GETPC 0

static struct {
    int produce_sound, sound_volcnt, cs_hacks, cpu_model, m68k_speed;
    int cachesize, cpu_compatible, cpu_memory_cycle_exact;
} currprefs = {3, 0, 0, 68000, 0, 0, 1, 1};
static struct { unsigned instruction_cnt; } regs;
static unsigned dmacon, adkcon, irq;
static int audio_channel_mask = 15, sampleripper_enabled;
static int current_hpos(void) { return 21; }
static unsigned INTREQR(void) { return irq; }
/* Record a scheduled interrupt without inventing delivery/handler timing. */
static unsigned scheduled_interrupts;
static void INTREQ_INT(int bit, unsigned delay) {
    assert(delay == CYCLE_UNIT);
    scheduled_interrupts |= 1u << bit;
}
static bool dmaen(unsigned bit) { return (dmacon & bit) != 0; }
static int audio_activate(void) { return 0; } /* host output already active */
static void write_log(const char *format, ...) { (void)format; abort(); }
static void event2_newevent_xx(int slot, unsigned delay, unsigned value,
                              void (*callback)(unsigned)) {
    /* Only the separately scheduled DSR flag is observable in this scope.
     * Never deliver it early; no tested startup length reaches this path. */
    (void)slot; (void)delay; (void)value; (void)callback; abort();
}

#include "original_audio_startup.h"

static void initialize(int channel, unsigned pointer, unsigned length,
                       unsigned period, unsigned attach) {
    memset(audio_channel, 0, sizeof audio_channel);
    dmacon = DMA_MASTER | (1u << channel);
    adkcon = attach;
    irq = scheduled_interrupts = 0;
    struct audio_channel_data *c = audio_channel + channel;
    c->lc = pointer;
    c->pt = 0;
    c->len = length;
    c->per = period * CYCLE_UNIT;
    c->data.audvol = 10;
    /* Nonzero sentinels distinguish discarding a zero from emitting silence. */
    c->dat2 = 0x7193;
    c->data.current_sample = 73;
    c->data.last_sample = -91;
}

static void startup(int channel, unsigned pointer, unsigned prefetch) {
    struct audio_channel_data *c = audio_channel + channel;
    assert(audio_state_channel2(channel, false));
    assert(c->state == 1);
    unsigned bits = audio_dmal() >> (2 * channel);
    assert((bits & 3) == 3);
    assert(audio_getpt(channel, (bits & 1) != 0) == 0);
    assert(c->pt == pointer);
    c->dat = prefetch;
    c->dat_written = true;
    assert(audio_state_channel2(channel, false));
    c->dat_written = false;
    assert(c->state == 5 && c->dat2 == 0x7193);
    assert(c->data.current_sample == 73 && c->data.last_sample == -91);
    assert(scheduled_interrupts == (0x80u << channel));
}

int main(int argc, char **argv) {
    assert(argc == 5);
    unsigned channel = strtoul(argv[1], NULL, 0);
    unsigned pointer = strtoul(argv[2], NULL, 0);
    unsigned prefetch = strtoul(argv[3], NULL, 0);
    unsigned first_word = strtoul(argv[4], NULL, 0);
    assert(channel < 4 && !(pointer & 1) && pointer < 0x80000);
    assert(prefetch <= 0xffff && first_word <= 0xffff);
    initialize(channel, pointer, 2062, 358, 0);
    startup(channel, pointer, prefetch);
    struct audio_channel_data *c = audio_channel + channel;
    unsigned bits = audio_dmal() >> (2 * channel);
    assert((bits & 3) == 2);
    assert(audio_getpt(channel, false) == pointer);
    c->dat = first_word;
    c->dat_written = true;
    assert(audio_state_channel2(channel, false));
    c->dat_written = false;
    assert(c->state == 2 && c->dat2 == first_word);
    assert(c->data.current_sample == (int8_t)(first_word >> 8));
    assert(audio_state_channel2(channel, true));
    assert(c->state == 3 && c->data.current_sample == (int8_t)first_word);
    printf("{\"startup_states\":[0,1,5,2,3],\"startup_word_emitted\":false,"
           "\"first_payload_bytes\":[%u,%u],", first_word >> 8, first_word & 255);

    unsigned guards = 0;
    /* A nonzero priming word must also disappear. Test every possible word,
     * every channel and every attachment setting; no silence mask involved. */
    for (unsigned word = 0; word <= 0xffff; word++) {
        initialize(channel, pointer, 2062, 358, 0);
        startup(channel, pointer, word);
        guards++;
    }
    for (int ch = 0; ch < 4; ch++) {
        for (unsigned attach = 0; attach <= 255; attach++) {
            initialize(ch, pointer, 2062, 358, attach);
            startup(ch, pointer, 0xbeef);
            guards++;
        }
    }
    printf("\"prefetch_poison_and_attachment_cases\":%u}\n", guards);
    return 0;
}
