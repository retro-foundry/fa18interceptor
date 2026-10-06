/* Sound output levels. */
#include "audio.h"
#include "menu_setup.h"

#include "fixed_math.h"

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
#ifdef FA18_NATIVE
    /* No Paula interrupt is pending in the host's silent frontend. Voice
     * ownership is still cleared by free_voice; no register is emulated. */
    (void)channel;
#else
    gaddr voice = rd_u32(VOICE_TABLE + (gaddr)(int32_t)(int16_t)(channel * 4));
    custom_write(INTREQ, rd_u16(voice + VOICE_INTERRUPT));
#endif
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

static void menu_sound_child(void *context,enum MenuSetupCall call,uint32_t value) {
    (void)context;
    switch(call) {
    case MENU_SOUND_FREE_BEFORE: case MENU_SOUND_FREE_OTHER: free_all_voices(); break;
    case MENU_SOUND_FIXED_FIRST: play_sound(13,0,(int32_t)value); break;
    case MENU_SOUND_FIXED_SECOND: play_sound(14,1,(int32_t)value); break;
    case MENU_SOUND_ARGUMENT_FIRST: play_sound(35,0,(int32_t)value); break;
    case MENU_SOUND_ARGUMENT_SECOND: play_sound(36,1,(int32_t)value); break;
    default: break;
    }
}
void start_menu_sound_pair(int32_t volume) {
    static const MenuSetupHooks hooks={menu_sound_child,NULL,NULL};
    select_menu_sound_pair((uint32_t)volume,&hooks);
}

void start_sound_6(int32_t period, int32_t ticks) {
    gaddr voice = rd_u32(0xC0A450u);
    int32_t remainder;
    if (!(rd_u8(SOUND_FLAGS - 1) & 1)) {
        wr_u8(FIRE_STATE, 0xFA);
        wr_u8(0xC45797u, 2);
        return;
    }
    if (!voice) return;
    free_voice(2);
    wr_u32(0xC50BDCu, (uint32_t)period << 16);
    wr_s32(0xC50BE4u, long_divide((int32_t)(0u - ((uint32_t)period << 16)), ticks, &remainder));
    wr_s32(0xC50BECu, ticks);
    wr_u32(voice + VOICE_POSITION, 0);
    wr_u32(voice + VOICE_PERIOD, (uint32_t)(random_bits(11) * 4 + 0x231E) << 16);
    wr_u32(voice + VOICE_DELAY, 1);
    play_sound(6, 2, 0);
}

void start_sound_12(int32_t period) {
    gaddr voice = rd_u32(0xC0A468u);
    int32_t remainder;
    if (!(rd_u8(SOUND_FLAGS - 1) & 2) || !voice) return;
    free_voice(2);
    wr_u32(0xC50C4Cu, (uint32_t)period << 16);
    wr_s32(0xC50C54u, long_divide((int32_t)(0u - ((uint32_t)(period - 1) << 16)), 60, &remainder));
    wr_u32(voice + VOICE_POSITION, 0);
    wr_u32(voice + VOICE_PERIOD, (uint32_t)(random_bits(3) + 0x8C) << 16);
    wr_u32(voice + VOICE_DELAY, 1);
    play_sound(12, 2, 0);
}

void dispatch_event_sound(int16_t period, int16_t volume) {
    if (rd_u8(SOUND_FLAGS - 1) & 2) {
        start_sound_12(volume);
        wr_u8(0xC45797u, 0);
    } else {
        play_engine((int32_t)period + 22, volume);
    }
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

static gaddr sound_voice(int sound) {
    return rd_u32(SOUND_VOICES + (gaddr)(4 * sound));
}

void play_engine(int32_t period, int32_t volume) {
    gaddr low, high;
    if (!(rd_u8(SOUND_FLAGS) & 0x02) || !sound_voice(SOUND_ENGINE_HIGH)) return;
    low = sound_voice(SOUND_ENGINE_LOW);
    high = sound_voice(SOUND_ENGINE_HIGH);
    wr_u32(high + VOICE_VOLUME_SLIDE, 0);
    wr_u32(low + VOICE_VOLUME_SLIDE, 0);
    wr_u32(high + VOICE_PERIOD_SLIDE, 0);
    wr_u32(low + VOICE_PERIOD_SLIDE, 0);
    wr_u32(low + VOICE_PERIOD, (uint32_t)period << 16);
    wr_u32(high + VOICE_PERIOD, (uint32_t)(period + 2) << 16);
    play_sound(SOUND_ENGINE_LOW, 0, volume);
    play_sound(SOUND_ENGINE_HIGH, 1, volume);
}

void slide_engine(int32_t period, int32_t volume, int32_t ticks) {
    gaddr high, low;
    int32_t remainder, step;
    if (!(rd_u8(SOUND_FLAGS) & 0x02) || !rd_u32(VOICE_SLOTS + 4)) return;
    high = rd_u32(VOICE_SLOTS + 4);
    low = rd_u32(VOICE_SLOTS);
    step = long_divide((int32_t)((uint32_t)period << 16) - rd_s32(low + VOICE_PERIOD), ticks, &remainder);
    wr_s32(high + VOICE_PERIOD_SLIDE, step);
    wr_s32(low + VOICE_PERIOD_SLIDE, step);
    wr_s32(high + VOICE_PERIOD_TICKS, ticks);
    wr_s32(low + VOICE_PERIOD_TICKS, ticks);
    step = long_divide((int32_t)((uint32_t)volume << 16) - rd_s32(low + VOICE_VOLUME), ticks, &remainder);
    wr_s32(high + VOICE_VOLUME_SLIDE, step);
    wr_s32(low + VOICE_VOLUME_SLIDE, step);
    wr_s32(high + VOICE_VOLUME_TICKS, ticks);
    wr_s32(low + VOICE_VOLUME_TICKS, ticks);
}

void play_noise(int32_t volume) {
    if (!(rd_u8(SOUND_FLAGS) & 0x10)) {
        free_voice(0);
        free_voice(1);
        return;
    }
    if (!sound_voice(SOUND_NOISE_HIGH)) return;
    wr_u32(sound_voice(SOUND_NOISE_LOW) + VOICE_PERIOD, (uint32_t)(random_bits(6) + 0x168) << 16);
    wr_u32(sound_voice(SOUND_NOISE_HIGH) + VOICE_PERIOD, (uint32_t)(random_bits(6) + 0x168) << 16);
    play_sound(SOUND_NOISE_LOW, 0, volume);
    play_sound(SOUND_NOISE_HIGH, 1, volume);
}

void play_programmed_sound(const int32_t a[9]) {
    gaddr voice = sound_voice(SOUND_PROGRAMMED);
    if (!voice) return;
    free_voice(3);
    wr_s32(PROGRAM_4_VALUES + 0x00, a[7]);
    wr_s32(PROGRAM_4_VALUES + 0x08, a[6]);
    wr_u32(PROGRAM_4_VALUES + 0x10, (uint32_t)a[0] << 16);
    wr_u32(PROGRAM_4_VALUES + 0x18, (uint32_t)a[1] << 16);
    wr_s32(PROGRAM_4_VALUES + 0x20, a[2]);
    wr_u32(PROGRAM_4_VALUES + 0x28, (uint32_t)a[3] << 16);
    wr_u32(PROGRAM_4_VALUES + 0x30, (uint32_t)a[4] << 16);
    wr_s32(PROGRAM_4_VALUES + 0x38, a[5]);
    wr_s32(PROGRAM_4_VALUES + 0x48, a[8]);
    voice = sound_voice(SOUND_PROGRAMMED);
    wr_u32(voice + VOICE_POSITION, 0);
    wr_u32(voice + VOICE_DELAY, 1);
    play_sound(SOUND_PROGRAMMED, 3, 0);
}

void play_scripted_sound(int32_t volume) {
    gaddr voice;
    if (!(rd_u8(SOUND_FLAGS) & 0x40)) return;
    if (rd_u16(SCRIPT_RECORD) != rd_u16(VIEW_RECORD)) return;
    if (!sound_voice(SOUND_SCRIPTED)) return;
    free_voice(2);
    voice = sound_voice(SOUND_SCRIPTED);
    wr_u32(voice + VOICE_DELAY, 0);
    wr_u32(voice + VOICE_PROGRAM, SCRIPTED_SOUND_PROGRAM);
    wr_u32(voice + VOICE_POSITION, 0);
    wr_u32(voice + VOICE_DELAY, 1);
    play_sound(SOUND_SCRIPTED, 2, volume);
}

void play_main_engine(int32_t period, int32_t volume) {
    gaddr low, high;
    if (!(rd_u8(SOUND_FLAGS) & 0x01)) {
        play_engine(period, volume >> 2);
        return;
    }
    if (!sound_voice(1)) return;
    high = sound_voice(1);
    low = sound_voice(0);
    wr_u32(high + VOICE_VOLUME_SLIDE, 0);
    wr_u32(low + VOICE_VOLUME_SLIDE, 0);
    wr_u32(high + VOICE_PERIOD_SLIDE, 0);
    wr_u32(low + VOICE_PERIOD_SLIDE, 0);
    wr_u32(low + VOICE_PERIOD, (uint32_t)period << 16);
    wr_u32(high + VOICE_PERIOD, (uint32_t)(period + 2) << 16);
    play_sound(0, 0, volume);
    play_sound(1, 1, volume);
}

void slide_main_engine(int32_t period, int32_t volume, int32_t ticks) {
    slide_engine(period, (rd_u8(SOUND_FLAGS) & 0x01) ? volume : volume >> 2, ticks);
}

void play_tone(int32_t kind, int32_t pitch) {
    int32_t args[9] = {0x12C, 0, 1, 0x12C, 0, 1, 1, 0, 0};
    if ((int8_t)rd_u8(TONE_MUTE) > 0) return;
    args[1] = pitch;
    args[4] = pitch;
    args[7] = kind;
    args[8] = kind;
    play_programmed_sound(args);
}

void play_tone_2(void) {
    play_tone(2, 2);
}

void play_context_tone_4(int16_t kind) {
    int32_t args[9] = {0x12C, 4, 1, 0x12C, 4, 1, 1, kind, kind};
    if (!rd_u8(CONTEXT_SELECT)) play_programmed_sound(args);
}

void play_status_tone(void) {
    play_tone(2, rd_u8(VOLUME_FADING) ? 2 : 4);
}

void play_status_tone_outside_context(void) {
    if (!rd_u8(CONTEXT_SELECT)) play_status_tone();
}

/* Add a slide to its value (each tick). */
static void apply_slide(gaddr voice, int value, int slide) {
    wr_u32(voice + (gaddr)value, rd_u32(voice + (gaddr)value) + rd_u32(voice + (gaddr)slide));
}

void update_voices(void) {
    int channel;
    for (channel = 0; channel < 4; channel++) {
        gaddr record = rd_u32(VOICE_TABLE + (gaddr)(4 * channel));
        gaddr slot = rd_u32(record + 4);
        gaddr voice = rd_u32(slot);
        if (!voice) continue;
        step_voice_program(voice, slot, channel);
        set_voice_output(rd_u32(record), voice);
        apply_slide(voice, VOICE_PERIOD, VOICE_PERIOD_SLIDE);
        apply_slide(voice, VOICE_VOLUME, VOICE_VOLUME_SLIDE);
        if (rd_u32(voice + VOICE_PERIOD_TICKS)) {
            wr_u32(voice + VOICE_PERIOD_TICKS, rd_u32(voice + VOICE_PERIOD_TICKS) - 1);
            if (!rd_u32(voice + VOICE_PERIOD_TICKS)) wr_u32(voice + VOICE_PERIOD_SLIDE, 0);
        }
        if (rd_u32(voice + VOICE_VOLUME_TICKS)) {
            wr_u32(voice + VOICE_VOLUME_TICKS, rd_u32(voice + VOICE_VOLUME_TICKS) - 1);
            if (!rd_u32(voice + VOICE_VOLUME_TICKS)) wr_u32(voice + VOICE_VOLUME_SLIDE, 0);
        }
    }
}

void free_all_voices(void) {
    int channel;
    for (channel = 0; channel < 4; channel++) free_voice(channel);
}

void sound_chosen_record_alert(int32_t volume) {
    gaddr voice = rd_u32(ALERT_VOICE);
    if (!(rd_u8(SOUND_FLAGS) & 4) || rd_u16(CHOSEN_RECORD) != rd_u16(VIEW_RECORD) || !voice) return;
    wr_u32(voice + 8, 0x1360000);
    wr_u32(voice + 0x18, 0xFFFE0000u);
    wr_u32(voice + 0x10, 1);
    play_sound(5, 2, volume);
}
