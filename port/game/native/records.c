/* Native composition of the source C1C63E -> C22C80 update path. */
#include "records.h"
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
#include <stdio.h>
#include <stdlib.h>

static FlightActionState action_child(void *context,enum FlightActionChild child) {
    (void)context;
    fprintf(stderr,"native record action child unavailable: %u\n",(unsigned)child); abort();
}
static void dynamics(gaddr record) {
    RecordDynamicsFrame frame={0};
    frame.work.record=record;
    const DynamicsHooks hooks={0};
    while(!advance_record_dynamics(&frame,&hooks)) {
        DynamicsState *w=&frame.work;
        switch(frame.child) {
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
