/* $C13176 reads the low words of two long stack arguments. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "globals.h"
#include "memory.h"

void play_sound_registers(uint32_t sound, uint32_t channel);

int glue_C13176(void) {
    int16_t period = rd_s16(A(7) + 6);
    int16_t volume = rd_s16(A(7) + 10);
    int event = (rd_u8(SOUND_FLAGS - 1) & 2) != 0;
    int active = event ? (rd_u32(0xC0A468u) != 0)
                       : ((rd_u8(SOUND_FLAGS) & 2) &&
                          rd_u32(SOUND_VOICES + 4 * SOUND_ENGINE_HIGH));

    dispatch_event_sound(period, volume);
    if (event) {
        D(0) = (uint32_t)(int32_t)volume;
        if (active) {
            D(1) = 16;
            play_sound_registers(12, 2);
        }
    } else {
        D(0) = (uint32_t)((int32_t)period + 22);
        D(1) = (uint32_t)(int32_t)volume;
        if (active) play_sound_registers(SOUND_ENGINE_HIGH, 1);
    }
    return glue_return();
}
