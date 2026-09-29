#ifndef FA18_GAME_AUDIO_H
#define FA18_GAME_AUDIO_H

#include "memory.h"

/* Voice records hold 16.16 fixed-point pitch and volume; the hardware gets
 * their integer parts. */
enum {
    VOICE_PERIOD = 0x08,       /* long: Paula period, 16.16 */
    VOICE_VOLUME = 0x0C,       /* long: volume 0-63, 16.16 */
    VOICE_PERIOD_SLIDE = 0x18, /* long: added to the period each tick */
    VOICE_VOLUME_SLIDE = 0x1C, /* long: added to the volume each tick */
    VOICE_PERIOD_TICKS = 0x38, /* long: ticks the period slide lasts */
    VOICE_VOLUME_TICKS = 0x3C  /* long: ticks the volume slide lasts */
};

/* Sounds with dedicated routines. */
enum {
    SOUND_ENGINE_LOW = 2, SOUND_ENGINE_HIGH = 3, SOUND_PROGRAMMED = 4,
    SOUND_NOISE_LOW = 8, SOUND_NOISE_HIGH = 9, SOUND_SCRIPTED = 11
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

/* Voice program: a list of (offset, value) long pairs run when the voice
 * delay expires. An offset below $2C stores the value into that voice
 * field; $2C waits `value` ticks (0 ends the program); an offset of $40 + n
 * is a loop on counter n: jump to `value` while the counter, decremented,
 * is not zero (a zero counter always jumps). */
enum { VOICE_LOOP_COUNTERS = 0x24, VOICE_DELAY = 0x2C, VOICE_PROGRAM = 0x30, VOICE_POSITION = 0x34 };

/* Advance a voice program; when it ends, free `slot` and clear the
 * channel interrupt. */
void step_voice_program(gaddr voice, gaddr slot, int channel);

/* Free the voice slot of a channel and clear its interrupt. */
void free_voice(int channel);
/* All four ($C0F4A6). */
void free_all_voices(void);

/* While the alert tone is enabled and the chosen record is the one viewed:
 * sound 5 on channel 2 from ALERT_VOICE's set-up ($C1803C). */
void sound_chosen_record_alert(int32_t volume);

/* Stop whatever channel 2 is playing. */
void stop_channel_2(void);

/* Play sound `sound` on `channel` at `volume` (0-63): its voice record
 * takes over the channel. Nothing happens for a sound without a record. */
void play_sound(int sound, int channel, int32_t volume);

/* Choose the menu's two-channel sound pair from the mode flags ($C17B96). */
void start_menu_sound_pair(int32_t volume);

/* The alert tone on channel 2, when enabled and the view shows the scripted
 * record. */
void play_alert_tone(int32_t volume);

/* The engine pair (sounds 2 and 3 on channels 0 and 1) at `period` and
 * period + 2, when enabled (SOUND_FLAGS bit 1) and sound 3 exists
 * ($C17CF6). Any slides stop. */
void play_engine(int32_t period, int32_t volume);

/* Slide the engine pair from its current period and volume to `period` and
 * `volume` over `ticks` ticks ($C17DAA). */
void slide_engine(int32_t period, int32_t volume, int32_t ticks);

/* The noise pair (sounds 8 and 9 on channels 0 and 1) at random periods
 * $168-$1A7 when SOUND_FLAGS bit 4 is set; otherwise channels 0 and 1 are
 * freed ($C17E4A). */
void play_noise(int32_t volume);

/* Sound 4 on channel 3 with its program's values set from the arguments
 * ($C17EF2): nine longs, see the definition. */
void play_programmed_sound(const int32_t args[9]);

/* Sound 11 on channel 2 with the program at SCRIPTED_SOUND_PROGRAM, when
 * enabled (SOUND_FLAGS bit 6) and the view shows the scripted record
 * ($C18096). */
void play_scripted_sound(int32_t volume);

/* The main engine pair (sounds 0 and 1 on channels 0 and 1) at `period`
 * and period + 2 when SOUND_FLAGS bit 0 is set and sound 1 exists;
 * otherwise play_engine at a quarter of the volume ($C17C62). */
void play_main_engine(int32_t period, int32_t volume);

/* slide_engine, the volume quartered unless SOUND_FLAGS bit 0 is set
 * ($C17D6E). */
void slide_main_engine(int32_t period, int32_t volume, int32_t ticks);

/* A short programmed tone of variant `kind` at `pitch`, unless TONE_MUTE
 * is positive ($C3319A; $C3316A plays kind 1). */
void play_tone(int32_t kind, int32_t pitch);

/* Tone 2 at pitch 2 ($C33180). */
void play_tone_2(void);

/* Tone 2 at pitch 2 while the volume fades, else at pitch 4 ($C3318E). */
void play_status_tone(void);

/* play_status_tone unless a context runs ($C33186). */
void play_status_tone_outside_context(void);

/* The audio interrupt server's work ($C50158): for each channel with a
 * voice, step its program, write it to Paula, and apply its period and
 * volume slides, each stopping when its tick count runs out. */
void update_voices(void);

#endif
