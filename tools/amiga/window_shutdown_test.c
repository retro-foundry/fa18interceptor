/* Drive the production runner's SDL_QUIT path with no visible window.
 * Supply its normal --adf/--window options, --frames 100 and output paths.
 * A one-shot host timer posts the same event as closing the window. */
#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif
#include <SDL.h>
#include <stdio.h>
#include <string.h>
static SDL_atomic_t posted;
static Uint32 close_window(Uint32 interval,void *context) {
    SDL_Event event; (void)interval; (void)context;
    memset(&event,0,sizeof event); event.type=SDL_QUIT;
    SDL_AtomicSet(&posted,SDL_PushEvent(&event)==1);
    return 0;
}
static void fixture_present(SDL_Renderer *renderer) {
    static int scheduled;
    SDL_RenderPresent(renderer);
    /* Start after the first actual frame; SDL display initialization can
     * take longer than the delay on some hosts. */
    if (!scheduled) {
        scheduled=1;
        if (!SDL_AddTimer(100,close_window,NULL))
            fprintf(stderr,"shutdown fixture timer failed: %s\n",SDL_GetError());
    }
}
#define SDL_RenderPresent fixture_present
#define main fa18_runner_main
#include "../../port/recomp/recomp_main.c"
#undef main
#undef SDL_RenderPresent
int main(int argc,char **argv) {
    if (SDL_setenv("SDL_VIDEODRIVER","dummy",1) || SDL_Init(SDL_INIT_TIMER|SDL_INIT_EVENTS)) {
        fprintf(stderr,"shutdown fixture SDL initialization failed: %s\n",SDL_GetError()); return 1;
    }
    int result=fa18_runner_main(argc,argv);
    if (!result && !SDL_AtomicGet(&posted)) {
        fprintf(stderr,"shutdown fixture ended before its close event\n"); return 1;
    }
    return result;
}
