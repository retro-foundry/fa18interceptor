/* $C13176 reads the low words of two long stack arguments. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "globals.h"
#include "memory.h"

void play_sound_registers(uint32_t sound, uint32_t channel);

/* Complete C13176 is in glue_control_readouts.c. */

int glue_C3316E(void) {
    play_context_tone_4((int16_t)D(0));
    return glue_return();
}
