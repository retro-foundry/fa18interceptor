#include "viewport_mode.h"
#include "viewport_transition.h"
#include <string.h>

typedef struct {
    const FA18ViewportModeBindings *bindings;
    const FA18Hunks *exe;
    FA18CopperMutableInstructionStream *streams;
    size_t stream_count;
    FA18ViewportModeStep *step;
    uint16_t selected_palette[16];
} ViewportWrapper;

static const uint16_t *select_palette(void *context,int mode) {
    ViewportWrapper *w=context;
    if(mode<0 || mode>=16 || fa18_load_viewport_mode_palette(w->exe,(uint8_t)mode,w->selected_palette)) return NULL;
    return w->selected_palette;
}
static int load_palette(void *context,FA18ViewportPalettePhase phase,const uint16_t *words) {
    ViewportWrapper *w=context;
    uint16_t palette[32]={0};
    uint32_t updated=0;
    (void)phase;
    memcpy(palette,words,16*sizeof *words);
    if(fa18_update_copper_palette_moves(w->streams,w->stream_count,palette,0xffffu,&updated)) return 0;
    w->step->palette_loaded=1;
    ++w->step->palette_load_count;
    w->step->copper_updated_mask=updated;
    return 1;
}
static int publish_pair(void *context,int index) {
    ViewportWrapper *w=context;
    if(fa18_prepare_outer_page_publication((uint16_t)index,w->bindings->left_pointer_table,
          w->bindings->right_pointer_table,w->bindings->pointer_table_entries,&w->step->publication)) return 0;
    w->step->pointer_pair_published=1;
    ++w->step->pointer_pair_publish_count;
    return 1;
}
int fa18_advance_viewport_mode(FA18ViewportModeState *state,
                               const FA18ViewportModeBindings *bindings,
                               const FA18Hunks *exe,
                               FA18CopperMutableInstructionStream *streams,
                               size_t stream_count,
                               FA18ViewportModeStep *step) {
    ViewportWrapper wrapper;
    FA18ViewportTransitionOps ops;
    if(!state || !bindings || !exe || !streams || !stream_count || !step ||
       !bindings->mode_palette_buffer || !bindings->left_pointer_table ||
       !bindings->right_pointer_table || bindings->pointer_table_entries<2 ||
       bindings->outer_selected_index>1) return -1;
    memset(step,0,sizeof *step);
    wrapper=(ViewportWrapper){bindings,exe,streams,stream_count,step,{0}};
    ops=(FA18ViewportTransitionOps){select_palette,load_palette,publish_pair,
          &bindings->outer_selected_index,bindings->mode_palette_buffer->words,&wrapper};
    if(!fa18_advance_native_viewport_transition(state,&ops)) return -1;
    step->mode_words_copied=(uint8_t)(step->pointer_pair_published && state->current==state->target);
    return 0;
}
