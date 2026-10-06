#ifndef FA18_CONTROL_ACTIONS_H
#define FA18_CONTROL_ACTIONS_H
#include "memory.h"
/* Typed C12950 sound requests, with the original argument order. */
enum ControlSoundKind { CONTROL_SCRIPT,CONTROL_STOP_SCRIPT,CONTROL_PROGRAM,
    CONTROL_FREE_VOICE,CONTROL_EVENT_SIX,CONTROL_EVENT_DISPATCH,
    CONTROL_ENGINE,CONTROL_MAIN_ENGINE,CONTROL_ENGINE_SLIDE,
    CONTROL_MAIN_ENGINE_SLIDE,CONTROL_NOISE };
typedef struct {
    enum ControlSoundKind kind;
    unsigned count;
    int32_t argument[9];
} ControlSoundCall;
typedef void (*ControlSoundSink)(void *context,const ControlSoundCall *call);
void consume_control_sound(const ControlSoundCall *call);
/* Original C131BE/C133B2/C12950 semantics with ordinary arguments/locals.
 * The instruction-observable reference owners remain in control_readouts.c. */
int16_t control_sound_magnitude(gaddr record,int16_t phase);
int16_t control_sound_step(gaddr record);
void update_control_actions(ControlSoundSink sink,void *context);
#endif
