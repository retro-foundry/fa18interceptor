#include "native_record_action_placement.h"
#include <limits.h>
#include <string.h>

static int16_t action_word(uint16_t v) {
    return v<=INT16_MAX?(int16_t)v:(int16_t)((int32_t)v-0x10000);
}
static int32_t action_long(uint32_t v) {
    return v<=INT32_MAX?(int32_t)v:(int32_t)((int64_t)v-INT64_C(0x100000000));
}
static uint32_t action_asr6(uint32_t v) {
    int32_t n=action_long(v);
    return (uint32_t)(n>=0?n/64:(int32_t)-((-(int64_t)n+63)/64));
}
static int action_byte(FA18NativeSceneRecord *r,size_t at,uint8_t value) {
    return fa18_write_native_scene_record(r,at,&value,1);
}
static int action_put_word(FA18NativeSceneRecord *r,size_t at,uint16_t value) {
    uint8_t b[2]={(uint8_t)(value>>8),(uint8_t)value};
    return fa18_write_native_scene_record(r,at,b,2);
}
static int action_valid(const FA18NativeRecordActionPlacement *s,unsigned slot,unsigned companion) {
    return s && s->records && slot<16 && companion<16 && s->assets &&
        s->pointer_groups && s->view_work && s->current_slot && s->viewed_record &&
        s->selector_word && s->primary_count && s->alternate_count &&
        s->warning_causes && s->events && s->space_latch && s->fire_pending &&
        s->secondary_enable && s->limit && s->divisor && s->alert_countdown &&
        s->fire_state && s->mode_changed && s->stores_redraw_a && s->stores_redraw_b && s->scene_redraw;
}
int fa18_load_native_record_action_placement_assets(const FA18Hunks *hunks,
                                                     FA18NativeRecordActionPlacementAssets *assets) {
    const FA18HunkSegment *source;
    if(!assets || !hunks || !hunks->segments || hunks->count<=17) return 0;
    memset(assets,0,sizeof *assets); source=hunks->segments+17;
    if(!source->data || source->size<0xe00) return 0;
    /* Tables and their unchanged adjacent words end at the first relocated
     * instruction operand. Further rows require explicit adjacent owners;
     * unrelocated pointer offsets are not the original numeric word values. */
    assets->offsets_10=(PortFieldWindow){.bytes=source->data+0xdb2,.byte_count=0x4e};
    assets->offsets_other=assets->offsets_10; assets->offsets_other.origin=0x24;
    return fa18_load_native_scene_pointer_group(hunks,0,&assets->primary) &&
        fa18_load_native_scene_pointer_group(hunks,0x14,&assets->alternate);
}
static int place_action(FA18NativeRecordActionPlacement *s,unsigned slot,unsigned companion) {
    FA18NativeSceneRecord *r=s->records->records+slot,*c=s->records->records+companion;
    const PortFieldWindow *table; unsigned i,j; uint8_t stores,index,link,bytes[4];
    int16_t local[3],row; uint32_t delta[3]; uint8_t high,low; uint16_t viewed;
    int alternate=(c->aircraft->weapon_radar&0xf0u)==0x30;
    if(!(c->byte_5f&(alternate?0x0fu:0xf0u))) { r->aircraft->flags&=0xffbf; return 1; }
    if(!port_field_word_pair_valid(s->viewed_record) ||
       !port_read_field_byte(s->viewed_record,&high) ||
       !port_read_field_byte(s->viewed_record+1,&low)) return 0;
    viewed=(uint16_t)(((unsigned)high<<8)|low);
    if((uint16_t)(companion*512u)==viewed) {
        *s->alert_countdown=8; *s->fire_state=0xfb;
    }
    for(i=0;i<41;++i)
        if(!fa18_read_native_scene_record(c,i*4u,bytes,4) ||
           !fa18_write_native_scene_record(r,i*4u,bytes,4)) return 0;
    if(!action_byte(r,5,0)) return 0;
    alternate=(r->aircraft->weapon_radar&0xf0u)==0x30;
    r->aircraft->equipment_kind=(uint8_t)alternate;
    if(!companion) {
        uint16_t *count=alternate?s->alternate_count:s->primary_count;
        ++*count; *s->mode_changed=1;
    }
    /* The source multiplies the slot by 20 in word arithmetic. The descriptor
     * bank is ordinary shared storage; out-of-bank destinations are unavailable. */
    row=action_word((uint16_t)(*s->current_slot*20u));
    if(row<0 || row%20 || (size_t)(row/20)>=s->pointer_group_count) return 0;
    s->pointer_groups[row/20]=alternate?s->assets->alternate:s->assets->primary;
    r->long_56=0; r->word_5a=0;
    if(!action_byte(r,0x64,0)) return 0;
    r->aircraft->secondary_flags&=0xfffb;
    stores=c->byte_5f;
    if((c->aircraft->weapon_radar&0xf0u)==0x30) {
        uint8_t used=(uint8_t)((stores&15u)-1u);
        index=(uint8_t)(used+2u); stores=(uint8_t)((stores&0xf0u)|used);
    } else {
        uint8_t used=(uint8_t)((stores&0xf0u)-0x10u);
        index=(uint8_t)(used>>4); stores=(uint8_t)((stores&15u)|used);
    }
    c->byte_5f=stores; *s->stores_redraw_a=3; *s->stores_redraw_b=3;
    table=c->aircraft->equipment_kind==0x10?&s->assets->offsets_10:&s->assets->offsets_other;
    row=action_word((uint16_t)((int16_t)(int8_t)index*6));
    for(i=0;i<3;++i)
        if(!port_field_window_s16(table,(int32_t)row+(int32_t)i*2,local+i)) return 0;
    s->view_work->carried_axis=(uint32_t)(int32_t)local[1];
    for(i=0;i<3;++i) {
        uint32_t sum=0;
        for(j=0;j<3;++j) sum+=(uint32_t)((int32_t)local[j]*c->geometry->inverse[i][j]);
        delta[i]=action_asr6(sum);
    }
    for(i=0;i<3;++i) r->geometry->position[i]+=delta[i];
    if(!action_put_word(r,0x4c,r->aircraft->equipment_kind?200:150) ||
       !action_put_word(r,0x26,20)) return 0;
    r->aircraft->flags|=0x11c2;
    if(!action_put_word(r,0x76,0)) return 0;
    if(r->aircraft->secondary_flags&0x80) r->long_42=0;
    r->aircraft->secondary_flags&=0xff7f;
    if(!action_put_word(r,0x2c,0xffff)) return 0;
    *s->scene_redraw=0x8c;
    return fa18_read_native_scene_record(c,0x38,&link,1) && action_byte(r,0x38,link);
}
int fa18_place_native_primary_record(FA18NativeRecordActionPlacement *s,
                                       unsigned slot,unsigned companion) {
    if(!action_valid(s,slot,companion)) return 0;
    *s->space_latch=0; *s->fire_pending=1;
    if(*s->warning_causes&0x4000u) { *s->warning_causes&=~UINT32_C(0x4000); *s->events|=8; }
    return place_action(s,slot,companion);
}
int fa18_place_native_secondary_record(FA18NativeRecordActionPlacement *s,
                                         unsigned slot,unsigned companion) {
    uint16_t mask;
    if(!action_valid(s,slot,companion)) return 0;
    if(!*s->secondary_enable) { s->records->aircraft[slot].flags&=0xffbf; return 1; }
    mask=(int8_t)*s->limit>=3?3:(int8_t)*s->limit>=2?5:7;
    if(*s->divisor) mask>>=1;
    if(*s->selector_word&mask) { s->records->aircraft[slot].flags&=0xffbf; return 1; }
    return place_action(s,slot,companion);
}
