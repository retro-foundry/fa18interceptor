/* C1518C -> C15688/C159AE/C15AD4 and C153FC: flight control effects.
 * All frame storage below is native scratch, separate from game records. */
#include "control_effects.h"
#include "../main_loop_control_messages.h"
#include "../record_control_actions.h"
#include "../flight_record_actions.h"
#include "../flight_dynamics.h"
#include "../flight_motion_helpers.h"
#include "../corner_view.h"
#include "../render_line.h"
#include "../audio.h"
#include "../fixed_math.h"
#include <stdio.h>
#include <stdlib.h>

enum { CONTROL_FRAME=0x4500,ACTION_FRAME=0x4600,AIM_FRAME=0x4900,
       DIRECTION_FRAME=0x4940,NORMAL_FRAME=0x4980,COLLISION_FRAME=0x4a00 };
static uint32_t action_child(void *context,enum RecordActionChild child);
static gaddr collision_frame(void *context) {(void)context;return COLLISION_FRAME;}
static DynamicsState collision_child(void *context,enum DynamicsChild child,DynamicsState w) {
    (void)context;(void)w;
    fprintf(stderr,"native control collision child unavailable: %u\n",(unsigned)child);abort();
}
static FlightActionState direction_child(void *context,enum FlightActionChild child,FlightActionState w) {
    (void)context;
    if(child==FA_DIRECTION_LENGTH) {
        w.selector=(uint32_t)magnitude3((int16_t)w.detail,(int16_t)w.x,(int16_t)w.y);return w;
    }
    fprintf(stderr,"native control direction child unavailable: %u\n",(unsigned)child);abort();
}
static uint32_t action_child(void *context,enum RecordActionChild child) {
    const int16_t index=*(const int16_t *)context;
    const RecordActionHooks hooks={action_child,NULL,context};
    switch(child) {
    case RA_INITIALISE_DIRECTION:
        wr_u32(AIM_FRAME+8,(uint32_t)(int32_t)index);
        aim_control_record_action(AIM_FRAME,&hooks);break;
    case RA_TRANSFORM_DIRECTION:
        wr_u32(DIRECTION_FRAME+8,(uint32_t)(int32_t)rd_s16(AIM_FRAME-2));
        for(unsigned i=0;i<3;++i)
            wr_u32(DIRECTION_FRAME+12+4*i,(uint32_t)(int32_t)rd_s16(AIM_FRAME-20-2*i));
        publish_control_record_direction(DIRECTION_FRAME,&hooks);break;
    case RA_PROJECT_DIRECTION: {
        for(unsigned i=0;i<4;++i) wr_u32(NORMAL_FRAME+8+4*i,rd_u32(DIRECTION_FRAME+8+4*i));
        const FlightActionHooks direction={.context=context,.consume_values=direction_child};
        normalise_flight_record_direction(NORMAL_FRAME,&direction);break;
    }
    case RA_CHECK_RECORD: {
        wr_u32(COLLISION_FRAME+8,(uint32_t)(int32_t)index);
        const DynamicsHooks collision={.frame=collision_frame,.context=context,.consume_values=collision_child};
        return collide_scene_motion((DynamicsState){0},&collision).primary;
    }
    case RA_CHECK_GROUND:
        wr_u32(COLLISION_FRAME+8,(uint32_t)(int32_t)index);
        project_scene_motion((MotionState){0},COLLISION_FRAME,NULL);return 0;
    case RA_DRAW_SPECIAL: draw_control_record(index,CONTROL_RECORD_PAIRS);break;
    case RA_DRAW_TRACKED: draw_control_record(index,CONTROL_RECORD_LAYERS);break;
    case RA_DRAW_STANDARD: draw_control_record(index,CONTROL_RECORD_POINT);break;
    default: fprintf(stderr,"native control record child unavailable: %u\n",(unsigned)child);abort();
    }
    return 0;
}
static uint32_t alert_child(void *context,enum RecordActionChild child) {
    (void)context;
    if(child==RA_RESERVE_ALERT) free_voice(2);
    else if(child==RA_START_ALERT) play_sound(10,2,0);
    else abort();
    return 0;
}
static MessageWorking control_child(void *context,enum MainControlChild child) {
    (void)context;
    switch(child) {
    case MC_RESET_FACE_STATE: reset_line_style();break;
    case MC_CONTROL_TONE: case MC_CONTROL_FALLBACK_TONE: start_sound_6(28,48);break;
    case MC_CONTROL_ALERT_FIRST: case MC_CONTROL_ALERT_SECOND: {
        wr_u32(ACTION_FRAME+8,6);wr_u32(ACTION_FRAME+12,child==MC_CONTROL_ALERT_FIRST?30:22);
        const RecordActionHooks alert={alert_child,NULL,NULL};
        start_control_record_alert(ACTION_FRAME,&alert);break;
    }
    case MC_BEGIN_RECORD: case MC_ADVANCE_RECORD: {
        const int16_t index=rd_s16(CONTROL_FRAME-16);
        wr_u32(ACTION_FRAME+8,(uint32_t)(int32_t)index);
        const RecordActionHooks actions={action_child,NULL,(void *)&index};
        if(child==MC_BEGIN_RECORD) initialise_control_record_action(ACTION_FRAME,&actions);
        else advance_control_record_action(ACTION_FRAME,&actions);
        break;
    }
    default: fprintf(stderr,"native control effect child unavailable: %u\n",(unsigned)child);abort();
    }
    return (MessageWorking){0};
}
void native_control_effects(void) {
    const MainControlHooks controls={.consume=control_child};
    advance_main_loop_control_records(CONTROL_FRAME,&controls);
}
