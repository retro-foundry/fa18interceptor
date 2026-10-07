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
#include "../context_publication.h"
#include "../cockpit.h"
#include "../main_loop_flight_controls.h"
#include "../matrix.h"
#include "../view.h"
#include "../menu_context_finish.h"
#include "../audio.h"
#include "../messages.h"
#include "../fault.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t action_view(uint32_t event,int16_t index);
static FlightActionState action_sound_child(void *context,enum FlightActionChild child) {
    (void)context;
    if(child!=FA_SOUND_MESSAGE) abort();
    int32_t arguments[9];
    for(unsigned i=0;i<9;++i) arguments[i]=rd_s16(0xc23174u+2*i);
    play_programmed_sound(arguments); /* C23186's MOVEM.W sign extends each argument. */
    return (FlightActionState){0}; /* The enclosing selector returns its own zero. */
}
static FlightActionState action_child(void *context,enum FlightActionChild child,FlightActionState w) {
    (void)context;
    switch(child) {
    case FA_MANOEUVRE_ACTION: {
        const FlightActionHooks hooks={.consume_values=action_child};
        initialise_flight_record_manoeuvre(w,&hooks);break;
    }
    case FA_ACTION_SOUND: {
        const FlightActionHooks hooks={.consume=action_sound_child};
        queue_flight_record_action_sound(&hooks);break;
    }
    case FA_REFRESH_ACTION_VIEW:
        w.primary=action_view(w.primary,(int16_t)w.selector);break;
    case FA_ACTION_ROTATION: {
        int16_t angles[3];MatrixTransformAngleState state;
        build_transform_product(w.coefficients,(uint16_t)w.primary,(uint16_t)w.detail,(uint16_t)w.y);
        extract_transform_angles(angles,&state);
        w.y=(uint32_t)(int32_t)angles[0];w.z=(uint32_t)(int32_t)angles[1];
        w.product_a=(uint32_t)(int32_t)angles[2];break;
    }
    case FA_ACTION_MATRIX:
        set_record_orientation(w.record,(uint16_t)w.y,(uint16_t)w.z,(uint16_t)w.product_a);break;
    case FA_MAGNITUDE_ALERT: play_context_tone_4((int16_t)w.primary);break; /* C3316E */
    case FA_BEGIN_STREAM: case FA_NEXT_STREAM: {
        const FlightActionHooks hooks={.consume_values=action_child};
        select_next_flight_record_stream(w,&hooks); break; /* C23578 */
    }
    case FA_STREAM_END_MESSAGE: case FA_STREAM_LIMIT_MESSAGE: case FA_NEXT_MESSAGE:
        post_message((uint16_t)w.primary); w.primary&=0xffffff00u; break; /* C25704 */
    case FA_ACTION_NORMALISE: {
        NormalizedVectorState v={w.primary,w.selector,w.detail,w.x,w.y,w.z,w.product_a,w.product_b,0,0};
        v=normalize_record_vector(v,NULL,NULL);
        w.primary=v.scale;w.selector=v.length;w.detail=v.shift;w.x=v.planar_factor;
        w.y=v.height_ratio;w.z=v.x;w.product_a=v.y;w.product_b=v.z;break;
    }
    default: fprintf(stderr,"native record action child unavailable: %u\n",(unsigned)child);abort();
    }
    return w;
}
static DynamicsState region_child(void *context,enum DynamicsChild child,DynamicsState work) {
    (void)context;
    if(child==DY_REGION_ENTER || child==DY_REGION_ACTIVE) {
        const DynamicsHooks hooks={.consume_values=region_child};
        return spawn_region_records(work,&hooks);
    }
    if(child==DY_PLACE_RECORD || child==DY_REGION_RELEASE || child==DY_REGION_REPLACE) {
        set_record_view(work.root,(int16_t)work.x,(int16_t)work.y,
            (int16_t)work.z,(int16_t)work.rate_x,work.rate_y); /* C28F16 */
        return work;
    }
    if(child==DY_ORIENT_RECORD) {
        set_record_orientation(work.record,(uint16_t)work.z,
            (uint16_t)work.rate_x,(uint16_t)work.rate_y); /* C2D954 */
        return work;
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
    case FC_REQUEST_CONTROL: {
        const ContextPublicationHooks request={0};
        set_selected_record_request(1,&request);break; /* C083A6 */
    }
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
            while(!update_dynamics_record_zone_exit(&zone,NULL)) {
                if(zone.phase==ZONE_AFTER_PLACE) {
                    GeometryState *v=&zone.work;
                    set_record_view(v->root,(int16_t)v->x,(int16_t)v->y,
                        (int16_t)v->z,(int16_t)v->rate_x,v->rate_y); /* C28F16 */
                } else if(zone.phase==ZONE_AFTER_FAULT) fault_hook(); /* C06C02 */
                else {
                    fprintf(stderr,"native record zone child unavailable: %u\n",(unsigned)zone.phase); abort();
                }
            }
            const GeometryState v=zone.work;
            *w=(DynamicsState){v.primary,v.detail,v.x,v.y,v.z,v.rate_x,v.rate_y,v.rate_z,
                v.root,v.record,v.geometry,v.scene,v.table,v.face,v.child_equal};
            break;
        }
        case DY_RECORD_CONTROLS:
            update_dynamics_record_input(record,w->primary); break;
        case DY_RECORD_ACTION: {
            AutopilotFrame guidance={0};guidance.work=*w;
            if(!update_dynamics_record_action(&guidance,NULL)) {
                fprintf(stderr,"native guidance boundary unavailable: %u at %06X\n",
                    (unsigned)guidance.phase,guidance.unresolved_target);abort();
            }
            *w=guidance.work;break;
        }
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
typedef struct { uint32_t event; } RecordView;
static uint32_t record_view_child(void *context,enum ViewCommandChild child) {
    const RecordView *view=context;
    if(child==VIEW_COMMAND_ZOOM_MAXIMUM) set_zoom_maximum();
    else if(child==VIEW_COMMAND_REDRAW) request_cockpit_redraw();
    else abort();
    return view->event; /* C08324/C082B8 preserve D0. */
}
static ContextPublicationResult record_publication_child(void *context,enum ContextPublicationChild child) {
    RecordView *view=context;
    if(child==CONTEXT_PUBLISH_ZOOM) set_zoom_maximum();
    else if(child==CONTEXT_PUBLISH_VIEW) {
        const ViewCommandHooks hooks={record_view_child,NULL,view};
        view->event=finish_view_redraw(view->event,&hooks);
        view->event=(view->event&0xffffff00u)|publish_command_event((uint8_t)view->event,NULL);
    } else abort();
    return (ContextPublicationResult){view->event,rd_s16(VIEW_RECORD)};
}
static uint32_t action_view(uint32_t event,int16_t index) {
    RecordView state={event};
    const ViewCommandHooks view={record_view_child,NULL,&state};
    const ContextPublicationHooks hooks={.view=&view,.consume=record_publication_child,.context=&state};
    publish_context_record_command(state.event,index,&hooks);
    return state.event;
}
static PostflightScheduleResult schedule_child(void *context,enum PostflightScheduleChild child,gaddr record) {
    (void)context;
    if(child==SCHEDULE_SELECTION_GATE) { release_lost_selection(); return (PostflightScheduleResult){0,1}; }
    if(child==SCHEDULE_READY_GATE) return postflight_player_readiness(NULL); /* C0A3EA */
    if(child==SCHEDULE_RESTORE_FIRST || child==SCHEDULE_RESTORE_SECOND) {
        schedule_postflight(POSTFLIGHT_RESTORE_RECORD,0,record,NULL); /* C0A12E */
        return (PostflightScheduleResult){0,1}; /* Result is dead to C0A002. */
    }
    if(child==SCHEDULE_NINE) {
        schedule_postflight(POSTFLIGHT_MODE_NINE,0,record,NULL);
        return (PostflightScheduleResult){0,1};
    }
    if(child==SCHEDULE_125) {
        schedule_postflight(POSTFLIGHT_MODE_125,125,record,NULL); /* C0A334 */
        return (PostflightScheduleResult){0,1};
    }
    if(child==SCHEDULE_THREE) {
        const PostflightScheduleHooks hooks={schedule_child,NULL,NULL};
        schedule_postflight(POSTFLIGHT_MODE_THREE,3,record,&hooks);
        return (PostflightScheduleResult){0,1};
    }
    if(child==SCHEDULE_SIX) {
        const PostflightScheduleHooks hooks={schedule_child,NULL,NULL};
        schedule_postflight(POSTFLIGHT_MODE_SIX,6,record,&hooks); /* C0A15C */
        return (PostflightScheduleResult){0,1};
    }
    if(child==SCHEDULE_FOUR) {
        const PostflightScheduleHooks hooks={schedule_child,NULL,NULL};
        schedule_postflight(POSTFLIGHT_MODE_FOUR,4,record,&hooks); /* C09EC4 */
        return (PostflightScheduleResult){0,1};
    }
    if(child==SCHEDULE_FIVE) {
        const PostflightScheduleHooks hooks={schedule_child,NULL,NULL};
        schedule_postflight(POSTFLIGHT_MODE_FIVE,5,record,&hooks); /* C0A002 */
        return (PostflightScheduleResult){0,1};
    }
    if(child==SCHEDULE_SEVEN) {
        const PostflightScheduleHooks hooks={schedule_child,NULL,NULL};
        schedule_postflight(POSTFLIGHT_MODE_SEVEN,7,record,&hooks); /* C0A1E0 */
        return (PostflightScheduleResult){0,1};
    }
    if(child==SCHEDULE_OTHER) {
        const PostflightScheduleHooks hooks={schedule_child,NULL,NULL};
        schedule_postflight(POSTFLIGHT_MODE_OTHER,rd_u8(MODE_SELECT),record,&hooks); /* C0A364 */
        return (PostflightScheduleResult){0,1};
    }
    if(child==SCHEDULE_PREPARE_FOUR || child==SCHEDULE_PREPARE_SEVEN) {
        /* C09EC4/C0A1E0 retain record 4's +6 OR +12 word before C1BEE8;
         * only its low byte reaches the command queue. D1 is STREAM_MODE. */
        RecordView state={rd_u16(CONTROL_RECORDS+0x806u)|rd_u16(CONTROL_RECORDS+0x80cu)};
        const ViewCommandHooks view={record_view_child,NULL,&state};
        const ContextPublicationHooks hooks={.view=&view,.consume=record_publication_child,.context=&state};
        publish_context_record_command(state.event,rd_s16(STREAM_MODE),&hooks);
        return (PostflightScheduleResult){state.event,0};
    }
    fprintf(stderr,"native record schedule child unavailable: %u\n",(unsigned)child); abort();
}
typedef struct { gaddr companion,viewer; } RecordLoop;
static void record_event(void *context,const RecordUpdateEvent *event) {
    RecordLoop *loop=context;
    if(event->phase==RECORD_UPDATE_ROOT ||
       (event->phase==RECORD_UPDATE_SLOT && !event->other)) loop->companion=event->companion;
    if(event->phase==RECORD_UPDATE_ROOT) loop->viewer=CONTROL_RECORDS;
}
static FlightWorking flight_child(void *context,enum FlightChild child,FlightWorking w) {
    (void)context;
    switch(child) {
    case FC_CLASSIFY_RECORD: classify_record_range(w.record);break; /* C24568 */
    case FC_PROJECT_VIEW: {
        int32_t point[3];
        local_to_world(w.viewer,w.viewer+RECORD_INVERSE,(int16_t)w.x,(int16_t)w.y,(int16_t)w.z,point);
        w.value=(uint32_t)point[0];w.speed=(uint32_t)point[1];w.turn=(uint32_t)point[2];break;
    }
    case FC_SIGHT_RECORD: update_in_sight(w.record,w.viewer);break; /* C2436A */
    case FC_ROUTE_FAULT: case FC_ZONE_FAULT: fault_hook();break;
    default: fprintf(stderr,"native record dispatch child unavailable: %u\n",(unsigned)child);abort();
    }
    return w;
}
static int record_child(void *context,enum RecordUpdateChild child,unsigned slot) {
    RecordLoop *loop=context;
    gaddr record=CONTROL_RECORDS+512u*slot;
    const FlightActionHooks actions={.consume_values=action_child};
    FlightActionState work={0}; work.record=record; work.source=loop->companion;
    switch(child) {
    case RECORD_UPDATE_PERIODIC: {
        const DynamicsHooks regions={.consume_values=region_child};
        update_scene_regions((DynamicsState){0},&regions); return 0;
    }
    case RECORD_UPDATE_RELEASE_SELECTION: release_lost_selection(); return 0;
    case RECORD_UPDATE_ROOT_CONTROL: advance_flight_record_control(work,&actions); return 0;
    case RECORD_UPDATE_SECONDARY_CONTROL: advance_flight_record_stream(work,&actions); return 0;
    case RECORD_UPDATE_ROOT_VIEW: {
        RecordViewUpdateWork view={0}; update_record_view(record,loop->viewer,0,&view);
        loop->viewer=view.viewer;return 1;
    }
    case RECORD_UPDATE_ROOT_MARKER: classify_selected_record_range(record); return 0;
    case RECORD_UPDATE_POSE: dynamics(record); return 0;
    case RECORD_UPDATE_PRIMARY_READY: return select_flight_record_action(work,1,&actions);
    case RECORD_UPDATE_PRIMARY_PLACE: try_primary_flight_record_action(work,&actions);return 0;
    case RECORD_UPDATE_SECONDARY_READY: return select_flight_record_action(work,0,&actions);
    case RECORD_UPDATE_SECONDARY_PLACE: try_flight_record_action(work,&actions);return 0;
    /* C231A2 reads A2, the aircraft owning this inactive paired missile. */
    case RECORD_UPDATE_PAIRED_READY: return paired_record_ready(loop->companion);
    case RECORD_UPDATE_DISPATCH: {
        FlightWorking flight={0}; flight.record=record; flight.auxiliary=loop->companion;
        /* C241A6's sight tail can retain the caller's viewer when the
         * selected-reference heading arm bypasses view projection. */
        flight.viewer=loop->viewer;
        const FlightHooks hooks={.consume_values=flight_child};
        flight=advance_main_loop_flight_record(flight,&hooks);
        loop->viewer=flight.viewer;
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
    if(child==ORIGIN_FALLBACK) { fault_hook(); return (SelectorOriginTriple){{0,0,0}}; }
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
void native_records_select_origin(void) {
    const SelectorOriginHooks hooks={origin_child,NULL,NULL};
    select_origin_control_record(&hooks);
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
