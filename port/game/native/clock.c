#include "clock.h"
#include "../globals.h"
#include "../menu_context_finish.h"
#include <stdlib.h>

static unsigned clock_ticks;
void native_clock_set(unsigned pal_ticks) { clock_ticks=pal_ticks; }
void native_clock_request(void) {
    wr_u32(MENU_TIME_REQUEST+32,clock_ticks/50);
    wr_u32(MENU_TIME_REQUEST+36,(clock_ticks%50)*20000u);
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
