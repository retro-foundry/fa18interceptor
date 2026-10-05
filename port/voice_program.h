#ifndef PORT_VOICE_PROGRAM_H
#define PORT_VOICE_PROGRAM_H

#include <stddef.h>
#include <stdint.h>

/* Portable tick-driven fixed-point voices. These named sound operations are
 * independent of game state, asset addresses, CPU state and audio hardware.
 * Imported programs use eight-byte (operation,value) cursor units. */
enum { PORT_VOICE_INSTRUCTION_BYTES=8 };
typedef enum {
    PORT_VOICE_SET_PERIOD, PORT_VOICE_SET_VOLUME,
    PORT_VOICE_SET_PERIOD_SLIDE, PORT_VOICE_SET_VOLUME_SLIDE,
    PORT_VOICE_SET_LOOP0, PORT_VOICE_SET_LOOP1,
    PORT_VOICE_WAIT, PORT_VOICE_LOOP0, PORT_VOICE_LOOP1
} PortVoiceOperation;
typedef struct {
    uint32_t *values;
    size_t count;
} PortVoiceProgramValues;
typedef struct {
    const PortVoiceOperation *operations;
    PortVoiceProgramValues data;
} PortVoiceProgram;
typedef struct {
    uint32_t period, volume, position, delay;
    uint32_t period_slide, volume_slide, loop_counters[2];
    uint32_t period_ticks, volume_ticks;
    const PortVoiceProgram *program;
} PortVoice;

typedef enum {
    PORT_VOICE_OK, PORT_VOICE_INVALID_ARGUMENT, PORT_VOICE_INVALID_PROGRAM
} PortVoiceResult;

/* Delay zero is idle. An expiring delay executes until a wait; wait zero
 * clears the supplied slot before calling ended, even if the slot aliases
 * another voice. Loop counters wrap, and zero means an unconditional jump.
 * Partial source-ordered writes remain on an invalid cursor/operation/jump.
 * No implicit terminator, instruction limit or alternate program is supplied. */
PortVoiceResult port_step_voice_program(PortVoice *voice, PortVoice **slot,
                                        void (*ended)(void *context), void *context);
/* Apply slides once, then expire their nonzero durations, using 32-bit wrap. */
void port_advance_voice_slides(PortVoice *voice);

#endif
