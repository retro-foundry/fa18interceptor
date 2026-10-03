/* Test-only snapshot of the original blitter's persistent data latches. */
#include "../../port/machine/blitter.c"
void fa18_hud_blitter_capture(uint16_t out[4]) {
    out[0]=bltaold; out[1]=bltbold; out[2]=bltbhold; out[3]=bltddat;
}
void fa18_hud_blitter_restore(void) {
    static uint16_t original[4]; static int saved;
    if(!saved) { fa18_hud_blitter_capture(original); saved=1; }
    bltaold=original[0]; bltbold=original[1]; bltbhold=original[2]; bltddat=original[3];
}
