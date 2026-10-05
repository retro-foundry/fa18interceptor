#include "native_postflight.h"
#include <limits.h>

static int16_t post_word(uint16_t n) { return n<=INT16_MAX?(int16_t)n:(int16_t)((int32_t)n-65536); }
static int32_t post_long(uint32_t n) {
    return n<=INT32_MAX?(int32_t)n:(int32_t)((int64_t)n-INT64_C(0x100000000));
}
static int post_valid(const FA18NativePostflight *s) {
    return s && s->records && s->selection && s->selection->records==s->records &&
        s->parameters && s->view_work && s->phase_fields && s->current_slot && s->target_record &&
        s->dispatch_gate && s->command_word && s->view_heading && s->space_latch && s->report_latch &&
        s->status && s->blocked && s->mode && s->player_phase && s->player_flags_f &&
        s->sequence_phase && s->sequence_step && s->context_gate && s->post_input_event &&
        s->saved_view && s->view_side && s->saved_context && s->context_select &&
        s->context_smooth && s->context_started && s->context_clear && s->refresh &&
        s->limit && s->admitted && s->aux && s->ready_mode;
}
static int post_put_word(FA18NativeSceneRecord *r,size_t at,uint16_t n) {
    uint8_t b[2]={(uint8_t)(n>>8),(uint8_t)n}; return fa18_write_native_scene_record(r,at,b,2);
}
static int post_get_word(FA18NativeSceneRecord *r,size_t at,uint16_t *n) {
    uint8_t b[2]; if(!fa18_read_native_scene_record(r,at,b,2)) return 0;
    *n=(uint16_t)(((unsigned)b[0]<<8)|b[1]); return 1;
}
static int phase_three(FA18NativePostflight *s,uint16_t event) {
    if(!port_fill_field_words(s->phase_fields,1,event)) return 0;
    *s->sequence_phase=3; *s->sequence_step=4; *s->context_gate=1; return 1;
}
static int post_outcome(FA18NativePostflight *s,uint8_t phase,uint16_t event) {
    *s->player_phase=phase; return phase_three(s,event);
}
int fa18_native_postflight_ready(FA18NativePostflight *s,int *ready) {
    FA18NativeSceneRecord *r;
    if(!post_valid(s) || !ready) return 0;
    r=s->records->records;
    *ready=(r->byte_21&1u) && (r->aircraft->secondary_flags&0x80u) &&
        (*s->ready_mode==3?(r->byte_04&0xc0u):(r->byte_04&4u)) && !r->word_6e;
    return 1;
}
static int existing_phase(FA18NativePostflight *s) {
    uint8_t phase=*s->player_phase; int ready;
    if(phase!=0xff && (uint8_t)(phase-1u)) return 1;
    return fa18_native_postflight_ready(s,&ready) && (!ready || post_outcome(s,0xfc,0));
}
static uint8_t post_countdown(FA18NativePostflight *s,uint8_t initial) {
    if(!*s->post_input_event) *s->post_input_event=initial;
    --*s->post_input_event; return *s->post_input_event;
}
static void save_view(FA18NativePostflight *s,FA18NativeSceneRecord *r,uint8_t refresh) {
    if((int8_t)*s->post_input_event>1 || (*s->saved_view&0x80u)) return;
    *s->saved_view=(uint8_t)(*s->view_side|0x80u); *s->saved_context=*s->context_select;
    if(!(r->aircraft->flags&0x40u)) return;
    *s->view_side=7; *s->context_smooth=1; *s->context_started=1; *s->context_clear=0;
    if(*s->context_select) return;
    *s->view_heading=r->geometry->angle; *s->command_word|=2; *s->refresh=refresh;
}
static int record_mode(FA18NativePostflight *s,int seven) {
    FA18NativeSceneRecord *r=s->records->records+4;
    uint16_t event;
    if(*s->player_phase) return existing_phase(s);
    *s->current_slot=4;
    event=(uint16_t)(r->word_06|r->word_0c);
    if(!event) return 1;
    if(r->aircraft->flags&0x40u) {
        if(seven) { if(r->aircraft->flags&0x80u) return 1; }
        else if(!(r->aircraft->secondary_flags&0x80u) || post_word(r->word_6c)>0x320 ||
            (s->records->aircraft[8].flags&0x40u) || (s->records->aircraft[10].flags&0x40u)) return 1;
    }
    if(post_countdown(s,seven?2:5)) { save_view(s,r,seven?3:0xff); return 1; }
    if(seven) {
        if(r->byte_20&2u) return *s->sequence_phase==3 || post_outcome(s,0xff,0);
    } else if(!(r->aircraft->flags&0x40u)) {
        return *s->sequence_phase==3 || post_outcome(s,(r->byte_20&0x80u)?0xfd:0xfe,0);
    }
    if(s->publication) {
        FA18NativeContextPublication *p=s->publication; uint32_t published;
        if(p->records!=s->records || !p->context || !p->context->view ||
           !p->context->view->flight || p->context->view->flight->commands!=s->records->input ||
           s->context_select!=&s->records->input->origin_mode ||
           s->sequence_phase!=&p->context->view->flight->sequence_phase ||
           s->view_side!=&p->context->view->detail_index ||
           s->refresh!=&p->context->view_request || s->view_heading!=&p->context->angle_history ||
           p->selection_marker!=s->selection->selection_marker ||
           !fa18_publish_native_context_record(p,event,(int16_t)*s->current_slot,
               &s->view_work->carried_axis,&published)) return 0;
    } else if(!s->ops || !s->ops->prepare ||
              !s->ops->prepare(s->ops->context,s,*s->current_slot,event,seven)) return 0;
    return *s->sequence_phase==3 || post_outcome(s,seven?0xfe:0xff,seven?7:4);
}
int fa18_restore_native_postflight_view(FA18NativePostflight *s,unsigned slot) {
    FA18NativeSceneRecord *r; uint8_t index; int16_t relative,words[5]; unsigned i;
    if(!post_valid(s) || slot>=16) return 0;
    r=s->records->records+slot;
    if(!fa18_read_native_scene_record(r,0x3a,&index,1) ||
       !port_field_window_s16(s->parameters,(int32_t)index*2,&relative)) return 0;
    for(i=0;i<5;++i)
        if(!port_field_window_s16(s->parameters,(int32_t)relative+2*(int32_t)i,words+i)) return 0;
    s->view_work->carried_axis=(uint32_t)(int32_t)words[2];
    for(i=0;i<4;++i) if(!post_put_word(r,0x2c+2*i,(uint16_t)words[i])) return 0;
    { uint32_t n=(uint32_t)(int32_t)words[4];
      uint8_t b[4]={(uint8_t)(n>>24),(uint8_t)(n>>16),(uint8_t)(n>>8),(uint8_t)n};
      return fa18_write_native_scene_record(r,0x34,b,4); }
}
/* SUB.L/BGE observes the unwrapped signed subtraction; NEG.L wraps. */
static uint32_t post_difference(uint32_t a,uint32_t b) {
    uint32_t d=a-b;
    return (int64_t)post_long(a)-post_long(b)<0?0u-d:d;
}
static int mode_five(FA18NativePostflight *s) {
    unsigned first=4,second=6,i; uint32_t limit,d;
    FA18NativeSceneRecord *r;
    if(*s->player_phase) return existing_phase(s);
    if(!(s->records->aircraft[first].flags&0x40u)) {
        if(!(s->records->aircraft[second].flags&0x40u)) goto admit;
        if(!(s->records->aircraft[second].flags&8u)) goto gate;
        first=6; second=4;
    } else if(!(s->records->aircraft[first].flags&8u)) goto gate;
    r=s->records->records+first;
    if(post_word(r->word_06)>=0x70) return *s->sequence_phase==3 || post_outcome(s,0xfe,0);
    s->view_work->carried_axis=s->records->geometry[0].position[1];
    limit=(int8_t)*s->limit>=3?0x18000u:(int8_t)*s->limit>=2?0x24000u:0x30000u;
    for(i=0;i<3;++i) {
        d=post_difference(s->records->geometry[0].position[i],r->geometry->position[i]);
        if(i==1) s->view_work->carried_axis=d;
        if(post_long(d)>(int32_t)limit) goto reset;
    }
    if(post_word(*s->dispatch_gate)>=0) { --*s->dispatch_gate; goto gate; }
    if(!fa18_restore_native_postflight_view(s,first) || !fa18_restore_native_postflight_view(s,second)) return 0;
reset:
    if(post_word(*s->dispatch_gate)>=0) *s->dispatch_gate=200;
gate:
    if(post_word(*s->dispatch_gate)>=0) return 1;
admit:
    if((int8_t)*s->admitted>(int8_t)*s->aux || post_countdown(s,2) || *s->sequence_phase==3) return 1;
    return post_outcome(s,0xff,0);
}
static int mode_six(FA18NativePostflight *s) {
    uint16_t offset,timer; unsigned slot; uint32_t x,z;
    FA18NativeSceneRecord *r;
    if(*s->player_phase) return existing_phase(s);
    offset=*s->target_record; if(!offset) return 1;
    if(offset%512u || offset/512u>=16) return 0;
    slot=offset/512u; r=s->records->records+slot;
    if(!(r->aircraft->flags&0x8000u)) return 1;
    if(!post_get_word(r,0x4c,&timer)) return 0;
    if(post_word(timer)>0) return 1;
    x=post_difference(s->records->geometry[11].position[0],r->geometry->position[0]);
    z=post_difference(s->records->geometry[11].position[2],r->geometry->position[2]);
    return *s->sequence_phase==3 || post_outcome(s,
        post_long(x)>0x10000 || post_long(z)>0x10000?0xfe:0xff,0);
}
int fa18_schedule_native_postflight(FA18NativePostflight *s,FA18NativePostflightMode mode,uint16_t event) {
    FA18NativeSceneRecord *root;
    if(!post_valid(s)) return 0;
    root=s->records->records;
    if(mode==FA18_POSTFLIGHT_FINISH) {
        *s->space_latch=0; *s->report_latch=0; *s->selection->pair_override=0;
        event=*s->selection->selected_record;
        if(!fa18_release_lost_native_selection(s->selection)) return 0;
        if(!(*s->status&0x40u) || *s->blocked || (int8_t)*s->mode<=2) return 1;
        event=(uint16_t)((event&0xff00u)|*s->mode);
        switch(*s->mode) {
        case 3: mode=FA18_POSTFLIGHT_THREE; break; case 4: mode=FA18_POSTFLIGHT_FOUR; break;
        case 5: mode=FA18_POSTFLIGHT_FIVE; break; case 6: mode=FA18_POSTFLIGHT_SIX; break;
        case 7: mode=FA18_POSTFLIGHT_SEVEN; break; case 9: mode=FA18_POSTFLIGHT_NINE; break;
        case 125: mode=FA18_POSTFLIGHT_125; break; default: mode=FA18_POSTFLIGHT_OTHER; break;
        }
    }
    switch(mode) {
    case FA18_POSTFLIGHT_THREE:
        if(*s->player_phase) return existing_phase(s);
        return (int8_t)*s->player_flags_f>=0 || *s->sequence_phase==3 || post_outcome(s,0xff,0);
    case FA18_POSTFLIGHT_FOUR: return record_mode(s,0);
    case FA18_POSTFLIGHT_FIVE: return mode_five(s);
    case FA18_POSTFLIGHT_SIX: return mode_six(s);
    case FA18_POSTFLIGHT_SEVEN: return record_mode(s,1);
    case FA18_POSTFLIGHT_NINE:
        return *s->player_phase || !(root->aircraft->flags&0x40u) ||
            (root->aircraft->secondary_flags&0xc080u)!=0xc080u || root->word_6e ||
            *s->sequence_phase==3 || post_outcome(s,0xff,0);
    case FA18_POSTFLIGHT_125:
        return *s->player_phase || (s->records->aircraft[4].flags&0x40u) ||
            *s->sequence_phase==3 || post_outcome(s,0xfe,0);
    case FA18_POSTFLIGHT_OTHER:
        if(*s->player_phase) return existing_phase(s);
        event=(uint16_t)((event&0xff00u)|*s->admitted);
        if((int8_t)*s->admitted>(int8_t)*s->aux || post_countdown(s,2) || *s->sequence_phase==3) return 1;
        return post_outcome(s,0xff,event);
    default: return 0;
    }
}
