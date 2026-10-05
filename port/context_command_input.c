/* Source: game/context_commands.c, $C1B664-$C1B778/$C1BF8C-$C1C0C8.
 * Includes the shared $C1C214 nonzero-to-zero toggle before publication. */
#include "context_command_input.h"

int fa18_is_context_input_command(enum CommandAction action) {
    return action==COMMAND_CONTEXT_SPECIAL || action==COMMAND_CONTEXT_REFRESH ||
           action==COMMAND_CONTEXT_CALCULATION || action==COMMAND_MAP || action==COMMAND_CONTEXT_REQUEST;
}

static int copy_angle(FA18ContextCommandState *s) {
    size_t i;
    for(i=0;i<s->record_count;++i)
        if(s->records[i].command_record==s->view->flight->viewed) {
            s->angle_history=s->records[i].angle;
            s->view->flight->command_word|=2; s->view_request=0xff; return 1;
        }
    return 0;
}
static int start_record(FA18ContextCommandState *s) {
    s->view->flight->context_started=1; s->view->flight->pause=0;
    return copy_angle(s);
}
static int invoke(FA18ContextCommandState *s,enum ContextCommandChild child,
                   const FA18ContextCommandChildInput *input,
                   const FA18ContextCommandOps *ops,FA18ContextCommandChildResult *result) {
    if(child==CONTEXT_COMMAND_LOCAL_TO_WORLD || child==CONTEXT_COMMAND_SET_OBSERVER)
        return fa18_apply_context_control_child(s,child,input,result);
    return ops && ops->consume && ops->consume(ops->context,s,child,input,result);
}
static uint32_t swapped_scaled(int16_t word) {
    uint32_t value=(uint32_t)(int32_t)word;
    return ((value<<16)|(value>>16))<<6;
}
static int calculate(FA18ContextCommandState *s,const FA18ContextCommandPoses *poses,
                      const FA18ContextCommandOps *ops,uint32_t *event) {
    FA18CommandInput *c=s->view->flight->commands;
    FA18ContextCommandChildInput input={0,{0,0,0},{0,0,0},0};
    FA18ContextCommandChildResult result;
    const FA18ContextCommandPose *pose;
    int index;
    unsigned i;
    s->view->emitted_requests|=0x0400; s->track_started=0xff;
    if(!poses || !poses->values || poses->first_index<-128 || poses->first_index>127) return 0;
    index=(int8_t)c->indexed.pose_entry-poses->first_index;
    if(index<0 || (size_t)index>=poses->count) return 0;
    pose=&poses->values[index];
    if(pose->kind==FA18_CONTEXT_POSE_RECORD) {
        uint8_t kind;
        if(!pose->record || !pose->record->command_record) return 0;
        input.record=pose->record; kind=pose->record->command_record->equipment_kind;
        input.local[0]=kind==0x20?-36:0;
        input.local[1]=kind==0x20?47:20;
        input.local[2]=kind==0x20?-48:-106;
        if(!invoke(s,CONTEXT_COMMAND_LOCAL_TO_WORLD,&input,ops,&result)) return 0;
        for(i=0;i<3;++i) input.position[i]=result.position[i];
    } else if(pose->kind==FA18_CONTEXT_POSE_PRESET) {
        input.position[0]=(int32_t)(swapped_scaled(pose->preset[0])+
            ((uint32_t)(int32_t)pose->grid_pair[0]<<8)+((uint32_t)(int32_t)pose->preset[4]<<8));
        input.position[2]=(int32_t)(swapped_scaled(pose->preset[1])+
            ((uint32_t)(int32_t)pose->grid_pair[1]<<8)+((uint32_t)(int32_t)pose->preset[5]<<8));
        input.position[1]=(int32_t)((uint32_t)(int32_t)pose->preset[3]<<8);
    } else return 0;
    input.event=(uint32_t)input.position[0];
    if(!invoke(s,CONTEXT_COMMAND_SET_OBSERVER,&input,ops,&result)) return 0;
    c->indexed.origin_gate_b=0;
    if(!c->origin_mode) { if(!start_record(s)) return 0; }
    else s->track_started=0;
    *event=result.event; return 1;
}
static int release_voices(FA18ContextCommandState *s,const CommandRequest *r,
                           enum ContextCommandChild child,const FA18ContextCommandOps *ops,
                           uint32_t *event) {
    FA18ContextCommandChildInput input={0,{0,0,0},{0,0,0},r->raw_event};
    FA18ContextCommandChildResult result;
    if(!invoke(s,child,&input,ops,&result)) return 0;
    *event=(result.event&0xffff0000u)|(r->raw_event&0xffffu); return 1;
}

int fa18_apply_context_input_command(FA18ContextCommandState *s,const CommandRequest *r,
                                     const FA18ContextCommandPoses *poses,
                                     const FA18ContextCommandOps *ops,uint32_t *event) {
    FA18FlightCommandState *f;
    FA18CommandInput *c;
    uint32_t published;
    unsigned i;
    if(!s || !r || !event || !s->view || !s->view->flight || !s->view->flight->commands ||
       !s->view->flight->viewed || !s->records || !s->record_count || !s->key_taken ||
       !fa18_is_context_input_command(r->action)) return 0;
    f=s->view->flight; c=f->commands; published=r->raw_event;
    switch(r->action) {
    case COMMAND_CONTEXT_SPECIAL: *s->key_taken=2; /* fall through */
    case COMMAND_CONTEXT_REFRESH:
        if(r->modifier) return calculate(s,poses,ops,event);
        s->view->emitted_requests|=0x0200;
        if(!r->origin_mode) s->view->detail_index=4;
        c->indexed.origin_gate_b=1;
        if(!start_record(s)) return 0;
        break;
    case COMMAND_CONTEXT_CALCULATION: return calculate(s,poses,ops,event);
    case COMMAND_MAP:
        if(c->indexed.origin_detail) break;
        s->view->emitted_requests|=0x0100;
        if(f->pause) {
            s->map_middle_cache=s->view->origin_middle; c->indexed.origin_gate_b=1;
            if(!start_record(s)) return 0;
            break;
        }
        if(!release_voices(s,r,CONTEXT_COMMAND_MAP_VOICES,ops,&published)) return 0;
        s->origin_first=0x10c00000; s->view->origin_middle=s->map_middle_cache;
        s->origin_third=0x11400000; s->negated[0]=s->negated[2]=0;
        s->negated[1]=0u-s->map_middle_cache;
        s->smoothed_delta=s->auxiliary_delta[0]=s->auxiliary_delta[1]=0;
        s->pan=0x1c20; s->rotate=0;
        f->context_started=0; c->indexed.origin_gate_b=1; f->pause=1;
        if(!c->origin_mode) { if(!copy_angle(s)) return 0; }
        else s->view->line_last_row=0xb3;
        break;
    case COMMAND_CONTEXT_REQUEST:
        if((int8_t)c->indexed.origin_detail<0) break;
        if(c->indexed.origin_gate_a) { s->view->fire_state=0xfe; c->indexed.origin_gate_a=0; break; }
        if(!release_voices(s,r,CONTEXT_COMMAND_REQUEST_VOICES,ops,&published)) return 0;
        if(!c->indexed.recorder_mode && s->recording_write && s->recorder_on) {
            if(s->recording_remaining<4) return 0;
            for(i=0;i<4;++i) s->recording_write[i]=0xff;
        }
        c->indexed.origin_gate_a=0xff; break;
    default: return 0;
    }
    *event=published; return 1;
}
