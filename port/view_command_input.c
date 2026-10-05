/* Source: game/view_commands.c, original $C1B77C-$C1BB76 actions,
 * $C08324 maximum zoom and $C082B8 cockpit redraw. */
#include "view_command_input.h"

int fa18_is_view_input_command(enum CommandAction action) {
    switch(action) {
    case COMMAND_CONTEXT_VIEW_DECREMENT: case COMMAND_CONTEXT_VIEW_INCREMENT:
    case COMMAND_CONTEXT_VIEW_ALTERNATE: case COMMAND_VIEW_ZERO: case COMMAND_VIEW_ONE:
    case COMMAND_VIEW_INCREMENT: case COMMAND_VIEW_DECREMENT: case COMMAND_VIEW_TWELVE:
    case COMMAND_VIEW_THIRTEEN: case COMMAND_VIEW_THREE: case COMMAND_VIEW_NINE:
    case COMMAND_VIEW_TOGGLE: case COMMAND_VIEW_EIGHT: case COMMAND_FIRE_REQUEST:
    case COMMAND_ZOOM_OUT: case COMMAND_ZOOM_IN: return 1;
    default: return 0;
    }
}

static void detail(FA18ViewCommandState *s,uint8_t value) {
    s->detail_index=value; s->update_mask=0xff;
    if(!s->fire_state) s->fire_state=0xff;
}

static void origin_range(FA18ViewCommandState *s,int increase) {
    FA18CommandInput *c=s->flight->commands;
    uint8_t old=c->origin_mode,value;
    uint32_t middle;
    if(!old || c->indexed.origin_detail) return;
    if(!increase) s->emitted_requests|=0x0800;
    if(s->flight->pause) {
        middle=increase?s->origin_middle+0x02000000u:s->origin_middle-0x02000000u;
        if(increase?(int32_t)middle>0x08000000:(int32_t)middle<0x01000000)
            middle=increase?0x08000000u:0x01000000u;
        s->origin_middle=middle; s->update_mask=0xff; return;
    }
    if(increase) {
        value=(uint8_t)(old+1);
        if(!value) value=1;
        else if((int8_t)value>=4) value=4;
    } else {
        value=(uint8_t)(old-1);
        /* Signed SUBQ/ BLE includes overflow: $80 - 1 takes this path. */
        if((int8_t)old<=1) value=0xff;
    }
    c->origin_mode=value; s->update_mask=0xff;
    if(!s->fire_state) s->fire_state=0xff;
}

static void finish_mode(FA18ViewCommandState *s,const FA18ViewSpanOffsets *spans) {
    int mode,span;
    uint16_t row;
    fa18_set_native_zoom_maximum(s);
    s->target_mark=0xffff; s->update_mask=0xff;
    s->mode_companion=0; s->refresh_request=0xff;
    if((s->flight->viewed->equipment_kind&0xf0)==0x30) return;
    mode=(int8_t)s->mode;
    if(mode>=3 && (mode<=9 || mode>=12)) {
        row=mode>=5 && mode<=7?0xb3:0xa7;
        if(row==s->line_last_row) { s->span_origin=0x32; s->span_origin_y=0x320; return; }
    }
    span=spans->values[mode+128];
    s->span_origin=(uint16_t)span;
    s->span_origin_y=(uint16_t)((uint16_t)span<<4);
    fa18_request_native_cockpit_redraw(s);
    if(mode>=3 && (mode<=9 || mode>=12)) row=mode>4 && mode<8?0xb3:0xa7;
    else row=0x90;
    s->line_last_row=row;
}

static void zero_mode(FA18ViewCommandState *s,const FA18ViewSpanOffsets *spans) {
    s->mode_auxiliary=0; s->mode=0; s->redraw_first=3;
    finish_mode(s,spans);
}

int fa18_apply_view_input_command(FA18ViewCommandState *s,const CommandRequest *r,
                                  const FA18ViewSpanOffsets *spans,uint32_t *event) {
    FA18CommandInput *c;
    uint8_t old,value;
    uint16_t scale;
    if(!s || !s->flight || !s->flight->commands || !s->flight->viewed || !r ||
       !spans || !event || !fa18_is_view_input_command(r->action)) return 0;
    c=s->flight->commands;
    switch(r->action) {
    case COMMAND_CONTEXT_VIEW_DECREMENT:
        if(c->origin_mode) { origin_range(s,0); break; }
        s->emitted_requests|=0x0800; s->refresh_request=0xff; break;
    case COMMAND_CONTEXT_VIEW_ALTERNATE:
        if(c->origin_mode) { s->emitted_requests|=0x0010; detail(s,8); }
        else { s->emitted_requests|=0x0800; s->refresh_request=0xff; }
        break;
    case COMMAND_CONTEXT_VIEW_INCREMENT:
        s->emitted_requests|=0x0080; origin_range(s,1); break;
    case COMMAND_VIEW_THREE: s->emitted_requests|=0x8000; detail(s,3); break;
    case COMMAND_VIEW_NINE: s->emitted_requests|=0x0020; detail(s,9); break;
    case COMMAND_VIEW_EIGHT: s->emitted_requests|=0x0010; detail(s,8); break;
    case COMMAND_VIEW_TOGGLE:
        s->emitted_requests|=0x2000;
        if(c->origin_mode) { detail(s,1); break; }
        zero_mode(s,spans); break;
    case COMMAND_VIEW_ZERO:
        s->emitted_requests|=0x1000;
        if(r->origin_mode) { detail(s,0); break; }
        zero_mode(s,spans); break;
    case COMMAND_VIEW_ONE:
        s->emitted_requests|=1;
        if(r->origin_mode) { detail(s,4); break; }
        s->mode_auxiliary=0; s->mode=6; s->redraw_first=3; finish_mode(s,spans); break;
    case COMMAND_VIEW_INCREMENT:
        s->emitted_requests|=0x4000;
        if(r->origin_mode) { detail(s,2); break; }
        s->mode_auxiliary=0; s->mode=(uint8_t)(s->mode+1);
        if((int8_t)s->mode<=11) finish_mode(s,spans); else zero_mode(s,spans);
        break;
    case COMMAND_VIEW_DECREMENT:
        s->emitted_requests|=4;
        if(r->origin_mode) { detail(s,6); break; }
        s->mode_auxiliary=0; old=s->mode; value=(uint8_t)(old-1); s->mode=value;
        if((int8_t)old<1) { s->mode=11; s->redraw_first=3; finish_mode(s,spans); }
        else if(!value || (int8_t)value>=11) zero_mode(s,spans);
        else finish_mode(s,spans);
        break;
    case COMMAND_VIEW_TWELVE: case COMMAND_VIEW_THIRTEEN:
        s->emitted_requests|=r->action==COMMAND_VIEW_TWELVE?8:2;
        if(r->origin_mode) { detail(s,r->action==COMMAND_VIEW_TWELVE?7:5); break; }
        s->mode=r->action==COMMAND_VIEW_TWELVE?12:13;
        s->mode_auxiliary=0; s->update_mask=0xff; s->redraw_first=3;
        finish_mode(s,spans); break;
    case COMMAND_FIRE_REQUEST:
        s->emitted_requests|=0x40; s->refresh_request=5; break;
    case COMMAND_ZOOM_OUT:
        if(r->origin_mode && c->indexed.origin_gate_b) { origin_range(s,0); break; }
        if((int16_t)s->zoom_scale>0x20) s->zoom_scale=(uint16_t)((int16_t)s->zoom_scale>>1);
        goto zoom_refresh;
    case COMMAND_ZOOM_IN:
        if(r->origin_mode && c->indexed.origin_gate_b) {
            s->emitted_requests|=0x80; origin_range(s,1); break;
        }
        if((int16_t)s->zoom_scale<0x80) s->zoom_scale=(uint16_t)(s->zoom_scale<<1);
    zoom_refresh:
        s->display_update=3; s->refresh_request=0xff;
        if(!(s->zoom_flags&0x7f)) s->update_mask=0xff;
        scale=s->zoom_scale;
        s->zoom_flags=scale==0x80?s->zoom_flags|0x80:s->zoom_flags&0x7f;
        break;
    default: return 0;
    }
    *event=r->raw_event; return 1;
}
