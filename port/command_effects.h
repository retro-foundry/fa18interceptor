#ifndef FA18_COMMAND_EFFECTS_H
#define FA18_COMMAND_EFFECTS_H

#include "command_dispatch.h"
#include "voice_program.h"

/* Shared command/audio-update voice values. Sample ownership is separate;
 * command effects and the update loop write the same ordinary voice object. */
typedef PortVoice FA18CommandVoice;

/* Mutable values of the original sound-program instructions, in order.
 * Opcodes and sample data belong to the asset/audio owners. This is game
 * sound-program data, with no instruction pointer or CPU address space. */
typedef PortVoiceProgramValues FA18CommandSoundProgram;

typedef struct {
    FA18CommandVoice *programmed_voice, *sweep_voice, *slots[4];
    FA18CommandSoundProgram programmed, sweep;
    uint32_t random_seed;
    uint8_t volume_fading, tone_mute, effect_flags, sound6_mode;
    uint8_t sound_flags;
    uint16_t interrupt_masks[4];
    uint32_t master_volume, master_volume_target;
    /* Actual host audio acknowledgement; invoked even for an empty slot.
     * Import masks from the original channel descriptors. No bus is used. */
    void (*acknowledge)(void *context, unsigned channel, uint16_t mask);
    void *acknowledge_context;
} FA18CommandAudio;

typedef struct {
    FA18ContextCommandState *context;
    FA18CommandAudio *audio;
    uint16_t message_code, message_state;
    FA18FlightCommandOps flight_ops;
    FA18ContextCommandOps context_ops;
} FA18CommandEffects;

/* Bind the two queue-reachable audio fields to their sole owners, importing
 * their current values. Queue, effects and audio must stay at stable addresses.
 * Caller imports message code/state, voices, program values, audio gates,
 * seed and channel masks; this initializer preserves those values except
 * the two fields imported from the queue. Requires initialization before
 * installing owners; keep the audio service and program imports valid.
 * Validates the original mutable program data and required host audio service.
 * Preserve the caller's three pose/span imports when installing the owners. */
int fa18_initialize_command_effects(FA18CommandEffects *effects,
                                    FA18ContextCommandState *context,
                                    FA18CommandQueue *queue,
                                    FA18CommandAudio *audio);
int fa18_install_command_effect_owners(FA18CommandEffects *effects,
                                       FA18NativeCommandOwners *owners);

/* Real game children: message post, space press, status tone, sound-6 sweep
 * and all-voice release. Events preserve the original full-word effects.
 * Return 0 for invalid data. Source writes before a failure remain applied. */
int fa18_post_native_command_message(FA18CommandEffects *effects,
                                      uint32_t event, uint32_t *result);
int fa18_press_native_space_command(FA18FlightCommandState *flight);
int fa18_start_native_sound6(FA18CommandEffects *effects, uint32_t event,
                             int32_t period, int32_t ticks, uint32_t *result);
int fa18_play_native_status_tone(FA18CommandAudio *audio,
                                  uint32_t *result);
int fa18_release_native_command_voices(FA18CommandAudio *audio,
                                        uint32_t *result);

#endif
