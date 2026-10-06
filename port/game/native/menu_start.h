#ifndef FA18_NATIVE_MENU_START_H
#define FA18_NATIVE_MENU_START_H
typedef struct { unsigned ready_tick; int pending; } NativeMenuSetup;
/* Connected C0FBE0 owner, yielding only across C0E78A's busy pause. */
void native_menu_begin(NativeMenuSetup *setup,unsigned ticks);
int native_menu_resume(NativeMenuSetup *setup,unsigned ticks);
#endif
