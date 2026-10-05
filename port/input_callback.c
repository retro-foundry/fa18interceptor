#include "input_callback.h"

static int16_t signed_word(uint16_t bits) {
    return bits<0x8000u?(int16_t)bits:(int16_t)((int)bits-0x10000);
}
static int16_t counter_delta(uint16_t value,uint16_t previous) {
    int16_t delta=signed_word((uint16_t)(value-previous));
    if(delta < -128) delta=signed_word((uint16_t)((uint16_t)delta+256u));
    else if(delta>127) delta=signed_word((uint16_t)((uint16_t)delta-256u));
    return delta;
}
static int16_t clamp_axis(int16_t value,int16_t minimum,int16_t maximum) {
    int16_t selected=maximum>=value?value:maximum;
    return minimum>selected?minimum:selected;
}

static const uint16_t *select_display_palette(void *context,int mode) {
    FA18NativeInputDisplay *s=context;
    int64_t index=(int64_t)mode-s->first_mode;
    if(index<0 || (uint64_t)index>=s->mode_count) return NULL;
    return s->mode_palettes[index];
}
static int load_display_palette(void *context,FA18ViewportPalettePhase phase,const uint16_t *words) {
    FA18NativeInputDisplay *s=context;
    return s->load_palette(s->context,phase,words);
}
static int publish_display_pair(void *context,int index) {
    FA18NativeInputDisplay *s=context;
    int64_t offset=(int64_t)index-s->first_pair;
    if(offset<0 || (uint64_t)offset>=s->pair_count) return 0;
    s->saved_pair.view=s->pairs[offset].view;
    s->saved_pair.display_list=s->pairs[offset].display_list;
    return 1;
}
int fa18_prepare_native_input_display(FA18NativeInputDisplay *s,FA18ViewportTransitionOps *ops) {
    if(!s || !ops || !s->pairs || !s->pair_count || !s->mode_palettes ||
       !s->mode_count || !s->stable_palette || !s->load_palette) return 0;
    *ops=(FA18ViewportTransitionOps){select_display_palette,load_display_palette,publish_display_pair,
          &s->draw_page,s->stable_palette,s};
    return 1;
}

int fa18_initialize_native_input_callback(FA18NativeInputCallbackState *s,FA18CommandQueue *q,
                                           FA18CommandAudio *a,FA18ViewportModeState *v) {
    if(!s || !q || !q->commands || !a || !v) return 0;
    if(!fa18_bind_command_queue_word(q,0x13,&s->ticks) ||
       !fa18_bind_command_queue_word(q,0x15,&s->mouse_x)) return 0;
    s->commands=q->commands; s->audio=a; s->viewport=v;
    return 1;
}

int fa18_advance_native_input_callback(FA18NativeInputCallbackState *s,uint16_t pair,
                                        const FA18ViewportTransitionOps *ops) {
    uint16_t x=pair&255u,y=pair>>8;
    int16_t dx,dy;
    if(!s || !s->commands || !s->audio || !s->viewport || !ops) return 0;
    dx=counter_delta(x,s->counter_x); dy=counter_delta(y,s->counter_y);
    if(!s->commands->indexed.player_ready && dy<0)
        dy=(int16_t)(-((-(int)dy+1)/2)); /* signed arithmetic shift, floor */
    s->mouse_x=signed_word((uint16_t)((uint16_t)s->mouse_x+(uint16_t)dx));
    s->commands->indexed.throttle=signed_word((uint16_t)((uint16_t)s->commands->indexed.throttle-(uint16_t)dy));
    s->mouse_x=clamp_axis(s->mouse_x,s->min_x,s->max_x);
    s->commands->indexed.throttle=clamp_axis(s->commands->indexed.throttle,s->min_y,s->max_y);
    s->counter_x=x; s->counter_y=y;
    s->ticks=signed_word((uint16_t)((uint16_t)s->ticks+1u));
    if(!fa18_advance_native_viewport_transition(s->viewport,ops)) return 0;
    return fa18_fade_native_master_volume(s->audio);
}
