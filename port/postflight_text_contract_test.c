#include "postflight_text.h"
#include "post_input_tick.h"
#include "post_input_followup.h"
#include "hex_field.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18NativePostflightText *publisher;
    uint16_t *palette;
    unsigned calls;
    int fail;
} Bootstrap;
static int bootstrap(void *context) {
    Bootstrap *b=context;
    ++b->calls;
    b->publisher->display->stable_palette=b->palette;
    b->publisher->flight->commands->indexed.mode=9;
    return !b->fail;
}
typedef struct { Bootstrap *bootstrap; unsigned calls; } Dispatch;
static int dispatch(void *context,FA18StageCallback callback) {
    Dispatch *d=context;
    FA18NativePostflightTextOps ops={bootstrap,d->bootstrap};
    ++d->calls;
    assert(callback==FA18_STAGE_C0F812);
    return fa18_publish_native_postflight_text(d->bootstrap->publisher,&ops)?0:-1;
}
int main(void) {
    FA18CommandInput commands={0}; FA18FlightCommandState flight={0};
    FA18NativeInputDisplay display={0}; FA18CommandAudio audio={0};
    FA18PostInputTickState tick={0}; FA18NativePostflightText p={0};
    uint16_t seed[64],unused[32],checksums[3]={0xc560,0x7e70,0x4de8};
    uint8_t text[64],hex[512];
    Bootstrap b={&p,seed+1,0,0}; Dispatch d={&b,0};
    FA18PostInputTickHooks hooks={dispatch,NULL,&d};
    FA18NativePostflightTextOps ops={bootstrap,&b};
    unsigned i;
    memset(text,' ',sizeof text);
    for(i=0;i<64;++i) seed[i]=(uint16_t)(0x1000+i);
    for(i=0;i<32;++i) unused[i]=0xeeee;
    flight.commands=&commands;
    display.stable_palette=unused;
    p.display=&display; p.flight=&flight; p.countdown=&tick.countdown;
    p.callback=&tick.callback; p.palette_seed=seed; p.text_descriptor=text;
    p.text_bytes=sizeof text; p.audio=&audio;
    for(i=0;i<3;++i) p.checksums[i]=&checksums[i];
    audio.volume_fading=0xff; /* Actual child early-return branch. */
    tick.entry_guard=-1; tick.callback=FA18_STAGE_C0F812;
    assert(!fa18_run_post_input_tick(&tick,&hooks));
    assert(b.calls==1 && d.calls==1 && tick.tick_count==1 && tick.countdown==3);
    assert(tick.callback==FA18_STAGE_C11446 && commands.indexed.mode==0 && flight.pause==0xff && p.transition==0xff);
    /* Sequential forward overlap propagates the first source word. The
     * replaced palette is read after bootstrap, and the old buffer is untouched. */
    for(i=0;i<32;++i) assert(seed[i+1]==0x1000 && unused[i]==0xeeee);
    checksums[0]=0xabcd; checksums[1]=0; /* Last mismatch zero chooses audio. */
    assert(fa18_publish_native_postflight_text(&p,&ops) && tick.callback==FA18_STAGE_C11446 && text[21]=='3');
    checksums[1]=0x7e70; checksums[2]=1;
    assert(fa18_publish_native_postflight_text(&p,&ops) && tick.callback==FA18_STAGE_C113E4);
    assert(text[21]=='5' && text[22]==' ' && !memcmp(text+23,"   1",4));
    /* Failure retains actual bootstrap effects but skips parent stores. */
    b.fail=1; tick.countdown=17; flight.pause=2;
    assert(!fa18_publish_native_postflight_text(&p,&ops) && commands.indexed.mode==9 && tick.countdown==17 && flight.pause==2);
    b.fail=0; p.text_bytes=26;
    assert(!fa18_publish_native_postflight_text(&p,&ops) && tick.countdown==17);
    memset(hex,'0',sizeof hex);
    assert(fa18_format_native_hex_field(hex,sizeof hex,256,0xabcd,4));
    assert(hex[256]=='0' && !memcmp(hex+257,"ABCD",4));
    assert(fa18_format_native_hex_field(hex,sizeof hex,256,0,127));
    for(i=257;i<383;++i) assert(hex[i]==' ');
    assert(hex[383]=='0');
    memset(hex,'0',sizeof hex);
    assert(fa18_format_native_hex_field(hex,sizeof hex,256,0xdeadbeef,0x80));
    for(i=129;i<256;++i) assert(hex[i]==' ');
    assert(hex[128]=='0' && hex[256]=='0');
    assert(fa18_format_native_hex_field(hex,1,0,0,0xff));
    assert(!fa18_format_native_hex_field(hex,16,130,0,0x80));
    for(i=3;i<16;++i) assert(hex[i]==' ');
    return 0;
}
