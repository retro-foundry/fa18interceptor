#ifndef FA18_NATIVE_FRONTEND_H
#define FA18_NATIVE_FRONTEND_H
#include "storage.h"
#include "../../amiga/ilbm.h"
enum NativeScreen { NATIVE_SPLASH,NATIVE_CREDITS,NATIVE_ENLISTMENT,NATIVE_CALLSIGN,NATIVE_MENU };
typedef struct {
    NativeStorage storage;
    AmigaIlbm splash;
    enum NativeScreen screen;
    unsigned ticks,screen_ticks;
    uint16_t palette[32];
    uint8_t indices[320*256];
    unsigned glyphs;
    int name_finished;
    char config_path[4096];
} NativeFrontend;
int native_frontend_open(NativeFrontend *game,const char *adf,const char *save_dir,char *error,size_t capacity);
void native_frontend_close(NativeFrontend *game);
void native_frontend_tick(NativeFrontend *game);
void native_frontend_key(NativeFrontend *game,int key);
const char *native_frontend_screen(const NativeFrontend *game);
#endif
