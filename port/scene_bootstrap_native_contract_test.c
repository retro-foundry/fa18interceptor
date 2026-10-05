#include "scene_bootstrap_native.h"
#include "run075_trig_asset.h"
#include "native_record_control_test_support.h"
#include "native_record_pose_test_support.h"
#include "native_record_action_placement_test_support.h"
#include "native_postflight_test_support.h"
#include "native_scene_regions_test_support.h"
#include "native_record_dispatch_test_support.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput input; FA18FlightCommandState flight; FA18ViewCommandState view;
    FA18ContextCommandState context; FA18CommandEffects effects; FA18CommandQueue queue;
    FA18NativeSceneRecords records; FA18NativeScenePlayerSetup player;
    FA18NativeScenePlacementAssets placement_assets; FA18NativeScenePointerGroup pointer_groups[16];
    FA18NativeSceneRecorder recorder; FA18NativeScenePlacement placement;
    FA18NativeRecordUpdateStage record_update; FA18NativeSelectorOrigin selector_origin;
    FA18NativeRecordSelection record_selection;
    FA18NativeRecordPose pose; FA18RecordPoseTestStorage pose_storage; FA18NativeRecordPoseOps pose_ops;
    FA18NativeRecordControl control_player; FA18RecordControlTestStorage control_storage;
    FA18NativeRecordActionPlacement action_placement; FA18RecordActionPlacementTestStorage placement_storage;
    FA18NativePostflight postflight; FA18PostflightTestStorage post_storage;
    FA18NativeSceneRegions regions; FA18SceneRegionsTestStorage region_storage;
    FA18NativeRecordRange range; FA18NativeVectorMath vector_math;
    uint8_t range_redraw; uint16_t range_magnitude;
    FA18NativeRecordView record_view; FA18NativeRecordViewWork view_work;
    FA18NativeRecordViewAssets view_assets;
    uint8_t view_pending,view_flag,view_created,view_admitted;
    int16_t normalized[3];
    FA18NativeControlRecordUpdate control_update; FA18NativeRecordDispatch dispatch; FA18RecordDispatchTestStorage dispatch_storage;
    FA18NativeContextRefresh refresh; FA18NativeContextRefreshOps refresh_ops;
    FA18NativeStartupRanges startup; FA18NativeViewedRecordWord viewed;
    FA18NativeGraphicsSetup graphics; FA18NativeGraphicsPlane planes[6];
    FA18NativeRendererClear renderer; FA18NativeSceneBootstrap bootstrap;
    FA18TemplateBitmaskBuffers gate_buffers; FA18TemplateBitmaskState gates;
    uint8_t streams[3][258]; uint16_t error_word;
    uint8_t buffers[6][8008],mission_b,mission_c,flags[6],phase,selection,fifth;
    uint8_t pose_data[16],grid_words_data[4],grid_bytes_data[2],kind_data[8];
    uint8_t recorder_bytes[64],recorder_words[16],root_ready,bar_e,bar_redraw,fire,input_source;
    uint8_t update_input,update_mirror,update_inhibit,context_select,detail,selector_coarse,selector_fine,record_rate;
    uint8_t post_event,counter_a,counter_b,primary_gate,secondary_gate;
    uint8_t action_first,action_second,action_third,pair_override;
    uint8_t prepared,alternate,cell_checks,view_mode,fixed_readouts,frame_gate;
    uint8_t limit_byte,previous_limit,context_state,transition,previous_byte,byte_be;
    uint16_t limit,selected,shown,marker,menu_return,word_4fda0,countdown,word_a6,word_a8,history,minimum,scales[2],depth[24],words[52];
    PortFieldByte fields[104];
    uint32_t warnings,events,message_queue,message_timer,valid[2],reference;
    int32_t target[3];
    int32_t update_long_mirror,projection_depth,origin[3];
    uint16_t context_record,key_a,key_b,grid_x,grid_z;
    uint16_t scaled,selector_x,selector_z;
    uint16_t action_pending;
    uint16_t periodic,current_slot,current_stride;
    uint16_t refresh_timer,stage_selector,current_colour;
    uint32_t line_style;
    unsigned calls; int complete;
} Fixture;
static Fixture fixture;
static int pose(void *context,FA18NativeRecordPose *state,FA18NativeRecordPoseChild child,
                const FA18NativeRecordPoseInput *in,FA18NativeRecordPoseResult *out) {
    Fixture *f=context; uint8_t high,low; unsigned i;
    assert(state==&f->pose && in && out && in->slot<16);
    if(child==FA18_POSE_MOTION_CANDIDATE) out->clear=1;
    if(child!=FA18_POSE_RECORD_MATRIX || in->slot!=0) return 1;
    assert(f->calls++==0 && f->context.origin_first==0x10c00000);
    assert(f->flight.viewed==f->records.aircraft && f->flight.player->flags==0x11c8);
    assert(f->countdown==5 && f->history==0x800 && f->transition==1 && f->phase==0);
    assert(f->limit_byte==f->previous_limit && f->context_state==0xff && f->message_timer==0x1b8);
    assert(f->view.line_last_row==0xa7 && f->view.span_origin==0x32 && f->view.span_origin_y==0x320);
    assert(f->context.pan==0x1c20 && !f->context.rotate && f->view.zoom_scale==0x80 && f->view.zoom_flags==0x80);
    assert(f->valid[0]==0xffffffff && f->valid[1]==0xffffffff && f->minimum==0x7fff && f->reference==0x800000);
    assert(f->depth[21]==21 && f->depth[22]==0x5a5a && f->records.records[0].byte_7d==0x0b);
    assert(port_read_field_byte(f->fields+30,&high) && port_read_field_byte(f->fields+31,&low));
    assert(!high && !low);
    for(i=0;i<2048;++i) assert(!f->gate_buffers.first[i] && !f->gate_buffers.second[i] && !f->gate_buffers.third[i]);
    f->input.indexed.mode=0x7d; f->flight.viewed=f->records.aircraft+7; f->context.origin_first++;
    f->input.message_state=3;
    return 1;
}
static int refresh(void *context,FA18NativeContextRefresh *state,FA18NativeContextRefreshChild child) {
    Fixture *f=context;
    assert(state==&f->refresh);
    if(child!=FA18_CONTEXT_REFRESH_SORT) return 1;
    assert(f->calls++==1 && f->input.message_state==3);
    return f->complete;
}
static void initialize(Fixture *f) {
    uint8_t source[8192],work[512],neighbors[FA18_COMMAND_QUEUE_NEIGHBORS],keys[128]={0};
    unsigned i;
    memset(f,0,sizeof *f); memset(source,0x5a,sizeof source); memset(work,0x5a,sizeof work); memset(neighbors,0x5a,sizeof neighbors);
    assert(fa18_import_native_scene_records(&f->records,&f->input,source,sizeof source,work,sizeof work));
    f->flight.commands=&f->input; f->flight.player=f->records.aircraft;
    f->view.flight=&f->flight; f->context.view=&f->view;
    f->context.records=f->records.geometry; f->context.record_count=16; f->effects.context=&f->context;
    assert(fa18_initialize_command_queue(&f->queue,&f->context,neighbors,sizeof neighbors,keys,sizeof keys));
    f->player=(FA18NativeScenePlayerSetup){.records=&f->records,.flight=&f->flight,.effects=&f->effects,
        .mission_flags_b=&f->mission_b,.mission_flags_c=&f->mission_c,.phase=&f->phase,
        .selection_active=&f->selection,.limit=&f->limit,.selected_record=&f->selected,.message_shown=&f->shown,
        .message_marker=&f->marker,.warning_causes=&f->warnings,.event_bits=&f->events};
    for(i=0;i<6;++i) f->player.player_flags[i]=f->flags+i;
    f->kind_data[6]=0xab;
    f->placement_assets.poses=(PortFieldWindow){.bytes=f->pose_data,.byte_count=sizeof f->pose_data};
    f->placement_assets.grid_words=(PortFieldWindow){.bytes=f->grid_words_data,.byte_count=sizeof f->grid_words_data};
    f->placement_assets.grid_bytes=(PortFieldWindow){.bytes=f->grid_bytes_data,.byte_count=sizeof f->grid_bytes_data};
    f->placement_assets.trig=(FA18FlightTrigData){.bytes=fa18_run075_trig_bytes,
        .byte_count=sizeof fa18_run075_trig_bytes};
    f->placement_assets.input_other.procedure=FA18_SCENE_PROCEDURE_COMPONENT_ACCUMULATION;
    f->placement_assets.input_other.data[0].data=(PortFieldWindow){.bytes=f->kind_data,.byte_count=sizeof f->kind_data};
    f->recorder=(FA18NativeSceneRecorder){.bytes=f->recorder_bytes,.words=f->recorder_words,
        .byte_count=sizeof f->recorder_bytes,.word_byte_count=sizeof f->recorder_words};
    f->placement=(FA18NativeScenePlacement){.player=&f->player,.assets=&f->placement_assets,
        .pointer_groups=f->pointer_groups,.pointer_group_count=16,.recorder=&f->recorder,
        .context_record=&f->context_record,.condition_key_a=&f->key_a,.condition_key_b=&f->key_b,
        .grid_origin_x=&f->grid_x,.grid_origin_z=&f->grid_z,.error_word=&f->error_word,
        .target_point=f->target,.root_ready=&f->root_ready,.bar_e_flag=&f->bar_e,
        .bar_redraw_e=&f->bar_redraw,.fire_state=&f->fire,.input_source=&f->input_source};
    f->record_selection=(FA18NativeRecordSelection){.records=&f->records,
        .selected_record=&f->selected,.selection_marker=&f->marker,.action_pending=&f->action_pending,
        .selection_active=&f->selection,.origin_enable=&f->context_select,
        .action_first=&f->action_first,.action_second=&f->action_second,
        .action_third=&f->action_third,.pair_override=&f->pair_override};
    f->range=(FA18NativeRecordRange){.records=&f->records,.selected_record=&f->selected,
        .current_stride=&f->current_stride,.magnitude=&f->range_magnitude,.bar_redraw_f=&f->range_redraw};
    f->record_view=(FA18NativeRecordView){.records=&f->records,.assets=&f->view_assets,
        .selected_record=&f->selected,.current_stride=&f->current_stride,.current_slot=&f->current_slot,
        .tick_word=&f->periodic,.error_word=&f->error_word,.post_input_event=&f->post_event,
        .mode=&f->view_mode,.limit=&f->limit_byte,.pending=&f->view_pending,.view_flag=&f->view_flag,
        .created=&f->view_created,.admitted=&f->view_admitted,.normalized=f->normalized};
    f->vector_math=(FA18NativeVectorMath){f->range.table,&f->range_magnitude,f->normalized};
    f->record_view.vector_math=&f->vector_math;
    f->view_work.viewer=f->records.records;
    fa18_test_bind_record_control(&f->control_player,&f->control_storage,&f->records,
        &f->view_work,&f->current_slot,&f->post_event,&f->view_mode);
    f->control_player.origin_enable=&f->context_select;
    f->control_player.scene_redraw=&f->view.update_mask;
    f->control_player.stream_view_state=&f->fire;
    fa18_test_bind_record_pose(&f->pose,&f->pose_storage,&f->control_player,&f->current_stride);
    f->pose.events=&f->events;
    fa18_test_bind_record_action_placement(&f->action_placement,&f->placement_storage,
        &f->control_player,&f->pose,&f->limit_byte);
    f->action_placement.pointer_groups=f->pointer_groups;
    f->action_placement.warning_causes=&f->warnings;
    f->action_placement.stores_redraw_a=&f->flight.weapon_mode_redraws;
    f->action_placement.stores_redraw_b=&f->flight.weapon_redraws;
    f->action_placement.viewed_record=f->fields+30;
    f->pose_ops=(FA18NativeRecordPoseOps){pose,f}; f->pose.ops=&f->pose_ops;
    f->control_update=(FA18NativeControlRecordUpdate){.records=&f->records,
        .selection=&f->record_selection,.range=&f->range,
        .view=&f->record_view,.view_work=&f->view_work,.control=&f->control_player,.pose=&f->pose,.placement=&f->action_placement,
        .post_input_event=&f->post_event,.counter_first=&f->counter_a,.counter_second=&f->counter_b,
        .primary_gate=&f->primary_gate,.secondary_gate=&f->secondary_gate,.periodic_word=&f->periodic,
        .current_slot=&f->current_slot,.current_stride=&f->current_stride};
    fa18_test_bind_postflight(&f->postflight,&f->post_storage,&f->control_update);
    fa18_test_bind_scene_regions(&f->regions,&f->region_storage,&f->control_update);
    fa18_test_bind_record_dispatch(&f->dispatch,&f->dispatch_storage,&f->control_update,&f->context,&f->queue);
    f->postflight.target_record=&f->flight.spawn_gate;
    f->postflight.command_word=&f->flight.command_word;
    f->postflight.player_phase=&f->phase;
    f->postflight.player_flags_f=f->flags+5;
    f->selector_origin=(FA18NativeSelectorOrigin){.vector_math=&f->vector_math,.records=&f->records,.origin=f->origin};
    f->record_update=(FA18NativeRecordUpdateStage){.records=&f->records,.view=&f->view,
        .control_records=&f->control_update,.origin_update=&f->selector_origin,
        .input_byte=&f->update_input,.input_byte_mirror=&f->update_mirror,.change_inhibit=&f->update_inhibit,
        .context_selection=&f->context_select,.origin_detail_mode=&f->detail,
        .selector_byte_coarse=&f->selector_coarse,.selector_byte_fine=&f->selector_fine,.record_rate=&f->record_rate,
        .position_bias=f->target+1,.long_mirror=&f->update_long_mirror,.projection_depth=&f->projection_depth,
        .origin=f->origin,.scaled_word=&f->scaled,.selector_word_x=&f->selector_x,.selector_word_z=&f->selector_z};
    f->refresh_ops=(FA18NativeContextRefreshOps){refresh,f};
    f->refresh=(FA18NativeContextRefresh){.records=&f->records,.view=&f->view,.ops=&f->refresh_ops,
        .position_bias=f->target+1,.origin=f->origin,.cell_timer=&f->refresh_timer,.error_word=&f->error_word,
        .condition_key_a=&f->key_a,.condition_key_b=&f->key_b,.stage_selector=&f->stage_selector,
        .current_colour=&f->current_colour,.line_style=&f->line_style,.context_selection=&f->context_select,
        .prepared=&f->prepared,.alternate=&f->alternate,.cell_checks=&f->cell_checks,
        .view_mode=&f->view_mode,.fixed_readouts=&f->fixed_readouts,.frame_gate=&f->frame_gate};
    memset(f->buffers,0x5a,sizeof f->buffers);
    for(i=0;i<6;++i) f->planes[i]=(FA18NativeGraphicsPlane){f->buffers[i],sizeof f->buffers[i],0};
    for(i=0;i<5;++i) f->graphics.source[i]=f->planes+i;
    for(i=0;i<4;++i) f->graphics.source[5+i]=f->planes+i;
    f->renderer=(FA18NativeRendererClear){&f->graphics,f->planes+5,&f->fifth};
    memset(&f->gate_buffers,0x5a,sizeof f->gate_buffers);
    f->gates.buffers=&f->gate_buffers; f->gates.error_word=&f->error_word;
    for(i=0;i<3;++i) {
        unsigned row;
        for(row=0;row<128;++row) f->streams[i][2*row]=1;
        f->streams[i][256]=f->streams[i][257]=0xff;
        f->gates.streams[i]=f->streams[i]; f->gates.stream_sizes[i]=258;
    }
    for(i=0;i<52;++i) {
        f->words[i]=0x5a5a;
        f->fields[2*i]=(PortFieldByte){.unsigned_word=f->words+i,.shift=8};
        f->fields[2*i+1]=(PortFieldByte){.unsigned_word=f->words+i};
    }
    f->words[15]=5*512; f->previous_limit=11;
    for(i=0;i<24;++i) f->depth[i]=0x5a5a;
    f->bootstrap=(FA18NativeSceneBootstrap){.context=&f->context,.player=&f->player,.placement=&f->placement,.update=&f->record_update,.refresh=&f->refresh,.startup=&f->startup,
        .viewed_word=&f->viewed,.renderer=&f->renderer,.gates=&f->gates,.scene_limit=&f->limit_byte,.previous_scene_limit=&f->previous_limit,
        .context_state=&f->context_state,.menu_transition=&f->transition,.previous_state_byte=&f->previous_byte,.byte_458be=&f->byte_be,
        .menu_return_word=&f->menu_return,.word_4fda0=&f->word_4fda0,.countdown=&f->countdown,
        .word_459a6=&f->word_a6,.word_459a8=&f->word_a8,.history_record=&f->history,.readout_minimum=&f->minimum,
        .row_scales={f->scales,f->scales+1},.message_queue_first=&f->message_queue,.message_timer=&f->message_timer,
        .readout_valid={f->valid,f->valid+1},.reference_18=&f->reference,.depth_values=f->depth,.depth_count=24};
    assert(fa18_bind_native_scene_bootstrap(&f->bootstrap,&f->queue,f->fields,104));
    assert(f->flight.viewed==f->records.aircraft+5);
    f->complete=1;
}
int main(void) {
    Fixture *f=&fixture; unsigned i,j; uint8_t byte,record[512];
    FA18NativeSceneBootstrapCall call={&f->bootstrap};
    initialize(f);
    assert(fa18_native_scene_bootstrap_callback(&call));
    assert(f->calls==2 && f->flight.viewed==f->records.aircraft+7);
    for(i=0;i<6;++i) for(j=0;j<8008;++j) assert(f->buffers[i][j]==(j<8000?0:0x5a));
    for(i=0;i<16;++i) {
        assert(fa18_read_native_scene_record(f->records.records+i,0,record,sizeof record));
        for(j=164;j<512;++j) assert(record[j]==0x5a);
        for(j=0;j<32;++j) assert(f->records.work[i][j]==(j==1?0x10:(j==4 || j==5)?0xff:0));
    }
    for(i=0;i<48;++i) {
        assert(port_read_field_byte(f->queue.slots+0x99+i,&byte));
        assert(byte==((i>=16 && i<44)?0x20:0x30));
    }
    /* Rebinding reads the live identity, never the stale imported word. */
    assert(fa18_bind_native_scene_bootstrap(&f->bootstrap,&f->queue,f->fields,104));
    assert(f->flight.viewed==f->records.aircraft+7);
    assert(fa18_clear_native_startup_ranges(&f->startup));
    assert(f->flight.viewed==f->records.aircraft);
    initialize(f); f->complete=0;
    assert(!fa18_native_scene_bootstrap_callback(&call));
    assert(f->calls==2 && f->input.message_state==3 && f->countdown==5);
    initialize(f);
    f->refresh.ops=NULL;
    assert(!fa18_bootstrap_native_scene(&f->bootstrap));
    assert(f->calls==1 && f->countdown==5 && f->flight.viewed==f->records.aircraft+7);
    initialize(f);
    f->renderer.additional_buffer=NULL;
    assert(!fa18_native_scene_bootstrap_callback(&call));
    assert(!f->calls && f->message_timer==0x1b8 && f->countdown==5);
    initialize(f); f->gates.streams[2]=NULL;
    assert(!fa18_native_scene_bootstrap_callback(&call));
    assert(!f->calls && f->flight.viewed==f->records.aircraft && f->countdown==5);
    /* Invalid data is explicit, rather than silently choosing record zero. */
    f->fields[30]=(PortFieldByte){.unsigned_word=f->words+15,.shift=8};
    f->fields[31]=(PortFieldByte){.unsigned_word=f->words+15};
    f->words[15]=1;
    assert(!fa18_bind_native_viewed_record_word(&f->viewed,&f->records,&f->flight,f->fields,104));
    f->words[15]=16*512;
    assert(!fa18_bind_native_viewed_record_word(&f->viewed,&f->records,&f->flight,f->fields,104));
    assert(!fa18_native_scene_bootstrap_callback(NULL));
    return 0;
}
