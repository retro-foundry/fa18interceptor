#ifndef FA18_NATIVE_FRONTEND_H
#define FA18_NATIVE_FRONTEND_H
#include "storage.h"
#include "menu_start.h"
#include "audio.h"
#include "../input_display_setup.h"
#include "../../amiga/ilbm.h"
#include "../../amiga/host_compat.h"
enum NativeScreen { NATIVE_SPLASH,NATIVE_CREDITS,NATIVE_ENLISTMENT,NATIVE_CALLSIGN,NATIVE_MENU,
    NATIVE_MODE_INTRO,NATIVE_MISSIONS,NATIVE_PILOT_LOG,NATIVE_SCENE_SETUP };
typedef struct NativeFrontend NativeFrontend;
enum NativeInputReturnOwner { NATIVE_INPUT_RETURN_UNKNOWN, NATIVE_INPUT_RETURN_MESSAGE,
                              NATIVE_INPUT_RETURN_PAGE_CLEAR, NATIVE_INPUT_RETURN_HUD_BAR,
                              NATIVE_INPUT_RETURN_HUD_TEXT, NATIVE_INPUT_RETURN_REDRAW,
                              NATIVE_INPUT_RETURN_DEBUG_TEXT, NATIVE_INPUT_RETURN_SCENE_LABEL,
                              NATIVE_INPUT_RETURN_HUD_LINE, NATIVE_INPUT_RETURN_GRID_MARKER,
                              NATIVE_INPUT_RETURN_VIEW_KEY };
typedef struct { uint8_t value; enum NativeInputReturnOwner owner; } NativeInputReturn;
enum NativeFrameBoundary { NATIVE_FRAME_BODY_BEGIN, NATIVE_FRAME_BODY_END, NATIVE_FRAME_INPUT_BEGIN,
                           NATIVE_FRAME_OWNER_EXIT };
struct NativeFrontend {
    NativeStorage storage;
    AmigaOfs disk;
    AmigaHostCompat files;
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
    /* Completed domain return for the next first pending command. Drawing
     * paths without reconstructed return contracts leave UNKNOWN. */
    NativeInputReturn completed_input_return;
    uint16_t mouse_buttons,joystick_directions;
    uint8_t mouse_x_counter,mouse_y_counter;
    int input_server_installed;
    unsigned update_iterations;
    void (*begin_update)(NativeFrontend *game,void *context);
    void *update_context;
    /* Optional diagnostics before input, at C0EFEA/C0F3C0 or C0DA38's exit. */
    void (*observe_frame)(NativeFrontend *game,enum NativeFrameBoundary boundary,
                          uint16_t saved_tick,void *context);
    void *frame_context;
    char config_path[4096];
};
int native_frontend_open(NativeFrontend *game,const char *adf,const char *save_dir,char *error,size_t capacity);
void native_frontend_mouse(NativeFrontend *game,int dx,int dy);
void native_frontend_button(NativeFrontend *game,unsigned button,int down);
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
