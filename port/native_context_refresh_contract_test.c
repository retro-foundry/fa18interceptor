#include "native_context_refresh.h"

#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput commands; FA18FlightCommandState flight; FA18ViewCommandState view;
    FA18NativeSceneRecords records; FA18NativeContextRefresh refresh; FA18NativeContextRefreshOps ops;
    uint8_t source[16*512],work[16*32],context_select,prepared,alternate,checks,view_mode,fixed,frame_gate;
    int32_t bias,origin[3]; uint16_t timer,error,key_a,key_b,selector,colour;
    uint32_t style; FA18NativeContextRefreshChild log[12]; unsigned calls;
    int consume_later,fail_sort;
} Fixture;
static int consume(void *context,FA18NativeContextRefresh *state,FA18NativeContextRefreshChild child) {
    Fixture *f=context;
    assert(state==&f->refresh && f->calls<12); f->log[f->calls++]=child;
    if(child==FA18_CONTEXT_REFRESH_TEMPLATES && f->consume_later)
        f->view.update_mask&=(uint8_t)~0x0a;
    return !(child==FA18_CONTEXT_REFRESH_SORT && f->fail_sort);
}
static void initialize(Fixture *f) {
    memset(f,0,sizeof *f); f->bias=0; f->flight.commands=&f->commands;
    assert(fa18_import_native_scene_records(&f->records,&f->commands,
        f->source,sizeof f->source,f->work,sizeof f->work));
    f->flight.player=f->records.aircraft; f->flight.viewed=f->records.aircraft+2;
    f->view.flight=&f->flight; f->ops=(FA18NativeContextRefreshOps){consume,f};
    f->refresh=(FA18NativeContextRefresh){.records=&f->records,.view=&f->view,.ops=&f->ops,
        .position_bias=&f->bias,.origin=f->origin,.cell_timer=&f->timer,.error_word=&f->error,
        .condition_key_a=&f->key_a,.condition_key_b=&f->key_b,.stage_selector=&f->selector,
        .current_colour=&f->colour,.line_style=&f->style,.context_selection=&f->context_select,
        .prepared=&f->prepared,.alternate=&f->alternate,.cell_checks=&f->checks,
        .view_mode=&f->view_mode,.fixed_readouts=&f->fixed,.frame_gate=&f->frame_gate};
}
int main(void) {
    static Fixture f; unsigned i;
    initialize(&f); f.view.update_mask=0x0b; f.records.records[2].word_06=8;
    f.records.records[2].word_08=12; f.selector=1; f.fixed=1;
    assert(fa18_refresh_native_context(&f.refresh));
    for(i=0;i<16;++i) assert((f.records.aircraft[i].flags&0x10) && (f.records.work[i][1]&0x10));
    assert(f.key_a==2 && f.key_b==3 && !f.view.update_mask && !f.prepared && f.alternate==1 && f.checks==1);
    assert(f.timer==0x4e && f.colour==15 && f.style==0x000fffff && !f.frame_gate && !f.error);
    assert(f.calls==7 && f.log[0]==FA18_CONTEXT_REFRESH_TEMPLATES &&
        f.log[1]==FA18_CONTEXT_REFRESH_TEMPLATES && f.log[2]==FA18_CONTEXT_REFRESH_TEMPLATES &&
        f.log[3]==FA18_CONTEXT_REFRESH_SORT && f.log[4]==FA18_CONTEXT_REFRESH_CACHE &&
        f.log[5]==FA18_CONTEXT_REFRESH_CONDITION_B && f.log[6]==FA18_CONTEXT_REFRESH_RENDER);

    initialize(&f); f.context_select=1; f.origin[0]=0x1fffffff; f.origin[2]=0x01000000;
    f.view.update_mask=1; f.consume_later=1;
    assert(fa18_refresh_native_context(&f.refresh));
    assert(f.key_a==31 && f.key_b==1 && f.error==0x27 && f.calls==4);
    assert(f.log[0]==FA18_CONTEXT_REFRESH_TEMPLATES && f.log[1]==FA18_CONTEXT_REFRESH_SORT &&
           f.log[2]==FA18_CONTEXT_REFRESH_CACHE && f.log[3]==FA18_CONTEXT_REFRESH_CONDITION_A);

    initialize(&f); f.bias=(int32_t)0xf7ffffffu;
    assert(fa18_refresh_native_context(&f.refresh) && !f.calls);
    initialize(&f); f.view.update_mask=0x0f; f.fail_sort=1;
    assert(!fa18_refresh_native_context(&f.refresh) && f.prepared && !f.view.update_mask && f.timer==0x4a);
    return 0;
}
