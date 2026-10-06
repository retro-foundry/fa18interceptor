#ifndef FA18_NATIVE_FRONTEND_H
#define FA18_NATIVE_FRONTEND_H
#include "storage.h"
#include "menu_start.h"
#include "audio.h"
#include "../input_display_setup.h"
#include "../../amiga/ilbm.h"
enum NativeScreen { NATIVE_SPLASH,NATIVE_CREDITS,NATIVE_ENLISTMENT,NATIVE_CALLSIGN,NATIVE_MENU,
    NATIVE_MODE_INTRO,NATIVE_MISSIONS,NATIVE_PILOT_LOG,NATIVE_SCENE_SETUP };
typedef struct NativeFrontend NativeFrontend;
struct NativeFrontend {
    NativeStorage storage;
    AmigaIlbm splash;
    enum NativeScreen screen;
    unsigned ticks,screen_ticks;
    NativeMenuSetup menu_setup;
    NativeAudio audio;
    uint16_t palette[32];
    uint8_t indices[320*256];
    unsigned glyphs;
    unsigned record_updates;
    unsigned scene_frames,terrain_polygons,model_calls,hud_frames,control_frames;
    unsigned shift_keys;
    int name_finished;
    int scene_selected;
    int flight_timer_pending;
    uint16_t flight_saved_tick;
    unsigned timer_yields;
    OuterDisplayState display;
    int display_pending,display_drawing,display_wait_pending;
    unsigned display_wait_tick,displayed_page,display_publications,display_yields;
    unsigned postflight_callbacks,postflight_resets;
    uint8_t input_keys[256];
    unsigned input_read,input_count,input_passes,input_events;
    uint16_t mouse_buttons,joystick_directions;
    unsigned update_iterations;
    void (*begin_update)(NativeFrontend *game,void *context);
    void *update_context;
    char config_path[4096];
};
int native_frontend_open(NativeFrontend *game,const char *adf,const char *save_dir,char *error,size_t capacity);
void native_frontend_close(NativeFrontend *game);
void native_frontend_tick(NativeFrontend *game);
void native_frontend_key(NativeFrontend *game,int key);
void native_frontend_event(NativeFrontend *game,int key,int down);
/* Raw Amiga events from recordings retain their original key identities. */
void native_frontend_raw_event(NativeFrontend *game,uint8_t raw,int down);
/* Source C11312 reset and native work-buffer ownership. */
void native_frontend_clear_text(void);
void native_frontend_start_menu(NativeFrontend *game);
void native_frontend_enlist(NativeFrontend *game);
void native_frontend_save_log(NativeFrontend *game);
const char *native_frontend_screen(const NativeFrontend *game);
#endif
