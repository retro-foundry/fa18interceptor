/* Actual reusable children: $C08324 maximum zoom and $C082B8 redraw. */
#include "view_command_input.h"

int fa18_set_native_zoom_maximum(FA18ViewCommandState *s) {
    if(!s) return 0;
    s->zoom_flags|=0x80; s->zoom_scale=0x80; s->display_update=3;
    return 1;
}

int fa18_request_native_cockpit_redraw(FA18ViewCommandState *s) {
    FA18FlightCommandState *f;
    if(!s || !s->flight) return 0;
    f=s->flight;
    s->gauge_refresh=s->redraw_first=s->grid_z_redraws=s->grid_x_redraws=3;
    s->redraw_bar_a=f->redraw_b=f->redraw_c=s->redraw_bar_aux=3;
    f->scale_redraws=s->display_update=f->weapon_mode_redraws=f->weapon_redraws=3;
    f->redraw_d=f->info_redraws=3;
    if(!s->redraw_keep_state) { s->redraw_state_word=0; s->redraw_state_long=0; }
    return 1;
}
