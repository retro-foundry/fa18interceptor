#include "native_record_pose.h"
#include <string.h>

static int mode_byte(FA18NativeSceneRecord *r,uint8_t *value) {
    return fa18_read_native_scene_record(r,5,value,1);
}
static int put_mode(FA18NativeSceneRecord *r,uint8_t value) {
    return fa18_write_native_scene_record(r,5,&value,1);
}
static int timer(FA18NativeSceneRecord *r,uint16_t *value) {
    uint8_t b[2];
    if(!fa18_read_native_scene_record(r,0x4c,b,2)) return 0;
    *value=(uint16_t)(((unsigned)b[0]<<8)|b[1]); return 1;
}
static int put_timer(FA18NativeSceneRecord *r,uint16_t value) {
    uint8_t b[2]={(uint8_t)(value>>8),(uint8_t)value};
    return fa18_write_native_scene_record(r,0x4c,b,2);
}
static int consume(FA18NativeRecordPose *s,FA18NativeRecordPoseChild child,
        FA18NativeRecordPoseInput *in,FA18NativeRecordPoseResult *out) {
    return s->ops && s->ops->consume && s->ops->consume(s->ops->context,s,child,in,out);
}
static int message(FA18NativeRecordPose *s,FA18NativeRecordPoseInput *in,uint16_t code) {
    FA18NativeRecordPoseResult out={0}; in->choice=code;
    return consume(s,FA18_POSE_MESSAGE,in,&out);
}
static uint16_t coarse(uint32_t value) { return (uint16_t)((int16_t)(value>>16)>>6); }
static uint32_t rotate_cell(uint32_t value) {
    value=(value<<16)|(value>>16); return (value<<4)|(value>>28);
}
static uint8_t cell(uint16_t x,uint16_t z) {
    return (uint8_t)((uint16_t)(3u-x)+4u*(uint16_t)(3u-z));
}
static int near_grid(uint16_t coordinate,uint16_t origin) {
    int32_t difference=(int16_t)coordinate-(int16_t)origin;
    uint16_t value=(uint16_t)difference;
    if(difference<0) value=(uint16_t)(0u-value);
    return (int16_t)value<=2;
}
static void publish(FA18NativeSceneRecord *r) {
    r->word_0c=(uint16_t)((r->geometry->position[0]&0x3fffffu)>>8);
    r->word_0e=(uint16_t)((r->geometry->position[2]&0x3fffffu)>>8);
    r->long_10=(uint32_t)((int32_t)r->geometry->position[1]>>8);
}
static int valid(const FA18NativeRecordPose *s,unsigned slot) {
    return s && slot<16 && s->records && s->records->records[slot].aircraft &&
        s->records->records[slot].geometry && s->current_slot && s->current_stride &&
        s->target_slot && s->selector_word && s->matrix_control && s->shown_message &&
        s->grid_x && s->grid_z && s->error_word && s->collision_slot && s->damage_count &&
        s->events && s->cell_only && s->post_input_event && s->origin_enable &&
        s->activity_count && s->scene_redraw && s->bar_redraw && s->view_decay &&
        s->collision_enable && s->collision_inhibit && s->cockpit_a && s->cockpit_b &&
        s->collision_report && s->mission_failure && s->failure_view && s->request_flag && s->request_clear &&
        s->history_count && s->history_index;
}
static int history_point(FA18NativeRecordPose *s,int32_t at,const uint32_t point[3]) {
    unsigned i,j;
    for(i=0;i<3;++i) for(j=0;j<4;++j) {
        int64_t position=(int64_t)s->history->origin+at+4*i+j;
        if(position<0 || (uint64_t)position>=s->history->byte_count ||
           !port_write_field_byte(s->history->fields+(size_t)position,(uint8_t)(point[i]>>(24-8*j)))) return 0;
    }
    return 1;
}
int fa18_update_native_record_motion_history(FA18NativeRecordPose *s) {
    FA18NativeSceneRecord *r; unsigned slot; uint8_t value,old; int32_t at;
    if(!s || !s->records || !s->current_stride || !s->collision_slot ||
       !s->post_input_event || !s->history_count || !s->history_index) return 0;
    if(*s->current_stride!=*s->collision_slot || *s->post_input_event) return 1;
    slot=*s->current_stride/512u;
    if((*s->current_stride%512u) || slot>=16 || !s->history || !s->history->fields) return 0;
    r=s->records->records+slot;
    if(!r->geometry || !r->aircraft) return 0;
    at=(int8_t)*s->history_index*12;
    if(!history_point(s,at,r->geometry->position)) return 0;
    if((int8_t)*s->history_count<5 && !history_point(s,at+12,r->geometry->position)) return 0;
    *s->history_index=(uint8_t)(*s->history_index+1u);
    if((int8_t)*s->history_index>5) *s->history_index=0;
    if((int8_t)*s->history_count<6) ++*s->history_count;
    if(r->word_6e && (r->aircraft->secondary_flags&0x1000u)) {
        value=(uint8_t)(*s->history_count-1u);
        if((int8_t)value>=5) value=4;
    } else {
        if(!fa18_read_native_scene_record(r,0x3d,&old,1)) return 0;
        value=old?(uint8_t)(old-1u):0;
        if((int8_t)old<=1) { *s->history_count=0; *s->history_index=0; }
    }
    return fa18_write_native_scene_record(r,0x3d,&value,1);
}
int fa18_update_native_record_pose(FA18NativeRecordPose *s,unsigned slot) {
    FA18NativeSceneRecord *r; FA18FlightCommandRecord *a;
    FA18NativeRecordPoseInput in={0}; FA18NativeRecordPoseResult out={0};
    uint32_t rates[3]={0,0,0},x,z,value; uint16_t old_flags,countdown,status=0,choice=17;
    uint8_t mode,kind,lo,hi; int64_t sum; unsigned i;
    if(!valid(s,slot)) return 0;
    r=s->records->records+slot; a=r->aircraft; in.slot=slot;
    if(*s->current_slot && *s->cell_only) {
        uint16_t angles[3];
        if(!(a->flags&0x40u)) return 1;
        angles[0]=r->angle_first; angles[1]=r->geometry->angle; angles[2]=r->angle_third;
        if(!fa18_publish_native_record_inverse_with_axis(r,angles,s->trig,NULL)) return 0;
        x=rotate_cell((uint32_t)(int32_t)(int16_t)r->word_0c);
        z=rotate_cell((uint32_t)(int32_t)(int16_t)r->word_0e);
        r->byte_0a=cell((uint16_t)x,(uint16_t)z); return 1;
    }
    if(*s->post_input_event) return 1;
    if(*s->current_slot) {
        if((*s->selector_word&31u)==*s->current_slot &&
           !consume(s,FA18_POSE_SELECTED_RECORD,&in,&out)) return 0;
    } else if(!*s->origin_enable) goto control_gates;
    old_flags=a->flags;
    if(old_flags&0x400u) {
        if(!timer(r,&countdown)) return 0;
        if((int16_t)countdown>0) goto clear_expiry;
        a->flags&=0xfbff; kind=a->equipment_kind;
        if(kind==21 || !(kind&0xf0u)) goto expire;
        if((kind&0xf0u)==0x10 && !(a->flags&8u) && !(a->secondary_flags&0x80u)) ++*s->activity_count;
        if(*s->current_slot && !(a->secondary_flags&0x80u) && !(a->flags&0x8000u)) goto clear_expiry;
expire:
        a->flags&=0xffbf;
        if(!put_timer(r,20)) return 0;
        a->flags&=0xbfff; a->flags&=0xdfff; goto record_timer;
clear_expiry:
        a->flags&=0xdfff;
    }
    if(old_flags&0x1000u) goto control_gates;
    if(!(old_flags&0x100u)) goto zero_rates;
control_gates:
    if(!mode_byte(r,&mode)) return 0;
    if((a->secondary_flags&0x100u) && !mode) {
        a->secondary_flags|=0x10; goto record_controls;
    }
    if(!(*s->matrix_control&0x40u)) goto matrix;
    if(*s->current_slot && !consume(s,FA18_POSE_RECORD_ACTION,&in,&out)) return 0;
record_controls:
    if(!consume(s,FA18_POSE_RECORD_CONTROLS,&in,&out)) return 0;
    if(*s->current_slot) goto record_warning;
    value=r->long_42;
    if((int32_t)value<0) {
        value=0u-value;
        if(r->byte_20&2u) goto action_warning;
        if((int16_t)r->angle_first>0x3840 || (int16_t)r->angle_first<0x1e0) goto height_warning;
        value<<=4;
        if((int32_t)value>(int32_t)r->geometry->position[1] || value==r->geometry->position[1]) goto warning_nine;
        goto height_warning;
warning_nine:
        if((a->secondary_flags&0x80u) || *s->shown_message==0x9c06) goto record_warning;
        *s->events|=0x80;
        if(!message(s,&in,0x9c06)) return 0;
        goto matrix;
    }
height_warning:
    if((int16_t)r->angle_third>0x3840) { if((int16_t)r->angle_third>0x6bd0) goto record_warning; }
    else if((int16_t)r->angle_third<0x4b0) goto record_warning;
    if((int32_t)r->geometry->position[1]<=0x1800) goto warning_nine;
    goto record_warning;
action_warning:
    if(r->byte_7c&15u) goto record_warning;
    value<<=6;
    if((int32_t)value<(int32_t)r->geometry->position[1] || *s->shown_message==0xd02a) goto record_warning;
    *s->events|=0x40;
    if(!message(s,&in,0xd02a)) return 0;
    goto matrix;
record_warning:
    if(*s->current_slot==*s->target_slot && !*s->origin_enable && (a->secondary_flags&1u) &&
       *s->shown_message!=0xd00a) {
        if(!message(s,&in,0xd00a)) return 0;
        *s->events|=0x80;
    }
    if(!(r->byte_20&2u) && (a->equipment_kind&0xf0u)==0x10 && (a->flags&0x1000u) &&
       !consume(s,FA18_POSE_RECORD_SELECTOR,&in,&out)) return 0;
matrix:
    if(((a->equipment_kind&0xf0u)==0x30 || !(a->flags&0x8000u)) &&
       !consume(s,FA18_POSE_RECORD_MATRIX,&in,&out)) return 0;
    if(!*s->current_slot) goto root_flight;
    for(i=0;i<3;++i) rates[i]=(uint32_t)(int32_t)((int16_t)r->geometry->inverse[i][2]>>2);
    if(a->flags&0x100u) { a->flags&=0xfeff; goto zero_rates; }
    if((a->equipment_kind&0xf0u)!=0x30) goto root_flight;
    sum=(int64_t)(int32_t)r->geometry->position[1]+(int32_t)r->long_42;
    value=r->geometry->position[1]+r->long_42;
    if(sum<=0 || !value) { a->flags|=0x8000; value=0; }
    r->geometry->position[1]=value;
    rates[0]=r->long_3e; rates[1]=r->long_42; rates[2]=r->long_46;
    if(a->flags&0x8000u) goto record_timer;
    goto integrate;
zero_rates:
    memset(rates,0,sizeof rates); goto publish;
root_flight:
    if(!(*s->matrix_control&0x40u)) memset(rates,0,sizeof rates);
    else {
        if(!consume(s,FA18_POSE_ROOT_FLIGHT,&in,&out)) return 0;
        rates[0]=r->long_3e; rates[1]=r->long_42; rates[2]=r->long_46;
    }
integrate:
    if(!(a->flags&0x1000u)) goto action_decay;
    x=r->geometry->position[0]; z=r->geometry->position[2];
    sum=(int64_t)(int32_t)x+(int32_t)rates[0]; x+=rates[0];
    if(sum<0) z=0; /* Preserve the source's Z clear on negative X. */
    else if((int32_t)x>0x1fffffff) x=0x1fffffff;
    sum=(int64_t)(int32_t)z+(int32_t)rates[2]; z+=rates[2];
    if(sum<0) z=0; else if((int32_t)z>0x1fffffff) z=0x1fffffff;
    r->geometry->position[0]=x; r->geometry->position[2]=z;
    if(a->equipment_kind==21) r->geometry->position[1]=0xb000;
publish:
    x=r->geometry->position[0]; z=r->geometry->position[2];
    if(r->word_06!=coarse(x)) { r->word_06=coarse(x); *s->scene_redraw=0xff; }
    if(r->word_08!=coarse(z)) { r->word_08=coarse(z); *s->scene_redraw=0xff; }
    publish(r);
    x=rotate_cell((x&0x3fffffu)>>8); z=rotate_cell((z&0x3fffffu)>>8);
    lo=cell((uint16_t)x,(uint16_t)z);
    if(r->byte_0a!=lo) {
        r->byte_0a=lo;
        if(near_grid(r->word_06,*s->grid_x) && near_grid(r->word_08,*s->grid_z)) {
            *s->scene_redraw=0xff; r->byte_0b=cell((uint16_t)x>>2,(uint16_t)z>>2);
        }
    }
action_decay:
    lo=r->byte_7c&15u; hi=r->byte_7c&0xf0u;
    if(lo) {
        if(lo>6) { --lo; if(lo>6) goto store_action; lo=0; }
        ++lo;
        if(lo>6) {
            if(!(a->secondary_flags&0x20u)) --lo;
            else { lo=0; a->secondary_flags&=0xffdf; }
        }
store_action:
        r->byte_7c=lo|hi;
    }
    if((int8_t)hi>0) hi=(uint8_t)(hi-16u);
    else if((int8_t)hi<0) {
        hi&=0x7f;
        if(hi>=0x60) goto view_decay;
        hi=(uint8_t)((hi+16u)|0x80u);
    } else goto view_decay;
    /* The source takes the low nibble from the masked high nibble: zero. */
    r->byte_7c=hi; *s->bar_redraw=3;
view_decay:
    if(!*s->current_stride) {
        lo=*s->view_decay;
        if((int8_t)lo>0) --lo;
        else if((int8_t)lo<0) {
            lo&=0x7f; if(lo>=5) goto candidate;
            lo=(uint8_t)((lo+1u)|0x80u);
        } else goto candidate;
        *s->view_decay=lo;
    }
candidate:
    if(a->flags&0x400u) goto record_timer;
    for(i=0;i<3;++i) in.point[i]=r->geometry->position[i]-rates[i];
    if(!consume(s,FA18_POSE_MOTION_CANDIDATE,&in,&out)) return 0;
    status=out.status;
    if(out.clear) goto no_collision;
    if(status&32u) { r->long_56=0; r->word_5a=0; }
    if(status==32 && (a->equipment_kind&0xf0u)) {
        /* The repeated class read has no intervening writer: the original
         * quarter-rate fall-through C26058-C26068 is unreachable. */
        if(!(a->secondary_flags&0x80u)) r->geometry->position[1]-=rates[1];
        r->geometry->position[0]-=rates[0]; r->geometry->position[2]-=rates[2];
    }
    publish(r); value=r->word_6e;
    if(*s->current_slot) goto collision_class;
    if(r->byte_7c&15u) goto collision_status;
    if(status&16u) { a->secondary_flags|=0x80; goto collision_speed; }
    if(!*s->collision_enable) goto record_timer;
    if((int16_t)status>32) goto collision_speed;
    old_flags=a->flags; a->flags|=0x8000;
    if((old_flags&0x8000u) || (int16_t)value>960 || !(status&32u)) goto collision_speed;
    in.sound_arguments[0]=0x1c; in.sound_arguments[1]=0x30;
    if(!consume(s,FA18_POSE_SOUND,&in,&out)) return 0;
collision_speed:
    a->flags|=0x8000;
    if((int16_t)value<=960 || *s->collision_inhibit) goto record_timer;
    if(status&64u) {
        if(!(*s->selector_word&1u)) {
            if((int8_t)(*s->cockpit_a|*s->cockpit_b)>0) goto record_timer;
            r->byte_20|=2; choice=0x9014; *s->matrix_control|=0x100;
        } else choice=(*s->selector_word&2u)?0xd00c:0xd00b;
        if(!message(s,&in,choice)) return 0;
        *s->collision_report=1; goto record_timer;
    }
    a->flags|=0x200; ++*s->damage_count; *s->mission_failure=1; *s->failure_view=4;
    if(!*s->origin_enable) goto record_timer;
collision_class:
    if(status!=64) a->flags|=0x8000;
collision_status:
    if(a->flags&0x400u) {
        if(!timer(r,&countdown)) return 0;
        if((int16_t)countdown<=15) goto record_timer;
        *s->error_word=58;
        /* Original release hook C06C02 is RTS; continue with the timer. */
        goto record_timer;
    }
    kind=a->equipment_kind&0xf0u;
    if(kind==0x30 && !(a->flags&0x200u)) goto record_timer;
    r->byte_20|=2;
    if(status!=64) a->flags&=0xefff;
    if(!kind) goto ground_collision;
    if(a->equipment_kind==21 || (int16_t)status<64) goto mark_expiry;
    *s->collision_slot=*s->current_stride; a->secondary_flags|=0x1000; r->byte_20|=2;
    if(!put_mode(r,1)) return 0;
ground_collision:
    if(status==64) { a->flags&=0xffbf; *s->scene_redraw=12; return 1; }
choose_slot:
    choice=17;
    if((int16_t)status>=32) goto motion_slot;
    memcpy(in.velocity,rates,sizeof rates);
    if(!consume(s,FA18_POSE_GROUND_PROJECTION,&in,&out)) return 0;
    if(!consume(s,FA18_POSE_REGION_PROBE,&in,&out)) return 0;
    if(r->byte_04&2u) goto mark_expiry;
    choice=0;
motion_slot:
    in.choice=choice;
    if(!consume(s,FA18_POSE_MOTION_SLOT,&in,&out)) return 0;
mark_expiry:
    a->flags|=0x400;
    if(!put_timer(r,15)) return 0;
    a->flags|=0x200;
    if(*s->current_slot) a->flags&=0xfdff;
    goto record_timer;
no_collision:
    if((r->byte_20&2u) && (a->secondary_flags&0x80u) && !(a->flags&0x400u)) {
        a->flags&=0xefff; goto mark_expiry;
    }
    kind=a->equipment_kind&0xf0u;
    if(kind==0x30) goto record_timer;
    a->flags&=0x7fff;
    if(!kind) {
        status=0;
        if(a->flags&0x400u) goto request_flag;
        if(a->secondary_flags&0x80u) goto choose_slot;
    }
    if(a->flags&0x200u) goto mark_expiry;
request_flag:
    if(*s->request_flag) { a->secondary_flags|=2; goto record_timer; }
    old_flags=a->secondary_flags; a->secondary_flags&=0xfffd;
    if((old_flags&2u) && !(a->equipment_kind&0xf0u)) { r->byte_20|=0x20; *s->request_clear=0; }
record_timer:
    return fa18_update_native_record_motion_history(s);
}
