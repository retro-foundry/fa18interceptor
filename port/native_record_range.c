#include "native_record_range.h"

static int read_word(const FA18NativeSceneRecord *r,size_t at,uint16_t *v) {
    uint8_t b[2];
    if(!fa18_read_native_scene_record(r,at,b,2)) return 0;
    *v=(uint16_t)(((unsigned)b[0]<<8)|b[1]); return 1;
}
static int write_word(FA18NativeSceneRecord *r,size_t at,uint16_t v) {
    uint8_t b[2]={(uint8_t)(v>>8),(uint8_t)v};
    return fa18_write_native_scene_record(r,at,b,2);
}
static int byte_change(FA18NativeSceneRecord *r,size_t at,uint8_t mask,uint8_t bits) {
    uint8_t b;
    if(!fa18_read_native_scene_record(r,at,&b,1)) return 0;
    b=(uint8_t)((b&mask)|bits);
    return fa18_write_native_scene_record(r,at,&b,1);
}
/* EXT.L precedes SUB.W and SWAP: the incoming coarse sign survives in the
 * swapped low half. BGE after ADD.L consumes N xor V, not the wrapped sign. */
static uint32_t cell_difference(uint16_t coarse,uint16_t fine,uint16_t own_coarse,uint16_t own_fine) {
    uint32_t swapped=((uint32_t)(uint16_t)(coarse-own_coarse)<<16)|(coarse&0x8000u?0xffffu:0u);
    int32_t cell=(int32_t)swapped>>2;
    int16_t detail=(int16_t)(uint16_t)(fine-own_fine);
    uint32_t result=(uint32_t)cell+(uint32_t)(int32_t)detail;
    return (int64_t)cell+detail<0?0u-result:result;
}
int fa18_classify_native_selected_range(FA18NativeRecordRange *s,unsigned slot) {
    FA18NativeSceneRecord *r,*target;
    uint16_t selected,distance; uint8_t counter,mode; uint32_t dx,dy,dz;
    int16_t scalar;
    if(!s || !s->records || slot>=16 || !s->selected_record || !s->current_stride ||
       !s->magnitude || !s->bar_redraw_f) return 0;
    r=s->records->records+slot;
    if((int16_t)r->word_6c>=0x1200 && !(r->byte_7c&0x70u)) {
        if(!*s->bar_redraw_f) {
            *s->bar_redraw_f=1;
            if(!s->ops || !s->ops->tone || !s->ops->tone(s->ops->context,s,4)) return 0;
        }
    } else *s->bar_redraw_f=0;
    selected=*s->selected_record;
    if((int16_t)selected<=0) return 1;
    if(selected%512u || selected/512u>=16) return 0;
    target=s->records->records+selected/512u;
    if(!fa18_read_native_scene_record(r,0x39,&counter,1)) return 0;
    counter=(uint8_t)(counter+0x10u);
    if(!fa18_write_native_scene_record(r,0x39,&counter,1) || !read_word(r,0x4a,&distance)) return 0;
    if((int16_t)distance>=0x480 && ((int16_t)distance>=0x900?0x50:0x20)>=(int8_t)(counter&0xf0u)) return 1;
    counter&=15u;
    if(!fa18_write_native_scene_record(r,0x39,&counter,1)) return 0;
    dx=cell_difference(target->word_06,target->word_0c,r->word_06,r->word_0c);
    dz=cell_difference(target->word_08,target->word_0e,r->word_08,r->word_0e);
    dy=target->long_10-r->long_10;
    if((int64_t)(int32_t)target->long_10-(int32_t)r->long_10<0) dy=0u-dy;
    if((int32_t)dx>0x7f00 || (int32_t)dy>0x7f00 || (int32_t)dz>0x7f00) goto far;
    if(fa18_scene_component_magnitude_window(s->table,(int16_t)dx,(int16_t)dy,(int16_t)dz,&scalar)!=0) return 0;
    *s->magnitude=(uint16_t)scalar;
    if(!write_word(r,0x4a,(uint16_t)scalar)) return 0;
    r->byte_04|=1;
    if(scalar>0x36c0) goto distant_class;
    if(!fa18_read_native_scene_record(r,0x7a,&mode,1)) return 0;
    if(scalar>0x1e00) { if(mode==4) mode=3; }
    else if(mode==3) mode=4;
    if(!fa18_write_native_scene_record(r,0x7a,&mode,1)) return 0;
    if(!*s->current_stride) return 1;
    if(scalar>0x1800) goto far;
    r->aircraft->weapon_radar=(uint8_t)((r->aircraft->weapon_radar&15u)|
        (scalar<=0x300?0x10u:scalar<=0xc00?0x20u:0x30u));
    return 1;
far:
    if(!write_word(r,0x4a,0x7fff)) return 0;
distant_class:
    if(*s->current_stride) return byte_change(r,0x63,0xff,0xf0);
    return 1;
}
