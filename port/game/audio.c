/* Sound output levels. */
#include "audio.h"

#include "globals.h"
#include "hardware.h"

#define FIXED_UNIT 0x10000
#define MAX_VOLUME (63 * FIXED_UNIT)
#define FADE_STEP (FIXED_UNIT / 4)

void set_voice_output(gaddr channel, gaddr voice) {
    int16_t period = (int16_t)(rd_u32(voice + VOICE_PERIOD) >> 16);
    int16_t volume = (int16_t)((rd_u32(voice + VOICE_VOLUME) >> 16) & 63);
    int16_t master = rd_s16(MASTER_VOLUME);

    if (period < PAULA_MIN_PERIOD) period = PAULA_MIN_PERIOD;
    wr_s16(channel + AUD_PERIOD, period);
    if (volume > master) volume = master;
    wr_s16(channel + AUD_VOLUME, volume);
}

void fade_master_volume(void) {
    int32_t target, level;

    if (!rd_u8(VOLUME_FADING)) return;
    target = rd_s32(MASTER_VOLUME_TARGET);
    level = rd_s32(MASTER_VOLUME);
    if (level == target) return;
    if (level > target) {
        level -= FADE_STEP;
        if (level < 0) level = 0;
    } else {
        level += FADE_STEP;
        if (level > MAX_VOLUME) level = MAX_VOLUME;
    }
    wr_s32(MASTER_VOLUME, level);
}

void clear_voice_interrupt(int channel) {
    gaddr voice = rd_u32(VOICE_TABLE + (gaddr)(int32_t)(int16_t)(channel * 4));
    custom_write(INTREQ, rd_u16(voice + VOICE_INTERRUPT));
}

void step_voice_program(gaddr voice, gaddr slot, int channel) {
    gaddr program, pc;

    if (rd_u32(voice + VOICE_DELAY) == 0) return;
    wr_u32(voice + VOICE_DELAY, rd_u32(voice + VOICE_DELAY) - 1);
    if (rd_u32(voice + VOICE_DELAY) != 0) return;

    program = rd_u32(voice + VOICE_PROGRAM);
    pc = program + rd_u32(voice + VOICE_POSITION);
    for (;;) {
        int32_t field = rd_s32(pc);
        uint32_t value = rd_u32(pc + 4);
        pc += 8;
        if (field < VOICE_DELAY) {
            wr_u32(voice + (gaddr)field, value);
            continue;
        }
        if (field == VOICE_DELAY) {
            wr_u32(voice + VOICE_DELAY, value);
            wr_u32(voice + VOICE_POSITION, pc - program);
            if (value == 0) {
                wr_u32(slot, 0);
                clear_voice_interrupt(channel);
            }
            return;
        }
        {
            gaddr counter = voice + VOICE_LOOP_COUNTERS + (gaddr)(int32_t)(int16_t)(field - 0x40);
            if (rd_u32(counter) != 0) {
                wr_u32(counter, rd_u32(counter) - 1);
                if (rd_u32(counter) == 0) continue;
            }
            pc = program + value;
        }
    }
}

void free_voice(int channel) {
    wr_u32(VOICE_SLOTS + (gaddr)(channel * 4), 0);
    clear_voice_interrupt(channel);
}

void stop_channel_2(void) { free_voice(2); }

void play_sound(int sound, int channel, int32_t volume) {
    gaddr entry = SOUND_VOICES + (gaddr)(sound * 4);
    if (!rd_u32(entry)) return;
    free_voice(channel);
    wr_u32(rd_u32(entry) + VOICE_VOLUME, (uint32_t)volume << 16);
    wr_u32(VOICE_SLOTS + (gaddr)(channel * 4), rd_u32(entry));
    clear_voice_interrupt(channel);
}

void play_alert_tone(int32_t volume) {
    gaddr voice;
    if (!(rd_u8(SOUND_FLAGS) & 0x04)) return;
    if (rd_u16(SCRIPT_RECORD) != rd_u16(VIEW_RECORD)) return;
    voice = rd_u32(SOUND_VOICES + 4 * SOUND_ALERT);
    if (!voice) return;
    wr_u32(voice + VOICE_PERIOD, 0x1360000u); /* period 310 */
    wr_u32(voice + 0x18, 0xFFFE0000u);        /* pitch slide -2 per tick */
    wr_u32(voice + 0x10, 1);
    play_sound(SOUND_ALERT, 2, volume);
}
