/* Connected source stage C0FECE and its scene constructors. */
#include "flight.h"
#include "records.h"
#include "setup.h"
#include "../globals.h"
#include "../menu_transition.h"
#include "../scene_dispatch.h"
#include "../scene_setup.h"
#include "../menu_cold.h"
#include "../context_refresh.h"
#include "../template_placements.h"
#include "../placement_order.h"
#include "../fixed_math.h"
#include "../stages.h"
#include "../audio.h"
#include "../post_input_tick.h"
#include "../scene_bootstrap.h"
#include "../template_gates.h"
#include "../control_records.h"
#include "../view.h"
#include "../input_device_callbacks.h"
#include "../menu_return.h"
#include "../menu_followup.h"
#include "../menu_outcome.h"
#include <stdio.h>
#include <stdlib.h>

static void refresh_child(void *context,enum ContextRefreshChild child);
static void storage_child(void *context,enum SceneBootstrapChild child) {
    int32_t *position=context;
    switch(child) {
    case BOOTSTRAP_CLEAR_STARTUP: clear_scene_startup_state(); break;
    case BOOTSTRAP_ENABLE_RECORDS: enable_scene_record_updates(); break;
    case BOOTSTRAP_CLEAR_BUFFERS: native_frontend_clear_text(); break;
    case BOOTSTRAP_PREPARE_PLAYER: prepare_player_record(); break;
    case BOOTSTRAP_START_POSITION: start_position(position); break;
    case BOOTSTRAP_SET_OBSERVER: set_observer_position(position[0],position[1],position[2]); break;
    case BOOTSTRAP_PLACE_VIEW: reset_scene_recorder(); break;
    case BOOTSTRAP_BUILD_GATES: build_template_bit_gates(); break;
    case BOOTSTRAP_UPDATE_RECORDS: native_records_update(); break;
    case BOOTSTRAP_REFRESH_CONTEXT: {
        const ContextRefreshHooks hooks={refresh_child,NULL,context};
        refresh_context_packet(&hooks); break;
    }
    default: fprintf(stderr,"native storage child unavailable: %u\n",(unsigned)child); abort();
    }
}
void native_flight_initialize(NativeFrontend *game) {
    /* C11B0E clears sixteen longs through this pointer. Ordinary host
     * allocation; contents are produced by game owners, never a capture. */
    wr_u32(LONG_TABLE,0x3000);
    int32_t position[3]={0};
    const SceneBootstrapHooks hooks={storage_child,NULL,position};
    bootstrap_scene(&hooks);
    game->record_updates=1;
}
static void refresh_child(void *context,enum ContextRefreshChild child) {
    (void)context;
    switch(child) {
    case CONTEXT_REFRESH_TEMPLATES: refresh_template_placements(); break;
    /* Original C1E328's saved-stack test is nonzero in the Free Flight
     * C0FECE/C1C860 call (A4 byte $9E in the live boundary trace). This
     * route sorts all pending lists; no CPU stack is retained by the host. */
    case CONTEXT_REFRESH_SORT: sort_display_list(1); break;
    case CONTEXT_REFRESH_CACHE: order_placement_cache(); break;
    case CONTEXT_REFRESH_CONDITION_A: update_condition_a(); break;
    case CONTEXT_REFRESH_CONDITION_B: update_condition_b(); break;
    default: fprintf(stderr,"native context child unavailable: %u\n",(unsigned)child); abort();
    }
}
static MenuTransitionResult transition_child(void *context,enum MenuTransitionCall child,uint32_t value) {
    (void)context;
    const MenuColdHooks cold={0};
    switch(child) {
    case MENU_STOP_ZERO: free_voice(0); break;
    case MENU_STOP_ONE: free_voice(1); break;
    case MENU_SOUND_PAIR: start_menu_alert_pair(value,&(MenuTransitionHooks){transition_child,NULL,context}); break;
    case MENU_PAIR_FIRST: play_sound(35,0,40); break;
    case MENU_PAIR_SECOND: play_sound(36,1,40); break;
    case MENU_NOISE: play_noise((int32_t)value); break;
    case MENU_SCRIPTED_NOISE: play_engine(0x300,(int32_t)value); break;
    case MENU_DELAY_RESET: case MENU_MODE_NINE_RESET: reset_message_sequence(); break;
    case MENU_DELAY_ROOT: case MENU_MODE_NINE_ROOT: initialize_scene_from_mode(NULL); break;
    case MENU_DELAY_VIEWPORT: clear_long_table(); break;
    case MENU_MODE_ONE_ROOT: set_menu_position_preset(&cold,0); break;
    case MENU_REFRESH: {
        const ContextRefreshHooks hooks={refresh_child,NULL,context};
        refresh_context_packet(&hooks); break;
    }
    default: fprintf(stderr,"native flight transition child unavailable: %u\n",(unsigned)child); abort();
    }
    return (MenuTransitionResult){0,0};
}
static void outcome_child(void *context,enum MenuOutcomeChild child,uint32_t value) {
    (void)context; (void)value;
    switch(child) {
    case MO_COUNTDOWN_RESET: case MO_OUTCOME_RESET: case MO_MESSAGE_RESET:
    case MO_DELAYED_RESET: reset_message_sequence(); break;
    default: fprintf(stderr,"native outcome child unavailable: %u\n",(unsigned)child); abort();
    }
}
static void cockpit_child(void *context,enum MenuColdChild child) {
    NativeFrontend *game=context;
    if(child==MENU_COLD_POSITION) reset_scene_recorder();
    else if(child==MENU_COLD_UPDATE) { native_control_records_update(); ++game->record_updates; }
    else { fprintf(stderr,"native cockpit child unavailable: %u\n",(unsigned)child); abort(); }
}
void native_flight_refresh_cockpit(NativeFrontend *game) {
    const MenuColdHooks hooks={cockpit_child,NULL,game};
    refresh_menu_cockpit(&hooks);
}
static void stage(void *context,gaddr routine) {
    NativeFrontend *game=context;
    const MenuOutcomeHooks outcome={outcome_child,NULL,game};
    if(routine==0xc0fece) {
        const MenuTransitionHooks hooks={transition_child,NULL,game};
        advance_delayed_menu(&hooks);
        if(rd_u32(STAGE_CALLBACK)!=routine) {
            game->scene_selected=1;
            game->screen=NATIVE_SCENE_SETUP;
        }
    } else if(routine==0xc101fc) reset_menu_viewport_after_countdown(NULL);
    else if(routine==0xc10228) enter_menu_mode_four(NULL);
    else if(routine==0xc10678) advance_menu_mode_messages(NULL);
    else if(routine==0xc1072e) queue_menu_message_four(NULL);
    else if(routine==0xc1075a) start_menu_outcome(&outcome);
    else if(routine==0xc1078a) finish_menu_outcome(&outcome);
    else if(routine==0xc10970) follow_menu_return_context(NULL);
    else if(routine==0xc109ac) complete_menu_return_after_countdown(NULL);
    else if(!native_setup_stage(game,routine)) { fprintf(stderr,"native flight stage unavailable: %08X\n",routine); abort(); }
}
enum { PALETTE_FRAME=0x3080 };
static int32_t palette_child(void *context,enum InputDeviceChild child) {
    NativeFrontend *game=context;
    switch(child) {
    case IDC_PALETTE_FIRST: case IDC_PALETTE_SECOND: {
        gaddr source=rd_u32(PALETTE_FRAME-16);
        for(unsigned i=0;i<16;++i) game->palette[i]=rd_u16(source+2*i);
        break;
    }
    case IDC_PALETTE_STABLE: break; /* host palette is already published */
    case IDC_FADE: fade_master_volume(); break;
    default: fprintf(stderr,"native palette child unavailable: %u\n",(unsigned)child); abort();
    }
    return 0;
}
void native_flight_tick(NativeFrontend *game) {
    if(game->screen!=NATIVE_MODE_INTRO && game->screen!=NATIVE_SCENE_SETUP) return;
    /* Connect Free Flight first. Other mode banners retain their existing
     * endpoint until their distinct scene/record-update paths are owned. */
    if(rd_u8(MODE_SELECT)!=1) return;
    const InputDeviceHooks palette={palette_child,NULL,game};
    advance_viewport_palette(PALETTE_FRAME,&palette);
    const PostInputTickHooks hooks={stage,NULL,game};
    run_post_input_tick(&hooks);
    /* C0EFD4 follows its stage tick with the record/context work while
     * POST_INPUT_AUX permits updates. The intervening input/view and draw
     * children remain pending; this is the connected record slice only. */
    if(rd_u8(POST_INPUT_AUX)) {
        native_records_update();
        ++game->record_updates;
        const ContextRefreshHooks refresh={refresh_child,NULL,game};
        refresh_context_packet(&refresh);
    }
}
