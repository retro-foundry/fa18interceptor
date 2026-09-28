/* Sound output levels. */
#include "audio.h"

#include "globals.h"

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
