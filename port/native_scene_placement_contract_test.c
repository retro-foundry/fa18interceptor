#include "native_scene_placement.h"
#include "run075_trig_asset.h"

#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput commands;
    FA18FlightCommandState flight;
    FA18ViewCommandState view;
    FA18ContextCommandState context;
    FA18CommandEffects effects;
    FA18NativeSceneRecords records;
    FA18NativeScenePlayerSetup player;
    FA18NativeScenePlacementAssets assets;
    FA18NativeScenePointerGroup groups[16];
    FA18NativeSceneRecorder recorder;
    FA18NativeScenePlacement placement;
    uint8_t record_source[16*512],work_source[16*32];
    uint8_t pose_bytes[32],grid_words[4],grid_bytes[2],kind_data[8],descriptor[6];
    uint8_t recorder_bytes[64],recorder_words[16];
    uint8_t mission_b,mission_c,player_flags[6],phase,selection;
    uint8_t root_ready,bar_e,bar_redraw,fire,input_source;
    uint16_t limit,selected,shown,marker,context_record,key_a,key_b,grid_x,grid_z,error;
    uint32_t warnings,events;
    int32_t target[3];
} Fixture;

static void be16(uint8_t *p,uint16_t value) { p[0]=(uint8_t)(value>>8); p[1]=(uint8_t)value; }
static void be32(uint8_t *p,uint32_t value) {
    p[0]=(uint8_t)(value>>24); p[1]=(uint8_t)(value>>16); p[2]=(uint8_t)(value>>8); p[3]=(uint8_t)value;
}
static void pose(uint8_t *p,const int16_t words[8]) {
    unsigned i; for(i=0;i<8;++i) be16(p+2*i,(uint16_t)words[i]);
}
static void initialize(Fixture *f) {
    unsigned i;
    memset(f,0,sizeof *f);
    assert(fa18_import_native_scene_records(&f->records,&f->commands,
        f->record_source,sizeof f->record_source,f->work_source,sizeof f->work_source));
    f->flight.commands=&f->commands; f->flight.player=f->records.aircraft;
    f->view.flight=&f->flight; f->context.view=&f->view;
    f->context.records=f->records.geometry; f->context.record_count=16;
    f->effects.context=&f->context;
    f->player=(FA18NativeScenePlayerSetup){.records=&f->records,.flight=&f->flight,
        .effects=&f->effects,.mission_flags_b=&f->mission_b,.mission_flags_c=&f->mission_c,
        .phase=&f->phase,.selection_active=&f->selection,.limit=&f->limit,
        .selected_record=&f->selected,.message_shown=&f->shown,.message_marker=&f->marker,
        .warning_causes=&f->warnings,.event_bits=&f->events};
    for(i=0;i<6;++i) f->player.player_flags[i]=f->player_flags+i;
    f->assets.poses=(PortFieldWindow){.bytes=f->pose_bytes,.byte_count=sizeof f->pose_bytes};
    f->assets.grid_words=(PortFieldWindow){.bytes=f->grid_words,.byte_count=sizeof f->grid_words};
    f->assets.grid_bytes=(PortFieldWindow){.bytes=f->grid_bytes,.byte_count=sizeof f->grid_bytes};
    f->assets.trig=(FA18FlightTrigData){.bytes=fa18_run075_trig_bytes,
        .byte_count=sizeof fa18_run075_trig_bytes};
    /* A nonnegative header selects word +4, then byte +6 as the root kind. */
    f->kind_data[6]=0xab;
    f->assets.input_other.procedure=FA18_SCENE_PROCEDURE_COMPONENT_ACCUMULATION;
    f->assets.input_other.data[0].data=(PortFieldWindow){.bytes=f->kind_data,.byte_count=sizeof f->kind_data};
    f->recorder=(FA18NativeSceneRecorder){.bytes=f->recorder_bytes,
        .words=f->recorder_words,.byte_count=sizeof f->recorder_bytes,
        .word_byte_count=sizeof f->recorder_words};
    f->placement=(FA18NativeScenePlacement){.player=&f->player,.assets=&f->assets,
        .pointer_groups=f->groups,.pointer_group_count=16,.recorder=&f->recorder,
        .context_record=&f->context_record,.condition_key_a=&f->key_a,.condition_key_b=&f->key_b,
        .grid_origin_x=&f->grid_x,.grid_origin_z=&f->grid_z,.error_word=&f->error,
        .target_point=f->target,.root_ready=&f->root_ready,.bar_e_flag=&f->bar_e,
        .bar_redraw_e=&f->bar_redraw,.fire_state=&f->fire,.input_source=&f->input_source};
}

int main(void) {
    static Fixture f;
    const int16_t positive[8]={2,-3,0,4,-5,6,-7,0};
    const int16_t negative[8]={(int16_t)0x8004,0,0,0,0,0,0,0};
    FA18NativeSceneRecord *root,*selected;
    initialize(&f); root=f.records.records;
    pose(f.pose_bytes,positive); be16(f.grid_words,3); be16(f.grid_words+2,(uint16_t)-4);
    f.grid_bytes[0]=1; f.grid_bytes[1]=(uint8_t)-2;
    f.recorder.byte_cursor=9; f.recorder.word_cursor=8; f.recorder.record_count=7;
    assert(fa18_place_native_scene_root(&f.placement));
    assert(f.recorder.byte_cursor==0 && f.recorder.word_cursor==0 &&
           f.recorder.playback_byte==0 && f.recorder.playback_word==4 &&
           !f.recorder.record_count && !f.recorder.playback_count);
    assert(f.context.recording_write==f.recorder_bytes && f.context.recording_remaining==sizeof f.recorder_bytes);
    assert(f.view.update_mask==0xff && f.context_record==0 && *root->level==9);
    assert(f.commands.indexed.throttle==72 && f.commands.indexed.throttle_companion==72 && f.fire==0xfe);
    assert(root->aircraft->equipment_kind==0 && root->byte_7d==0x0b && root->aircraft->secondary_flags==0x80);
    assert(f.key_a==2 && f.key_b==(uint16_t)-3 && root->word_06==9 && root->word_08==(uint16_t)-14);
    assert(root->geometry->position[0]==0x02001c60u && root->geometry->position[1]==0x708u &&
           root->geometry->position[2]==0xfdffda90u);
    assert(root->word_0c==0x10 && root->word_0e==0xffeb && root->long_10==0xfffffff9u);
    assert(f.target[0]==-4192 && f.target[1]==-0x708 && f.target[2]==5232);
    assert(root->forward[0][0]==0x4000 && root->geometry->inverse[2][2]==0x4000);

    initialize(&f); root=f.records.records; selected=f.records.records+4;
    pose(f.pose_bytes+16,negative); f.commands.indexed.pose_entry=1;
    selected->aircraft->flags=0x40;
    selected->geometry->position[0]=1000; selected->geometry->position[1]=2000;
    selected->geometry->position[2]=3000;
    selected->geometry->inverse[0][0]=selected->geometry->inverse[1][1]=
        selected->geometry->inverse[2][2]=0x4000;
    be32(f.descriptor+2,0x80000070u);
    f.groups[4].data[3].data=(PortFieldWindow){.bytes=f.descriptor,.byte_count=sizeof f.descriptor};
    assert(fa18_place_native_scene_root(&f.placement));
    assert(root->long_10==0x77 && root->geometry->position[1]==0x7708 && root->byte_04==0xc8);
    assert(root->geometry->position[0]==12264 && root->geometry->position[2]==109496);
    assert(root->word_b8==0 && root->forward[0][0]==0x4000 && !f.error);

    initialize(&f); pose(f.pose_bytes,positive); f.placement.fire_state=NULL;
    assert(!fa18_place_native_scene_root(&f.placement));
    return 0;
}
