/* The menu sound selector calls the already recreated voice routines. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "globals.h"
#include "memory.h"

void play_sound_registers(uint32_t sound, uint32_t channel);

int glue_C17B96(void) {
    uint8_t fading = rd_u8(VOLUME_FADING);
    uint8_t flags = rd_u8(SOUND_FLAGS);
    uint8_t mode = rd_u8(SOUND_FLAGS - 1);
    int32_t volume = (int32_t)rd_u32(A(7) + 4);

    start_menu_sound_pair(volume);
    if (fading) {
        SET_B(D(0), fading);
    } else if (flags & 0x80) {
        D(0) = 12;
        A(0) = rd_u32(VOICE_TABLE + 12);
        play_sound_registers(13, 0);
        play_sound_registers(14, 1);
    } else if (mode & 0x04) {
        play_sound_registers(35, 0);
        play_sound_registers(36, 1);
    } else {
        D(0) = 12;
        A(0) = rd_u32(VOICE_TABLE + 12);
    }
    return glue_return();
}
