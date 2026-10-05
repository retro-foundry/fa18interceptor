#include "native_record_dispatch.h"

static int dispatch_valid(const FA18NativeRecordDispatch *s) {
    return s && s->view && s->view->records && s->view_work && s->range &&
        s->range->records==s->view->records && s->range->current_stride==s->view->current_stride &&
        s->range->selected_record==s->view->selected_record && s->view->post_input_event &&
        s->view->current_slot && s->publication && s->publication->records==s->view->records &&
        s->publication->target_record==s->target_slot && s->selected_controls &&
        s->cell_only && s->track_enable && s->sequence_phase && s->sequence_step &&
        s->detail_clear && s->scene_redraw && s->control_choice[0] && s->control_choice[1] && s->target_slot;
}
static uint16_t dispatch_half(uint16_t v,unsigned count) {
    uint16_t fill=(v&0x8000u)?(uint16_t)(0xffffu<<(16-count)):0;
    return (uint16_t)((v>>count)|fill);
}
static uint32_t dispatch_shift(uint32_t v,unsigned count) {
    uint32_t fill=(v&0x80000000u)?UINT32_MAX<<(32-count):0;
    return (v>>count)|fill;
}
static int dispatch_read_byte(const FA18NativeSceneRecord *r,unsigned at,uint8_t *v) {
    return fa18_read_native_scene_record(r,at,v,1);
}
static int dispatch_write_byte(FA18NativeSceneRecord *r,unsigned at,uint8_t v) {
    return fa18_write_native_scene_record(r,at,&v,1);
}
static int dispatch_word(FA18NativeSceneRecord *r,unsigned at,uint16_t *v) {
    uint8_t b[2]; if(!fa18_read_native_scene_record(r,at,b,2)) return 0;
    *v=(uint16_t)(((unsigned)b[0]<<8)|b[1]); return 1;
}
static int dispatch_write_word(FA18NativeSceneRecord *r,unsigned at,uint16_t v) {
    uint8_t b[2]={(uint8_t)(v>>8),(uint8_t)v}; return fa18_write_native_scene_record(r,at,b,2);
}
static int dispatch_write_long(FA18NativeSceneRecord *r,unsigned at,uint32_t v) {
    uint8_t b[4]={(uint8_t)(v>>24),(uint8_t)(v>>16),(uint8_t)(v>>8),(uint8_t)v};
    return fa18_write_native_scene_record(r,at,b,4);
}
static int dispatch_copy_point(FA18NativeSceneRecord *r,const FA18NativeSceneRecord *other) {
    return dispatch_write_word(r,0x2c,other->word_06) && dispatch_write_word(r,0x2e,other->word_08) &&
        dispatch_write_word(r,0x30,other->word_0c) && dispatch_write_word(r,0x32,other->word_0e) &&
        dispatch_write_long(r,0x34,other->long_10);
}
static uint32_t dispatch_difference(uint32_t target,uint32_t current) {
    uint32_t d=target-current;
    return (int64_t)(int32_t)target-(int32_t)current<0?0u-d:d;
}
int fa18_select_native_control_target(FA18NativeRecordDispatch *s,unsigned slot,uint8_t choice) {
    FA18NativeSceneRecord *r; uint16_t coarse[2]; uint32_t point[3],world[3],threshold;
    int32_t offset; unsigned i;
    if(!dispatch_valid(s) || slot>=16) return 0;
    r=s->view->records->records+slot;
    offset=(int32_t)(int16_t)(uint16_t)((uint16_t)(int16_t)(int8_t)(uint8_t)(choice-1u)<<6);
    for(i=0;i<2;++i) {
        if(!port_field_window_u16(s->selected_controls,offset+48+2*(int32_t)i,coarse+i)) return 0;
    }
    for(i=0;i<2;++i) if(!dispatch_write_word(r,0x2c+2*i,coarse[i])) return 0;
    /* The coarse words are zero-extended before shifting and swapping. */
    world[0]=(uint32_t)(uint16_t)(coarse[0]<<6)<<16;
    world[2]=(uint32_t)(uint16_t)(coarse[1]<<6)<<16;
    for(i=0;i<3;++i) if(!port_field_window_u32(s->selected_controls,offset+4*(int32_t)i,point+i)) return 0;
    world[0]+=point[0]; world[1]=point[1]; world[2]+=point[2];
    s->view_work->carried_axis=point[1];
    if(!dispatch_write_word(r,0x30,(uint16_t)dispatch_shift(point[0],8)) ||
       !dispatch_write_word(r,0x32,(uint16_t)dispatch_shift(point[2],8)) ||
       !dispatch_write_long(r,0x34,dispatch_shift(point[1],8))) return 0;
    threshold=(dispatch_shift(point[0],8)&0xffff0000u)|0x4800u;
    for(i=0;i<3;++i) {
        uint32_t d=dispatch_difference(world[i],r->geometry->position[i]);
        if(i==1) s->view_work->carried_axis=d;
        if((int32_t)d>(int32_t)threshold) {
            if(r->aircraft->secondary_flags&1u) r->byte_20|=32;
            return 1;
        }
    }
    r->aircraft->secondary_flags|=1; return 1;
}
int fa18_track_native_record_target(FA18NativeRecordDispatch *s,unsigned slot) {
    FA18NativeSceneRecord *r,*other; uint8_t link,choice;
    unsigned target;
    if(!dispatch_valid(s) || slot>=16) return 0;
    r=s->view->records->records+slot; r->aircraft->flags|=1;
    if(!dispatch_read_byte(r,0x38,&link)) return 0;
    target=link&127u;
    if(!target) {
        choice=*s->control_choice[r->aircraft->equipment_kind==0?0:1];
        if(choice) return fa18_select_native_control_target(s,slot,choice);
    }
    if(target>=16) return 0;
    other=s->view->records->records+target; s->view_work->viewer=other;
    if(!(r->aircraft->flags&64u)) r->aircraft->flags&=0xff7fu;
    return dispatch_copy_point(r,other);
}
static int dispatch_approach(FA18NativeSceneRecord *r,uint16_t limit) {
    uint16_t angle=r->word_6c;
    if(!dispatch_write_long(r,0x34,0)) return 0;
    if((int16_t)angle<=(int16_t)limit) {
        angle=(uint16_t)(angle+0x240u); if((int16_t)angle>(int16_t)limit) angle=limit;
    } else {
        angle=(uint16_t)(angle-0x240u); if((int16_t)angle<(int16_t)limit) angle=limit;
    }
    r->word_6c=r->word_6e=angle; return 1;
}
static int dispatch_clear(FA18NativeRecordDispatch *s,FA18NativeSceneRecord *r,int *decision) {
    r->aircraft->flags=0; *s->scene_redraw=12; *decision=0; return 1;
}
int fa18_dispatch_native_unclassified_record(FA18NativeRecordDispatch *s,unsigned slot,int *decision) {
    FA18NativeSceneRecord *r; uint16_t life; int old;
    if(!dispatch_valid(s) || slot>=16 || !decision) return 0;
    r=s->view->records->records+slot; *decision=0;
    if(!(r->aircraft->flags&64u)) return 1;
    if(!*s->cell_only && !*s->view->post_input_event) {
        if(!dispatch_word(r,0x4c,&life)) return 0;
        old=(int16_t)life;
        if(!dispatch_write_word(r,0x4c,(uint16_t)(life-1u))) return 0;
        if(old-1<=0) return dispatch_clear(s,r,decision);
    }
    *decision=1;
    if(!dispatch_word(r,0x4c,&life)) return 0;
    if((r->aircraft->flags&0x400u) || (int16_t)life<50) {
        if(!(r->aircraft->flags&0x400u) && (int16_t)life<12) r->aircraft->flags|=0x400u;
        return dispatch_approach(r,0x3000);
    }
    r->word_6c=(uint16_t)(r->word_6c+0x1e0u);
    if((int16_t)r->word_6c>0x4200) r->word_6c=0x4200;
    r->word_6e=r->word_6c;
    if((r->aircraft->flags&2u) && (r->aircraft->flags&8u))
        return fa18_link_native_record_view(s->view,slot,s->view_work);
    if(r->aircraft->flags&2u) r->aircraft->flags&=0xfffeu;
    return fa18_track_native_record_target(s,slot);
}
static int dispatch_motion(FA18NativeRecordDispatch *s,unsigned slot,unsigned companion,int *decision) {
    FA18NativeSceneRecord *r=s->view->records->records+slot;
    uint32_t motion[3]={r->long_3e,r->long_42,r->long_46}; uint16_t life; unsigned rate;
    *decision=1;
    if(*s->cell_only || *s->view->post_input_event) return 1;
    if(r->aircraft->flags&0x400u) {
        s->view->records->aircraft[companion].secondary_flags|=32;
        return dispatch_clear(s,r,decision);
    }
    if(!dispatch_word(r,0x4c,&life)) return 0;
    if(r->aircraft->flags&0x8000u) {
        if((int16_t)life>0) {
            life=(int16_t)life<5?(uint16_t)(life-1u):4;
            if(!dispatch_write_word(r,0x4c,life)) return 0;
        }
        if((int16_t)life<=0 && r->aircraft->equipment_kind==48 && !*s->sequence_phase) {
            *s->sequence_phase=2; *s->sequence_step=4;
        }
        if((int32_t)motion[0]<192) motion[0]+=12;
        motion[1]=0; motion[2]+=(int32_t)motion[2]<=0?1u:UINT32_MAX;
    } else {
        int check_reset=(int16_t)life<0,reset=0;
        life=(uint16_t)(life+1u);
        if(!dispatch_write_word(r,0x4c,life)) return 0;
        rate=check_reset?6u:(int16_t)life<3?2u:3u;
        if(!check_reset && (int16_t)life>=3 && r->aircraft->equipment_kind==48) {
            uint16_t flags=s->view->records->aircraft[0].flags;
            s->view_work->carried_axis=(s->view_work->carried_axis&0xffff0000u)|flags;
            if(!(flags&64u) || ((flags&4u) && (int16_t)life<5)) reset=1;
            else if(!(flags&4u)) check_reset=1;
        }
        if(check_reset) {
            uint16_t flags=s->view->records->aircraft[0].flags;
            s->view_work->carried_axis=(s->view_work->carried_axis&0xffff0000u)|(flags&0x600u);
            if((flags&0x600u) || (int32_t)r->geometry->position[1]<0x2000 || (int16_t)life>=20) reset=1;
        }
        if(reset) {
            FA18CommandInput *input=s->view->records->input;
            if(!input) return 0;
            if(input->indexed.origin_gate_b && (int16_t)*s->view->current_slot!=(int16_t)*s->target_slot) {
                uint32_t event;
                if(!fa18_publish_native_context_record(s->publication,motion[0],(int16_t)*s->view->current_slot,
                    &s->view_work->carried_axis,&event)) return 0;
                *s->detail_clear=0;
                if(!fa18_publish_native_context_detail(s->publication,event,&s->view_work->carried_axis,&event)) return 0;
            }
        }
        { uint32_t across=(int32_t)motion[0]<0?0u-motion[0]:motion[0];
          uint32_t along=(int32_t)motion[2]<0?0u-motion[2]:motion[2];
          uint32_t maximum=(int32_t)along<(int32_t)across?across:along;
          uint16_t threshold=(uint16_t)(0u-life);
          int positive=(int16_t)threshold+240;
          threshold=positive<=0?0:(uint16_t)(threshold+240u);
          s->view_work->carried_axis=(uint32_t)(int32_t)(int16_t)threshold;
          if((int32_t)maximum>=(int32_t)s->view_work->carried_axis) {
              uint32_t delta=dispatch_shift(motion[0],rate);
              s->view_work->carried_axis=delta;
              motion[0]-=delta; motion[2]-=dispatch_shift(motion[2],rate);
          } }
        if((int16_t)life>=3 && (int32_t)motion[1]>0) motion[1]-=960;
        else if((int32_t)motion[1]>-768) motion[1]-=60;
        else motion[1]=dispatch_shift(motion[1]-768u,1);
    }
    r->long_3e=motion[0]; r->long_42=motion[1]; r->long_46=motion[2];
    { uint32_t maximum=(int32_t)motion[0]<0?0u-motion[0]:motion[0]; unsigned i;
      for(i=1;i<3;++i) { uint32_t v=(int32_t)motion[i]<0?0u-motion[i]:motion[i];
          if((int32_t)v>(int32_t)maximum) maximum=v; }
      r->word_6e=(uint16_t)dispatch_shift(maximum,2); }
    return 1;
}
static int dispatch_heading(FA18NativeRecordDispatch *s,unsigned slot) {
    FA18NativeSceneRecord *r=s->view->records->records+slot,*other;
    uint16_t speed,turn,range; uint8_t link,mode,control; unsigned target;
    int trend=0,negative;
    if(!fa18_classify_native_view_range(s->range,slot,&s->view_work->carried_axis) ||
       !dispatch_read_byte(r,0x38,&link)) return 0;
    turn=r->word_6c;
    if(link==255) {
        if(r->aircraft->equipment_kind!=20) goto default_heading;
        if(r->aircraft->secondary_flags&0x80u) {
            speed=r->word_6c;
            if((int16_t)speed<0x100) { *r->level=0; goto publish; }
            speed=dispatch_half(speed,1); goto blend;
        }
        speed=0x1680;
        if((int32_t)r->long_10<=0x6c0) {
            if(r->byte_7c&128u) r->byte_7c^=128;
            speed=0x600;
        }
        goto minimum;
    }
    target=link&127u; if(target>=16) return 0;
    if(!(r->aircraft->flags&64u)) {
        if(!dispatch_write_byte(r,0x38,128)) return 0;
        target=0;
    }
    other=s->view->records->records+target; s->view_work->companion=other; speed=other->word_6c;
    if(!dispatch_word(r,0x4a,&range)) return 0;
    if(r->byte_04&32u) {
        if((int16_t)range<=0x240) goto double_heading;
        if((int16_t)range>=0x600) goto reduce;
        goto minimum;
    }
    if(!(r->aircraft->secondary_flags&0x100u)) {
        if(!dispatch_read_byte(r,5,&mode)) return 0;
        if(mode && mode!=1) {
            if((int16_t)range<=0x360) goto minimum;
            if(mode==6) goto double_heading;
            goto reduce;
        }
    }
    if(!dispatch_read_byte(r,0x64,&control)) return 0;
    if((int16_t)range<0x180) goto reduce;
    if((control&0x60u)==0x60u) goto double_heading;
    if((int16_t)range<=0x300) goto reduce;
    goto minimum;
double_heading:
    if((int16_t)range>=0x300) goto default_heading;
    speed=(uint16_t)(speed+speed);
    if((int16_t)speed<=0x1bc0) goto minimum;
default_heading:
    speed=r->aircraft->equipment_kind==21?0x1ec0:(r->aircraft->flags&8u)?0x15c0:0x1d40;
    goto minimum;
reduce:
    { uint16_t decrement=dispatch_half(speed,3);
      if((int16_t)decrement>=0x420) decrement=0x420;
      speed=(uint16_t)(speed-decrement); }
minimum:
    if((int16_t)speed<=0x900) speed=0x900;
blend:
    speed=dispatch_half((uint16_t)(speed+turn),1);
    negative=(int16_t)speed-(int16_t)turn<0;
    { uint16_t difference=(uint16_t)(speed-turn);
      uint16_t step=negative?0xff70:0x90;
      s->view_work->carried_axis=(s->view_work->carried_axis&0xffff0000u)|step;
      if(negative) difference=(uint16_t)(0u-difference);
      if((int16_t)difference>=0x90) speed=(uint16_t)(turn+step); }
    if((int16_t)speed!=(int16_t)r->word_6c) trend=(int16_t)speed<(int16_t)r->word_6c?2:1;
publish:
    r->aircraft->stick=(uint8_t)((r->aircraft->stick&0xfcu)|(unsigned)trend); return 1;
}
int fa18_dispatch_native_record(FA18NativeRecordDispatch *s,unsigned slot,unsigned companion,int *decision) {
    FA18NativeSceneRecord *r; uint8_t kind; uint16_t life,angle,coarse;
    if(!dispatch_valid(s) || slot>=16 || companion>=16 || !decision) return 0;
    r=s->view->records->records+slot; *decision=1;
    s->view_work->companion=s->view->records->records+companion;
    if(*s->view->post_input_event) return 1;
    kind=r->aircraft->equipment_kind&240u;
    if(!kind) return fa18_dispatch_native_unclassified_record(s,slot,decision);
    if(kind==48) return dispatch_motion(s,slot,companion,decision);
    if(kind==32) return 1;
    if(!dispatch_word(r,0x4c,&life) || !dispatch_write_word(r,0x4c,(uint16_t)(life-1u))) return 0;
    if(!(r->aircraft->flags&0x1000u) || !*s->track_enable) return 1;
    if(r->byte_20&2u) {
        if(!dispatch_write_long(r,0x34,0) || !dispatch_write_word(r,0x2c,r->word_06) ||
           !dispatch_write_word(r,0x2e,r->word_08) || !dispatch_write_word(r,0x30,r->word_0c) ||
           !dispatch_write_word(r,0x32,r->word_0e)) return 0;
        angle=r->word_6c;
        if((int16_t)angle<=0x1200) { angle=(uint16_t)(angle+0x90u); if((int16_t)angle>0x1200) angle=0x1200; }
        else { angle=(uint16_t)(angle-0x90u); if((int16_t)angle<0x1200) angle=0x1200; }
        r->word_6c=r->word_6e=angle; r->aircraft->secondary_flags&=0xdff7u; return 1;
    }
    if(!(r->aircraft->secondary_flags&0x100u)) {
        if(!dispatch_word(r,0x2c,&coarse)) return 0;
        if((int16_t)coarse>=0 && !dispatch_heading(s,slot)) return 0;
    }
    return fa18_update_native_record_view(s->view,slot,s->view_work);
}
