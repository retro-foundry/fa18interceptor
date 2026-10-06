/* Native composition of the source C1C63E -> C22C80 update path. */
#include "records.h"
#include "clock.h"
#include "../globals.h"
#include "../update_stage.h"
#include "../record_update_stage.h"
#include "../flight_record_actions.h"
#include "../flight_dynamics.h"
#include "../flight_motion_helpers.h"
#include "../record_region_probe.h"
#include "../selector_origin.h"
#include "../candidate_record_update.h"
#include "../control_records.h"
#include "../fixed_math.h"
#include "../postflight_scheduler.h"
#include "../main_loop_flight_controls.h"
#include "../matrix.h"
#include "../view.h"
#include "../menu_context_finish.h"
#include "../audio.h"
#include "../messages.h"
#include "../fault.h"
#include <stdio.h>
#include <stdlib.h>

static FlightActionState action_child(void *context,enum FlightActionChild child) {
    (void)context;
    fprintf(stderr,"native record action child unavailable: %u\n",(unsigned)child); abort();
}
static DynamicsState region_child(void *context,enum DynamicsChild child,DynamicsState work) {
    (void)context;
    if(child==DY_REGION_ENTER || child==DY_REGION_ACTIVE) {
        const DynamicsHooks hooks={.consume_values=region_child};
        return spawn_region_records(work,&hooks);
    }
    fprintf(stderr,"native scene region child unavailable: %u\n",(unsigned)child); abort();
}
static FlightWorking root_control_child(void *context,enum FlightChild child,FlightWorking w) {
    (void)context;
    switch(child) {
    case FC_NORMALISE_CONTROL: {
        const gaddr frame=0x4700;
        wr_u32(frame+8,w.value);wr_u32(frame+12,w.z);
        wr_u32(frame+16,w.rate);wr_u32(frame+20,w.depth);
        const FlightHooks hooks={.consume_values=root_control_child};
        normalise_main_loop_control_vector(frame,&hooks);break;
    }
    case FC_NORMALISE_LENGTH: {
        int16_t x=(int16_t)w.z,y=(int16_t)w.rate,z=(int16_t)w.depth;
        if(x<0) x=(int16_t)-x;if(y<0) y=(int16_t)-y;if(z<0) z=(int16_t)-z;
        w.speed=(uint32_t)magnitude3(x,y,z);w.zero=w.speed==0;break;
    }
    case FC_ATTENUATE_X: case FC_ATTENUATE_Y: case FC_ATTENUATE_Z:
        w.value=(uint32_t)(int32_t)attenuate_offset((int16_t)w.value,(int16_t)w.speed);break;
    case FC_PROBE_CONTROL: probe_record_regions(NULL);break;
    case FC_TOUCHDOWN_FAST_TONE: case FC_TOUCHDOWN_SLOW_TONE:
        sound_chosen_record_alert(child==FC_TOUCHDOWN_FAST_TONE?40:30);break;
    case FC_RESET_CONTROL:
        begin_mission_reset();break; /* C083E2 after landing on the carrier. */
    case FC_SAMPLE_TOUCHDOWN: case FC_SAMPLE_TAKEOFF: {
        native_clock_sample();break;
    }
    default: fprintf(stderr,"native root control child unavailable: %u\n",(unsigned)child);abort();
    }
    return w;
}
static void dynamics(gaddr record) {
    RecordDynamicsFrame frame={0};
    frame.work.record=record;
    const DynamicsHooks hooks={0};
    while(!advance_record_dynamics(&frame,&hooks)) {
        DynamicsState *w=&frame.work;
        switch(frame.child) {
        case DY_SELECTED_RECORD: {
            ZoneExitFrame zone={0};
            zone.work=(GeometryState){w->primary,w->detail,w->x,w->y,w->z,w->rate_x,w->rate_y,w->rate_z,
                w->root,w->record,w->geometry,w->scene,w->table,w->face,w->child_equal};
            if(!update_dynamics_record_zone_exit(&zone,NULL)) {
                fprintf(stderr,"native record zone child unavailable: %u\n",(unsigned)zone.phase); abort();
            }
            const GeometryState v=zone.work;
            *w=(DynamicsState){v.primary,v.detail,v.x,v.y,v.z,v.rate_x,v.rate_y,v.rate_z,
                v.root,v.record,v.geometry,v.scene,v.table,v.face,v.child_equal};
            break;
        }
        case DY_RECORD_CONTROLS:
            update_dynamics_record_input(record,w->primary); break;
        case DY_DESCENT_ALERT: case DY_RECORD_ALERT: case DY_COLLISION_MESSAGE:
            post_message((uint16_t)w->primary);
            /* C25704 returns the posted message's classification byte. */
            w->primary&=0xffffff00u; break;
        case DY_COLLISION_SOUND:
            /* C260EC-C260F8: C17F8C receives period $1C and duration $30.
             * The source voice gate owns both audio and FIRE_STATE effects. */
            start_sound_6(0x1c,0x30);break;
        case DY_COLLISION_FAULT:
            fault_hook();break; /* C06C02 is empty in the release executable. */
        case DY_RECORD_SELECTOR: {
            IndexedRecordWork selected={0};update_dynamics_selected_record(&selected);
            w->primary=rd_u16(selected.record+0x6e);
            w->detail=(w->detail&0xffff0000u)|rd_u16(selected.record+0x6c);
            w->root=selected.record;break;
        }
        case DY_ROOT_FLIGHT: {
            const FlightHooks flight={.consume_values=root_control_child};
            advance_main_loop_flight_controls(0x4800,&flight);break;
        }
        case DY_CELL_MATRIX:
            inverse_orientation_matrix(record,(uint16_t)w->rate_x,(uint16_t)w->rate_y,(uint16_t)w->rate_z);
            break;
        case DY_RECORD_MATRIX: {
            const RecordMatrixInput input={record,{w->primary,w->detail,w->x,w->y,w->z,w->rate_x,w->rate_y,w->rate_z}};
            RecordMatrixResult result={0};
            update_dynamics_record_matrix(&input,&result,NULL,NULL);
            break;
        }
        case DY_MOTION_CANDIDATE: {
            CandidateUpdateWork candidate={0};
            w->primary=(uint32_t)update_candidate_record(&candidate,(int32_t)w->x,(int32_t)w->y,(int32_t)w->z);
            w->child_equal=w->primary==0; break;
        }
        case DY_RECORD_TIMER: {
            record_position_history(); break;
        }
        case DY_GROUND_PROJECTION: {
            MotionState motion={w->primary,w->detail,w->x,w->y,w->z,w->rate_x,w->rate_y,w->rate_z,
                                w->root,w->record,w->geometry,w->scene,w->table,w->face,w->child_equal};
            motion=project_record_motion(motion,NULL);
            w->primary=motion.value; w->detail=motion.selector;
            w->x=motion.x; w->y=motion.y; w->z=motion.z;
            break;
        }
        case DY_REGION_PROBE: probe_record_regions(NULL); break;
        case DY_MOTION_SLOT: {
            const MotionState motion={w->primary,w->detail,w->x,w->y,w->z,w->rate_x,w->rate_y,w->rate_z,
                                      w->root,w->record,w->geometry,w->scene,w->table,w->face,w->child_equal};
            publish_motion_slot(motion,NULL); break;
        }
        default: fprintf(stderr,"native record dynamics child unavailable: %u\n",(unsigned)frame.child); abort();
        }
    }
}
static PostflightScheduleResult schedule_child(void *context,enum PostflightScheduleChild child,gaddr record) {
    (void)context; (void)record;
    if(child==SCHEDULE_SELECTION_GATE) { release_lost_selection(); return (PostflightScheduleResult){0,1}; }
    if(child==SCHEDULE_NINE) {
        schedule_postflight(POSTFLIGHT_MODE_NINE,0,record,NULL);
        return (PostflightScheduleResult){0,1};
    }
    fprintf(stderr,"native record schedule child unavailable: %u\n",(unsigned)child); abort();
}
typedef struct { gaddr companion; } RecordLoop;
static void record_event(void *context,const RecordUpdateEvent *event) {
    RecordLoop *loop=context;
    if(event->phase==RECORD_UPDATE_ROOT ||
       (event->phase==RECORD_UPDATE_SLOT && !event->other)) loop->companion=event->companion;
}
static FlightWorking flight_child(void *context,enum FlightChild child) {
    (void)context;
    fprintf(stderr,"native record dispatch child unavailable: %u\n",(unsigned)child); abort();
}
static int record_child(void *context,enum RecordUpdateChild child,unsigned slot) {
    const RecordLoop *loop=context;
    gaddr record=CONTROL_RECORDS+512u*slot;
    const FlightActionHooks actions={action_child,NULL,NULL,NULL};
    FlightActionState work={0}; work.record=record; work.source=loop->companion;
    switch(child) {
    case RECORD_UPDATE_PERIODIC: {
        const DynamicsHooks regions={.consume_values=region_child};
        update_scene_regions((DynamicsState){0},&regions); return 0;
    }
    case RECORD_UPDATE_RELEASE_SELECTION: release_lost_selection(); return 0;
    case RECORD_UPDATE_ROOT_CONTROL: advance_flight_record_control(work,&actions); return 0;
    case RECORD_UPDATE_ROOT_VIEW: {
        RecordViewUpdateWork view={0}; update_record_view(record,0,0,&view); return 1;
    }
    case RECORD_UPDATE_ROOT_MARKER: classify_selected_record_range(record); return 0;
    case RECORD_UPDATE_POSE: dynamics(record); return 0;
    case RECORD_UPDATE_PRIMARY_READY: return select_flight_record_action(work,1,&actions);
    case RECORD_UPDATE_SECONDARY_READY: return select_flight_record_action(work,0,&actions);
    case RECORD_UPDATE_PAIRED_READY: return paired_record_ready(record);
    case RECORD_UPDATE_DISPATCH: {
        FlightWorking flight={0}; flight.record=record; flight.auxiliary=loop->companion;
        const FlightHooks hooks={flight_child,NULL,NULL,NULL};
        flight=advance_main_loop_flight_record(flight,&hooks);
        return flight.value!=0;
    }
    case RECORD_UPDATE_FINISH: {
        const PostflightScheduleHooks hooks={schedule_child,NULL,NULL};
        schedule_postflight(POSTFLIGHT_DISPATCH,0,record,&hooks); return 0;
    }
    default: fprintf(stderr,"native control record child unavailable: %u (slot %u)\n",(unsigned)child,slot); abort();
    }
}
static SelectorOriginTriple origin_child(void *context,enum SelectorOriginChild child,const SelectorOriginTriple *input) {
    (void)context;
    if(child==ORIGIN_PREPARE) { update_view_matrix(); return (SelectorOriginTriple){{0,0,0}}; }
    if(child==ORIGIN_NORMALIZE) {
        NormalizedVectorState state={0}; state.scale=0x200;
        state.x=input->component[0]; state.y=input->component[1]; state.z=input->component[2];
        state=normalize_record_vector(state,NULL,NULL);
        return (SelectorOriginTriple){{state.x,state.y,state.z}};
    }
    if(child==ORIGIN_MATRIX_A || child==ORIGIN_MATRIX_B) {
        gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(VIEW_RECORD);
        if(child==ORIGIN_MATRIX_A) update_view_matrix();
        int32_t out[3];
        local_to_world(record,child==ORIGIN_MATRIX_A?VIEW_MATRIX:record+RECORD_INVERSE,
            (int16_t)input->component[0],(int16_t)input->component[1],(int16_t)input->component[2],out);
        return (SelectorOriginTriple){{(uint32_t)out[0],(uint32_t)out[1],(uint32_t)out[2]}};
    }
    if(child==ORIGIN_REGENERATE) {
        int32_t out[3]; start_position(out);
        return (SelectorOriginTriple){{(uint32_t)out[0],(uint32_t)out[1],(uint32_t)out[2]}};
    }
    fprintf(stderr,"native record origin child unavailable: %u\n",(unsigned)child); abort();
}
void native_control_records_update(void) {
    RecordLoop loop={0};
    const RecordUpdateHooks records={record_child,record_event,&loop};
    update_control_records(&records);
}
void native_records_update(void) {
    RecordUpdateStageFrame frame={0};
    RecordLoop loop={0};
    const UpdateStageHooks stage={0};
    const RecordUpdateHooks records={record_child,record_event,&loop};
    while(!advance_record_update_stage(&frame,&stage)) {
        if(frame.child==UPDATE_STAGE_RECORDS) {
            update_control_records(&records);
            frame.result=(UpdateStageResult){CONTROL_RECORDS,frame.requests};
        } else {
            const SelectorOriginHooks origin={origin_child,NULL,NULL};
            publish_selector_origin(&origin);
            frame.result=(UpdateStageResult){CONTROL_RECORDS,frame.requests};
        }
    }
}
