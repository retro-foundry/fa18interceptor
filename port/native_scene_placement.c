#include "native_scene_placement.h"
#include "two_angle_matrix.h"

#include <limits.h>
#include <string.h>

enum {
    POINTER_HUNK=16, POINTER_INPUT_10=0x3c, POINTER_INPUT_OTHER=0x50,
    GRID_HUNK=8, GRID_WORDS=0x151a, GRID_BYTES=0x160e,
    POSE_HUNK=67, POSES=0x32
};

static int32_t signed_long(uint32_t value) {
    return value<=INT32_MAX?(int32_t)value:(int32_t)((int64_t)value-INT64_C(0x100000000));
}
static int16_t signed_word(uint16_t value) {
    return value<=INT16_MAX?(int16_t)value:(int16_t)((int32_t)value-0x10000);
}
static int32_t add_long(int32_t a,int32_t b) {
    return signed_long((uint32_t)a+(uint32_t)b);
}
static int32_t shift_long(int32_t value,unsigned count) {
    return signed_long((uint32_t)value<<count);
}
static int32_t asr_long(int32_t value,unsigned count) {
    if(value>=0) return value/(INT32_C(1)<<count);
    return (int32_t)-((-(int64_t)value+((INT64_C(1)<<count)-1))/(INT64_C(1)<<count));
}
static int16_t asr_word(int16_t value,unsigned count) {
    if(value>=0) return (int16_t)(value/(1<<count));
    return (int16_t)-((-(int32_t)value+((1<<count)-1))/(1<<count));
}
/* MOVEM.W sign extension, SWAP, then the later ASL.L #8. */
static int32_t swapped_word_shift8(int16_t value) {
    uint32_t swapped=((uint32_t)(uint16_t)value<<16)|(value<0?UINT16_MAX:0u);
    return signed_long(swapped<<8);
}
static uint8_t quadrant(int16_t across,int16_t along) {
    uint8_t a=(uint8_t)(3u-(uint8_t)((uint16_t)across&3u));
    uint8_t b=(uint8_t)(3u-(uint8_t)((uint16_t)along&3u));
    return (uint8_t)(b*4u+a);
}

static int asset_window(const FA18Hunks *hunks,uint32_t segment,size_t origin,
                        PortFieldWindow *window) {
    const FA18HunkSegment *source;
    if(!hunks || !window || !hunks->segments || segment>=hunks->count) return 0;
    source=hunks->segments+segment;
    if(!source->data || origin>source->size) return 0;
    *window=(PortFieldWindow){.bytes=source->data,.byte_count=source->size,.origin=origin};
    return 1;
}
static int asset_reference(const FA18Hunks *hunks,uint32_t source_segment,
                           uint32_t field,FA18NativeAssetReference *reference) {
    uint32_t segment,offset,raw;
    const FA18HunkSegment *source;
    if(!hunks || !reference || !hunks->segments || source_segment>=hunks->count) return 0;
    source=hunks->segments+source_segment;
    if(!source->data || field>source->size || source->size-field<4) return 0;
    raw=fa18_be32(source->data+field);
    memset(reference,0,sizeof *reference);
    /* A relocated zero is the first byte of a target hunk, not a null
     * reference. Only an unrelocated zero denotes absence. */
    if(!fa18_hunk_pointer(hunks,source_segment,field,&segment,&offset)) return !raw;
    if(segment>=hunks->count || !hunks->segments[segment].data ||
       offset>hunks->segments[segment].size) return 0;
    reference->segment=(uint16_t)segment; reference->offset=offset;
    return asset_window(hunks,segment,offset,&reference->data);
}
int fa18_load_native_scene_pointer_group(const FA18Hunks *hunks,uint32_t offset,
                                          FA18NativeScenePointerGroup *group) {
    unsigned i; uint32_t procedure_segment,procedure_offset;
    if(!group) return 0;
    memset(group,0,sizeof *group);
    if(!hunks || !hunks->segments || hunks->count<=POINTER_HUNK ||
       !hunks->segments[POINTER_HUNK].data) return 0;
    /* Resolve the actual source handler identity, independently of load
     * addresses. Region rows also use $78/$C8 and other original groups. */
    if(offset>=0x140 || offset%20 ||
       !fa18_hunk_pointer(hunks,POINTER_HUNK,offset,&procedure_segment,&procedure_offset)) return 0;
    if(procedure_segment==10 && procedure_offset==0x14)
        group->procedure=FA18_SCENE_PROCEDURE_RECORD_STREAM;
    else if(procedure_segment==16 && procedure_offset==0xa78)
        group->procedure=FA18_SCENE_PROCEDURE_COMPONENT_ACCUMULATION;
    else return 0;
    for(i=0;i<4;++i)
        if(!asset_reference(hunks,POINTER_HUNK,offset+4u+4u*i,group->data+i)) return 0;
    return 1;
}
int fa18_load_native_scene_placement_assets(const FA18Hunks *hunks,
                                             FA18NativeScenePlacementAssets *assets) {
    if(!assets) return 0;
    memset(assets,0,sizeof *assets);
    return asset_window(hunks,POSE_HUNK,POSES,&assets->poses) &&
        asset_window(hunks,GRID_HUNK,GRID_WORDS,&assets->grid_words) &&
        asset_window(hunks,GRID_HUNK,GRID_BYTES,&assets->grid_bytes) &&
        fa18_load_native_trig_data(hunks,&assets->trig)==0 &&
        fa18_load_native_scene_pointer_group(hunks,POINTER_INPUT_10,&assets->input_10) &&
        fa18_load_native_scene_pointer_group(hunks,POINTER_INPUT_OTHER,&assets->input_other);
}

static int root_kind(FA18NativeSceneRecord *root,const FA18NativeScenePointerGroup *group) {
    int16_t word; uint8_t kind;
    if(!root || !group || !group->data[0].data.bytes ||
       !port_field_window_s16(&group->data[0].data,0,&word)) return 0;
    if(word>=0) {
        if(!port_field_window_s16(&group->data[0].data,(word&0x4000)?2:4,&word)) return 0;
    }
    if(!port_field_window_byte(&group->data[0].data,6+(word&0x0fff),&kind)) return 0;
    root->byte_7d=(uint8_t)(kind&0x0f); return 1;
}
static int read_pose(const PortFieldWindow *poses,int8_t entry,int16_t words[8]) {
    int32_t offset=(int32_t)entry*16; unsigned i;
    for(i=0;i<8;++i)
        if(!port_field_window_s16(poses,offset+(int32_t)(2*i),words+i)) return 0;
    return 1;
}
static int place_positive(FA18NativeScenePlacement *s,FA18NativeSceneRecord *record,
                          const int16_t words[8]) {
    int16_t byte_offset,word_offset,adjust_x,adjust_z,pair_x,pair_z;
    int32_t x,z,dx,dz; uint8_t raw; uint16_t angles[3];
    byte_offset=signed_word((uint16_t)((uint16_t)words[2]*2u));
    word_offset=signed_word((uint16_t)((uint16_t)words[2]*4u));
    if(!port_field_window_byte(&s->assets->grid_bytes,byte_offset,&raw)) return 0;
    adjust_x=(int8_t)raw;
    if(!port_field_window_byte(&s->assets->grid_bytes,(int32_t)byte_offset+1,&raw)) return 0;
    adjust_z=(int8_t)raw;
    if(!port_field_window_s16(&s->assets->grid_words,word_offset,&pair_x) ||
       !port_field_window_s16(&s->assets->grid_words,(int32_t)word_offset+2,&pair_z)) return 0;

    *s->condition_key_a=(uint16_t)words[0]; *s->condition_key_b=(uint16_t)words[1];
    record->byte_0b=(uint8_t)words[2];
    record->word_06=(uint16_t)signed_word((uint16_t)((uint16_t)words[0]*4u+(uint16_t)adjust_x));
    *s->grid_origin_x=record->word_06;
    *s->grid_origin_z=(uint16_t)signed_word((uint16_t)((uint16_t)words[1]*4u+(uint16_t)adjust_z));
    record->word_08=*s->grid_origin_z;

    x=add_long(swapped_word_shift8(words[0]),shift_long(pair_x,10));
    z=add_long(swapped_word_shift8(words[1]),shift_long(pair_z,10));
    dx=add_long(shift_long(words[3],10),shift_long(words[5],4));
    dz=add_long(shift_long(words[4],10),shift_long(words[6],4));
    record->geometry->position[1]=0x708;
    record->geometry->position[0]=(uint32_t)add_long(x,dx);
    record->geometry->position[2]=(uint32_t)add_long(z,dz);
    s->target_point[0]=signed_long(0u-(uint32_t)dx);
    s->target_point[2]=signed_long(0u-(uint32_t)dz);
    s->target_point[1]=-0x708;
    record->word_0c=(uint16_t)asr_long(dx,8);
    record->word_0e=(uint16_t)asr_long(dz,8);
    record->long_10=UINT32_C(0xfffffff9);
    angles[0]=0; angles[1]=(uint16_t)((uint16_t)words[7]*80u); angles[2]=0;
    return fa18_publish_native_record_orientation(record,angles,&s->assets->trig);
}
static int place_negative(FA18NativeScenePlacement *s,FA18NativeSceneRecord *selected,
                          uint16_t index) {
    FA18NativeSceneRecord *root=s->player->records->records;
    const FA18NativeAssetReference *descriptor;
    FA18ContextCommandChildInput input={0}; FA18ContextCommandChildResult result;
    uint32_t bits,masked; int32_t height; int16_t across,along; uint16_t angles[3];
    if(index>=s->pointer_group_count) return 0;
    descriptor=&s->pointer_groups[index].data[3];
    if(!descriptor->data.bytes || !port_field_window_u32(&descriptor->data,2,&bits)) return 0;
    height=signed_long(bits);
    if(height>=0) { *s->error_word=0x28; height=0; }
    height=signed_long((uint32_t)height&UINT32_C(0x7fffffff));
    root->long_10=(uint32_t)height+7u;
    root->geometry->position[1]=(uint32_t)shift_long(height,8)+0x708u;
    root->byte_04|=0xc8; root->word_b8=0;
    root->angle_first=selected->angle_first;
    root->geometry->angle=selected->geometry->angle;
    root->angle_third=selected->angle_third;

    input.record=selected->geometry; input.local[0]=11; input.local[1]=0; input.local[2]=0x68;
    if(!fa18_apply_context_control_child(s->player->effects->context,
        CONTEXT_COMMAND_LOCAL_TO_WORLD,&input,&result)) return 0;
    root->geometry->position[0]=(uint32_t)result.position[0];
    root->geometry->position[2]=(uint32_t)result.position[2];
    root->word_0c=(uint16_t)(((uint32_t)result.position[0]&0x3fffffu)>>8);
    root->word_0e=(uint16_t)(((uint32_t)result.position[2]&0x3fffffu)>>8);
    masked=(uint32_t)result.position[0]&0x1fffffffu;
    across=asr_word((int16_t)(masked>>16),4);
    masked=(uint32_t)result.position[2]&0x1fffffffu;
    along=asr_word((int16_t)(masked>>16),4);
    root->byte_0a=quadrant(across,along);
    across=asr_word(across,2); along=asr_word(along,2);
    root->word_06=(uint16_t)across; root->word_08=(uint16_t)along;
    root->byte_0b=quadrant(across,along);
    angles[0]=root->angle_first; angles[1]=root->geometry->angle; angles[2]=root->angle_third;
    return fa18_publish_native_record_orientation(root,angles,&s->assets->trig);
}

int fa18_place_native_scene_root(FA18NativeScenePlacement *s) {
    FA18FlightCommandState *flight; FA18CommandInput *commands;
    FA18NativeSceneRecord *root,*record; FA18NativeScenePointerGroup chosen;
    int16_t words[8],displacement; ptrdiff_t slot; uint16_t index;
    if(!s || !s->player || !s->player->records || !s->player->flight ||
       !s->player->effects || !s->player->effects->context || !s->assets ||
       !s->pointer_groups || !s->pointer_group_count || !s->recorder ||
       !s->recorder->bytes || !s->recorder->words || s->recorder->word_byte_count<4 ||
       !s->context_record || !s->condition_key_a || !s->condition_key_b ||
       !s->grid_origin_x || !s->grid_origin_z || !s->error_word || !s->target_point ||
       !s->root_ready || !s->bar_e_flag || !s->bar_redraw_e || !s->fire_state ||
       !s->input_source) return 0;
    flight=s->player->flight; commands=flight->commands;
    if(!commands || s->player->records->input!=commands ||
       s->player->effects->context->records!=s->player->records->geometry) return 0;

    s->recorder->byte_cursor=0; s->recorder->word_cursor=0;
    s->recorder->playback_byte=0; s->recorder->playback_word=4;
    s->recorder->record_count=0; s->recorder->playback_count=0;
    s->player->effects->context->recording_write=s->recorder->bytes;
    s->player->effects->context->recording_remaining=s->recorder->byte_count;

    s->player->effects->context->view->update_mask=0xff;
    if(!fa18_prepare_native_scene_player(s->player) || !fa18_reset_native_scene_player(s->player)) return 0;
    root=s->player->records->records; record=root;
    *s->context_record=0; *root->level=9;
    commands->indexed.throttle=72; commands->indexed.throttle_companion=72;
    *s->fire_state=0xfe; root->aircraft->equipment_kind=*s->input_source;
    chosen=*s->input_source==0x10?s->assets->input_10:s->assets->input_other;
    s->pointer_groups[0]=chosen;
    if(!root_kind(root,s->pointer_groups)) return 0;
    *s->root_ready=0;
    root->aircraft->flags&=0xf9ff;
    root->aircraft->secondary_flags&=0xdeff;
    root->aircraft->secondary_flags&=0x7ff7;
    root->aircraft->secondary_flags&=0xefff;
    root->aircraft->secondary_flags|=0x0080;
    root->byte_04&=0x3f; root->byte_04&=0xfd; root->byte_04&=0xf7;
    root->byte_20&=0xfe; root->byte_20&=0xfb; root->byte_20&=0xfd;
    root->byte_7c=0;
    flight->script_count=0; commands->indexed.function_level=0;
    *s->bar_e_flag=0; *s->bar_redraw_e=0;

    for(;;) {
        if(!read_pose(&s->assets->poses,(int8_t)commands->indexed.pose_entry,words)) return 0;
        if(words[0]>=0) return place_positive(s,record,words);
        index=(uint16_t)words[0]&0x7fff;
        displacement=signed_word((uint16_t)(index<<9));
        slot=(record-root)+(displacement/FA18_NATIVE_SCENE_RECORD_BYTES);
        if(slot<0 || slot>=FA18_NATIVE_SCENE_RECORDS) return 0;
        record=root+slot;
        if(!(record->aircraft->flags&0x40)) { commands->indexed.pose_entry=0; continue; }
        return place_negative(s,record,index);
    }
}
