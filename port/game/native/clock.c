#include "clock.h"
#include "../globals.h"
#include "../menu_context_finish.h"
#include <stdlib.h>

static unsigned clock_ticks;
static NativeClockRead clock_read;
static void *clock_context;
static NativeClockStats clock_stats;
void native_clock_set_source(NativeClockRead read,void *context) {
    clock_read=read;clock_context=context;clock_stats=(NativeClockStats){0};
}
NativeClockStats native_clock_stats(void) { return clock_stats; }
void native_clock_set(unsigned pal_ticks) { clock_ticks=pal_ticks; }
void native_clock_request(void) {
    const uint64_t sample=clock_read?clock_read(clock_context):(uint64_t)clock_ticks*20000u;
    wr_u32(MENU_TIME_REQUEST+32,(uint32_t)(sample/1000000u));
    wr_u32(MENU_TIME_REQUEST+36,(uint32_t)(sample%1000000u));
    ++clock_stats.requests;clock_stats.last_microseconds=sample;
    clock_stats.low_bits_seen|=(uint32_t)1u<<(sample&31u);
}
static int32_t request(void *context,enum MenuContextChild child) {
    (void)context;
    if(child!=MC_TIMER_REQUEST) abort();
    native_clock_request(); return 0;
}
void native_clock_sample(void) {
    const MenuContextHooks hooks={.consume=request};
    read_menu_time_sample(&hooks); /* C16D04, including request/result stores. */
}
