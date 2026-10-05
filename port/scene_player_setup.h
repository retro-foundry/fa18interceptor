#ifndef FA18_NATIVE_SCENE_PLAYER_SETUP_H
#define FA18_NATIVE_SCENE_PLAYER_SETUP_H
#include "native_scene_records.h"
#include "command_effects.h"

typedef struct {
    FA18NativeSceneRecords *records;
    FA18FlightCommandState *flight;
    FA18CommandEffects *effects;
    uint8_t *mission_flags_b,*mission_flags_c;
    uint8_t *player_flags[6]; /* A..F; G is flight->ecm_enabled */
    uint8_t *phase,*selection_active;
    uint16_t *limit,*selected_record,*message_shown,*message_marker;
    uint32_t *warning_causes,*event_bits;
} FA18NativeScenePlayerSetup;

/* Bind the queue-reachable phase and selection bytes to their required live
 * owners. Phase can be the actual native tick's field. All other globals
 * remain required caller-owned references, without copied scalar state. */
int fa18_bind_native_scene_player(FA18NativeScenePlayerSetup *state,FA18CommandQueue *queue);
/* Complete $C0840E, $C09620 (including its real $C0840E child), and $C095C0.
 * Writes share the aircraft/context records used by native command owners.
 * Return 1 on completion, 0 for missing owners, preserving preceding stores. */
int fa18_reset_native_mission_objects(FA18NativeScenePlayerSetup *state);
int fa18_prepare_native_scene_player(FA18NativeScenePlayerSetup *state);
int fa18_reset_native_scene_player(FA18NativeScenePlayerSetup *state);
/* Complete $C0910C, returning the actual bootstrap tuple as ordinary values.
 * The caller passes these to the existing real native observer child. */
int fa18_native_scene_start_position(uint32_t position[3]);
#endif
