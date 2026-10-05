#include "view_command_input.h"
#include <assert.h>
#include <string.h>

int main(void) {
    FA18CommandInput commands;
    FA18FlightCommandState flight;
    FA18FlightCommandRecord viewed;
    FA18ViewCommandState s;
    FA18ViewSpanOffsets spans;
    CommandRequest request;
    uint32_t event;
    memset(&commands,0,sizeof commands);
    memset(&flight,0,sizeof flight);
    memset(&viewed,0,sizeof viewed);
    memset(&s,0,sizeof s);
    memset(&spans,0,sizeof spans);
    flight.commands=&commands; flight.viewed=&viewed; s.flight=&flight;
    commands.event_counter=commands.indexed.mode_gate=commands.indexed.mode=1;
    commands.pending_b=0x1000;
    assert(fa18_select_pending_command(&commands,&request));
    assert(request.action==COMMAND_VIEW_ZERO && !commands.pending_b);
    s.redraw_state_word=123; s.redraw_state_long=456;
    spans.values[128]=7;
    assert(fa18_apply_view_input_command(&s,&request,&spans,&event));
    assert(!s.mode && s.zoom_scale==0x80 && s.zoom_flags==0x80);
    assert(s.target_mark==0xffff && s.span_origin==7 && s.span_origin_y==112);
    assert(s.line_last_row==0x90 && s.update_mask==0xff && s.refresh_request==0xff);
    assert(s.gauge_refresh==3 && flight.weapon_mode_redraws==3 &&
           flight.redraw_b==3 && flight.info_redraws==3 && flight.redraw_d==3);
    assert(!s.redraw_state_word && !s.redraw_state_long && s.emitted_requests==0x1000);
    s.redraw_keep_state=1; s.redraw_state_word=123; s.redraw_state_long=456;
    assert(fa18_request_native_cockpit_redraw(&s));
    assert(s.redraw_state_word==123 && s.redraw_state_long==456);

    /* Signed view indexing and a complete event survive actual child calls. */
    request=(CommandRequest){COMMAND_VIEW_INCREMENT,0x87654321,0,0,0,0};
    s.mode=0xfe; spans.values[127]=-8;
    assert(fa18_apply_view_input_command(&s,&request,&spans,&event));
    assert(s.mode==0xff && s.span_origin==0xfff8 && s.span_origin_y==0xff80 && event==0x87654321);
    request.action=COMMAND_VIEW_ONE;
    s.line_last_row=0xb3; s.gauge_refresh=0;
    assert(fa18_apply_view_input_command(&s,&request,&spans,&event));
    assert(s.mode==6 && s.span_origin==0x32 && s.span_origin_y==0x320 && !s.gauge_refresh);
    viewed.equipment_kind=0x31; s.target_mark=0; s.span_origin=99;
    request.action=COMMAND_VIEW_ZERO;
    assert(fa18_apply_view_input_command(&s,&request,&spans,&event));
    assert(s.target_mark==0xffff && s.span_origin==99 && s.redraw_first==3);

    /* Origin and zoom selection share gates with native aircraft/indexed state. */
    commands.origin_mode=1;
    commands.indexed.origin_gate_b=1;
    flight.pause=1; s.origin_middle=0x07000000;
    assert(fa18_select_keyboard_command(&commands,0x1a,&request));
    assert(request.action==COMMAND_ZOOM_IN);
    assert(fa18_apply_view_input_command(&s,&request,&spans,&event));
    assert(s.origin_middle==0x08000000 && commands.origin_mode==1 && (s.emitted_requests&0x80));
    flight.pause=0; commands.origin_mode=0xff;
    assert(fa18_apply_view_input_command(&s,&request,&spans,&event));
    assert(commands.origin_mode==1);
    request.action=COMMAND_ZOOM_OUT; commands.origin_mode=0x80;
    assert(fa18_apply_view_input_command(&s,&request,&spans,&event));
    assert(commands.origin_mode==0xff);
    request.origin_mode=0; s.zoom_scale=0x40; s.zoom_flags=0;
    assert(fa18_apply_view_input_command(&s,&request,&spans,&event));
    assert(s.zoom_scale==0x20 && !s.zoom_flags && s.display_update==3);
    request.action=COMMAND_ZOOM_IN; s.zoom_scale=0x8000;
    assert(fa18_apply_view_input_command(&s,&request,&spans,&event));
    assert(!s.zoom_scale);
    request.action=COMMAND_GEAR; event=0xfeedface;
    assert(!fa18_apply_view_input_command(&s,&request,&spans,&event));
    assert(event==0xfeedface && !fa18_is_view_input_command(request.action));
    assert(!fa18_apply_view_input_command(NULL,&request,&spans,&event));
    assert(!fa18_set_native_zoom_maximum(NULL));
    assert(!fa18_request_native_cockpit_redraw(NULL));
    return 0;
}
