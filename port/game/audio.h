#ifndef FA18_GAME_AUDIO_H
#define FA18_GAME_AUDIO_H

#include "memory.h"

/* Voice records hold 16.16 fixed-point pitch and volume; the hardware gets
 * their integer parts. */
enum {
    VOICE_PERIOD = 0x08, /* long: Paula period, 16.16 */
    VOICE_VOLUME = 0x0C  /* long: volume 0-63, 16.16 */
};

/* Paula channel registers relative to the channel base ($DFF0A0 + 16*n). */
enum { AUD_PERIOD = 0x06, AUD_VOLUME = 0x08 };

#define PAULA_MIN_PERIOD 124

/* Write a voice's period and volume to its channel, limiting the period to
 * what Paula can play and the volume to the master volume. */
void set_voice_output(gaddr channel, gaddr voice);

/* Move the master volume a quarter step toward its target while a fade is
 * active. */
void fade_master_volume(void);

enum { VOICE_INTERRUPT = 0x14 }; /* word: this channel's INTREQ bits */

/* Clear a channel's audio interrupt request (INTREQ bits from its voice). */
void clear_voice_interrupt(int channel);

#endif
