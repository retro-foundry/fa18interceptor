#include "native_record_control.h"

static int byte_at(const FA18NativeSceneRecord *r,size_t at,uint8_t *v) {
    return fa18_read_native_scene_record(r,at,v,1);
}
static int store_byte(FA18NativeSceneRecord *r,size_t at,uint8_t v) {
    return fa18_write_native_scene_record(r,at,&v,1);
}
static int valid(const FA18NativeRecordControl *s) {
    return s && s->records && s->assets && s->view_work && s->magnitude_bias && s->stream_position &&
        s->current_slot && s->target_slot && s->magnitude_alert && s->mode && s->stream_index &&
        s->stream_pending && s->stream_gate && s->post_input_event && s->playback_enable &&
        s->origin_enable && s->sequence_phase && s->view_selector && s->stream_view_state && s->scene_redraw;
}
static uint8_t next_index(uint8_t index) {
    index=(uint8_t)(index+1u); return (int8_t)index>7?1:index;
}
static int16_t row_offset(uint8_t index) {
    return (int16_t)(uint16_t)((uint16_t)(int16_t)(int8_t)index<<3);
}
static int message(FA18NativeRecordControl *s,uint16_t code) {
    return s->ops && s->ops->message && s->ops->message(s->ops->context,s,code);
}
static int row_message(FA18NativeRecordControl *s,uint8_t index,unsigned field) {
    uint16_t code;
    return port_field_window_u16(&s->assets->messages,(int32_t)row_offset(index)+(int32_t)field,&code) && message(s,code);
}
static int initialize_scene(FA18NativeRecordControl *s,unsigned slot) {
    FA18NativeSceneRecord *r=s->records->records+slot;
    r->aircraft->flags=0;
    if(!s->ops || !s->ops->initialize_scene ||
       !s->ops->initialize_scene(s->ops->context,s,slot,&s->view_work->carried_axis)) return 0;
    r->aircraft->secondary_flags|=0x100;
    *r->level=0x60; return 1;
}
int fa18_next_native_record_stream(FA18NativeRecordControl *s,unsigned slot) {
    uint8_t chosen; uint16_t code; uint32_t value;
    if(!valid(s) || slot>=16) return 0;
    *s->stream_index=next_index(*s->stream_index); *s->sequence_phase=0; *s->stream_position=0;
    s->records->aircraft[slot].stick=0;
    chosen=(int8_t)*s->stream_index>0?*s->stream_index:1;
    value=*s->current_slot;
    if(*s->current_slot==*s->target_slot) {
        int16_t at=(int16_t)(uint16_t)(*s->current_slot<<3);
        if(!port_field_window_u32(&s->assets->metadata,at,&value)) return 0;
        *s->view_selector=(uint8_t)value;
    }
    if(*s->mode==2 && *s->origin_enable) {
        uint8_t origin=(uint8_t)((uint16_t)value>>8);
        *s->origin_enable=(int8_t)origin>0?origin:0xff;
    }
    *s->scene_redraw=0xff;
    if(chosen==4 && (int32_t)s->records->geometry[slot].position[1]<=0x100000) code=53;
    else if(!port_field_window_u16(&s->assets->messages,(int32_t)row_offset(chosen)+2,&code)) return 0;
    return message(s,code);
}
static const FA18NativeControlStream *selected_stream(const FA18NativeRecordControl *s) {
    int32_t index=(int8_t)*s->stream_index;
    int64_t position=(int64_t)index-s->assets->first_index;
    if(!s->assets->streams || position<0 || (uint64_t)position>=s->assets->count) return NULL;
    return s->assets->streams+(size_t)position;
}
static int play_stream(FA18NativeRecordControl *s,unsigned slot) {
    FA18NativeSceneRecord *r=s->records->records+slot;
    const FA18NativeControlStream *stream; uint8_t code,mode; int special=0;
    if(*s->post_input_event || !(r->aircraft->secondary_flags&0x100u)) return 1;
    if(*s->mode==2) {
        uint16_t old;
        if(!*s->playback_enable) return 1;
        old=s->records->aircraft[0].secondary_flags;
        s->records->aircraft[0].secondary_flags&=0xf7ff;
        if(old&0x800u) {
            if(!initialize_scene(s,slot) || !fa18_next_native_record_stream(s,slot)) return 0;
            special=1;
        }
    }
    for(;;) {
        stream=selected_stream(s); if(!stream) return 0;
        if(!special && (int16_t)row_offset(*s->stream_index)<=0 && *s->mode==2) {
            *r->level=0x60; *s->stream_view_state=0xfe;
            if(stream->kind==FA18_CONTROL_STREAM_EMPTY) return 1;
            if(stream->kind==FA18_CONTROL_STREAM_CODE || (int16_t)*s->stream_position>20) goto stream_end;
        } else {
            if(stream->kind==FA18_CONTROL_STREAM_EMPTY) return 1;
            if(stream->kind==FA18_CONTROL_STREAM_CODE) {
                if(!*s->stream_position) {
                    r->aircraft->secondary_flags&=0xf7ff;
                    if(!store_byte(r,5,stream->code)) return 0;
                    *s->stream_position=1; return 1;
                }
                if(!byte_at(r,5,&mode)) return 0;
                if(mode) return 1;
                r->aircraft->stick&=0xc3;
                goto stream_end;
            }
        }
        if(stream->kind!=FA18_CONTROL_STREAM_COMMANDS ||
           !port_field_window_byte(&stream->commands,(int16_t)*s->stream_position,&code)) return 0;
        if((int8_t)code<0) goto stream_end;
        r->aircraft->secondary_flags&=0xf7ff; r->aircraft->stick=code;
        *s->stream_position=(uint16_t)(*s->stream_position+1u);
        if((int16_t)*s->stream_position<0x1fd) return 1;
        if(!message(s,29)) return 0;
        r->aircraft->secondary_flags&=0xfeff; return 1;
stream_end:
        if(*s->mode!=2) {
            uint16_t old=r->aircraft->secondary_flags;
            r->aircraft->secondary_flags&=0xf7ff;
            if(!(old&0x800u)) {
                if((int8_t)*s->stream_pending<0) return 1;
                *s->stream_pending=0xff;
                return row_message(s,next_index(*s->stream_index),0);
            }
        }
        *s->stream_pending=0;
        if((int8_t)*s->stream_index>=7 && !initialize_scene(s,slot)) return 0;
        if(!fa18_next_native_record_stream(s,slot)) return 0;
        special=0;
    }
}
int fa18_update_native_record_stream(FA18NativeRecordControl *s,unsigned slot) {
    FA18NativeSceneRecord *r;
    if(!valid(s) || slot>=16) return 0;
    r=s->records->records+slot;
    if(*s->mode==125 && !(r->aircraft->secondary_flags&0x100u)) {
        if(!store_byte(r,5,0)) return 0;
        if(!s->records->records[0].word_6e) return 1;
        r->aircraft->secondary_flags|=0x100; *s->stream_position=0;
    }
    return play_stream(s,slot);
}
static int clone_record(FA18NativeRecordControl *s,unsigned slot,unsigned companion) {
    FA18NativeSceneRecord *r=s->records->records+slot,*source=s->records->records+companion;
    uint8_t classification,b[4]; unsigned i,j; uint32_t world[3],sum,axis;
    const int16_t local[3]={9,1,-97};
    *s->stream_index=*s->stream_pending==0xff?*s->stream_index:next_index(*s->stream_index);
    if(!row_message(s,next_index(*s->stream_index),0)) return 0;
    *s->stream_pending=0xfe;
    classification=r->aircraft->weapon_radar;
    for(i=0;i<41;++i) {
        if(!fa18_read_native_scene_record(source,4*i,b,4) || !fa18_write_native_scene_record(r,4*i,b,4)) return 0;
    }
    r->aircraft->weapon_radar=classification;
    if(!store_byte(r,0x5e,0)) return 0;
    r->aircraft->equipment_kind=17; r->aircraft->secondary_flags&=0xfeff;
    if(!store_byte(r,5,0)) return 0;
    r->aircraft->stick=0;
    for(i=0;i<3;++i) {
        sum=0;
        for(j=0;j<3;++j) sum+=(uint32_t)((int32_t)local[j]*source->geometry->inverse[i][j]);
        world[i]=(uint32_t)((int32_t)sum>>4)+source->geometry->position[i];
    }
    for(i=0;i<3;++i) r->geometry->position[i]=world[i];
    axis=(uint32_t)((int32_t)world[2]>>8);
    s->view_work->carried_axis=(axis&0xffff0000u)|(axis&0x3fffu);
    r->word_06=(uint16_t)((int16_t)(world[0]>>16)>>6); r->word_08=(uint16_t)((int16_t)(world[2]>>16)>>6);
    r->word_0c=(uint16_t)((world[0]>>8)&0x3fff); r->word_0e=(uint16_t)((world[2]>>8)&0x3fff);
    r->long_10=(uint32_t)((int32_t)world[1]>>8); *s->scene_redraw=0xff; return 1;
}
int fa18_update_native_record_control(FA18NativeRecordControl *s,unsigned slot,unsigned companion) {
    FA18NativeSceneRecord *r; int16_t term; uint16_t value,old;
    if(!valid(s) || slot>=16 || companion>=16) return 0;
    r=s->records->records+slot;
    value=(uint16_t)(r->long_56>>16);
    value=(uint16_t)(value+(int16_t)((int16_t)value>>3)); value=(uint16_t)(value+*s->magnitude_bias);
    term=(int16_t)value>>3; if(term<0) term=(int16_t)(uint16_t)(0u-(uint16_t)term);
    if(term>=78) {
        if(!*s->magnitude_alert) {
            *s->magnitude_alert=1;
            if(!s->ops || !s->ops->tone || !s->ops->tone(s->ops->context,s,8)) return 0;
        }
    } else *s->magnitude_alert=0;
    if(*s->mode==125) {
        old=r->aircraft->secondary_flags; r->aircraft->secondary_flags&=0xf7ff;
        if((old&0x800u) && !clone_record(s,slot,companion)) return 0;
    }
    if((int8_t)*s->stream_gate<0 || *s->post_input_event) return 1;
    return play_stream(s,slot);
}
