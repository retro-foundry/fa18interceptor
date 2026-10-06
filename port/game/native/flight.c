/* Connected source stage C0FECE and its scene constructors. */
#include "flight.h"
#include "records.h"
#include "setup.h"
#include "scene.h"
#include "hud.h"
#include "clock.h"
#include "input.h"
#include "frame_tail.h"
#include "../main_loop_timers.h"
#include "../render_buffers.h"
#include "../cockpit.h"
#include "../control_actions.h"
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
#include "../messages.h"
#include "../notify.h"
#include "../post_input_tick.h"
#include "../post_input.h"
#include "../scene_bootstrap.h"
#include "../template_gates.h"
#include "../control_records.h"
#include "../view.h"
#include "../input_device_callbacks.h"
#include "../menu_return.h"
#include "../menu_followup.h"
#include "../menu_outcome.h"
#include "../postflight_completion.h"
#include "../postflight_messages.h"
#include <stdio.h>
#include <stdlib.h>

static void refresh_child(void *context,enum ContextRefreshChild child);
static void storage_child(void *context,enum SceneBootstrapChild child) {
    int32_t *position=context;
    switch(child) {
    case BOOTSTRAP_CLEAR_STARTUP: clear_scene_startup_state(); break;
    case BOOTSTRAP_ENABLE_RECORDS: enable_scene_record_updates(); break;
    case BOOTSTRAP_CLEAR_BUFFERS: clear_render_buffers(); break; /* C2FD22 work banks */
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
    case BOOTSTRAP_RUN: {
        const SceneBootstrapHooks hooks={storage_child,NULL,position};
        bootstrap_scene(&hooks);break;
    }
    case BOOTSTRAP_FREE_VOICES: free_all_voices();break;
    case BOOTSTRAP_LOAD_MENU_TABLE: load_long_table(0xc08490);break;
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
    case MENU_MODE_NINE_POSITION: reset_scene_context(); break; /* C0924A */
    case MENU_MODE_NINE_VIEW: finish_scene_setup(); break; /* C082B0 */
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
void native_flight_reset_aircraft(NativeFrontend *game) {
    const MenuColdHooks hooks={cockpit_child,NULL,game};
    refresh_menu_cockpit(&hooks);
}
static void return_child(void *context,enum MenuReturnChild child) {
    (void)context;
    if(child==MR_CHOOSE_RESET || child==MR_LEAVE_RESET) reset_message_sequence();
    else { fprintf(stderr,"native menu-return child unavailable: %u\n",(unsigned)child);abort(); }
}
static int32_t result_message_child(void *context,enum PostflightMessageChild child) {
    NativeFrontend *game=context;
    switch(child) {
    case PM_RESET_SEQUENCE: reset_message_sequence();return 0;
    case PM_LOAD_MODE:
        /* C1643A's 78-byte config write uses the existing native save overlay. */
        native_frontend_save_log(game);return 0;
    default: fprintf(stderr,"native result message child unavailable: %u\n",(unsigned)child);abort();
    }
}
static void stage(void *context,gaddr routine) {
    NativeFrontend *game=context;
    const MenuOutcomeHooks outcome={outcome_child,NULL,game};
    if(routine==0xc0f920) {
        int32_t position[3]={0};
        const SceneBootstrapHooks hooks={storage_child,NULL,position};
        reset_sequence_after_bootstrap(&hooks);
        ++game->record_updates;
    } else if(routine==0xc0f992) {
        int32_t position[3]={0};
        const SceneBootstrapHooks hooks={storage_child,NULL,position};
        begin_sequence_after_bootstrap(&hooks);
        ++game->record_updates;
        if(rd_u32(STAGE_CALLBACK)==0xc0fcb4) game->screen=NATIVE_MENU;
    } else if(routine==0xc0fa04) finish_post_input_followup(NULL);
    else if(routine==0xc0fa4c) await_viewport_match();
    else if(routine==0xc0fa80) complete_post_input();
    else if(routine==0xc0fece) {
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
    else if(routine==0xc0fb70 || routine==0xc0fbb6) {
        const MenuReturnHooks hooks={return_child,NULL,game};
        if(routine==0xc0fb70) choose_menu_exit_after_countdown(&hooks);
        else leave_menu_on_key_or_message(&hooks);
    }
    else if(routine==0xc10970) follow_menu_return_context(NULL);
    else if(routine==0xc109ac) complete_menu_return_after_countdown(NULL);
    else if(routine==0xc11788) {
        ++game->postflight_callbacks;
        advance_postflight_completion(NULL);
        if(rd_u32(STAGE_CALLBACK)!=routine) ++game->postflight_resets;
    }
    else if(routine==0xc11830) restart_postflight_completion(NULL);
    else if(routine==0xc11872) expire_postflight_completion(NULL);
    else if(routine==0xc118a0) queue_postflight_failure(NULL);
    else if(routine==0xc118e6) end_postflight_message(NULL);
    else if(routine==0xc118fc) follow_postflight_message(NULL);
    else if(routine==0xc11934) clear_postflight_phase(NULL);
    else if(routine==0xc11958) follow_postflight_message_or_phase(NULL);
    else if(routine==0xc119d4) restart_postflight_after_countdown(NULL);
    else if(routine==0xc1104c) queue_postflight_end(NULL);
    else if(routine==0xc0f946) await_postflight_viewport(NULL);
    else if(routine==0xc0f974) mark_postflight_viewport_ready(NULL);
    else if(routine==0xc11078) raise_postflight_message_event(NULL);
    else if(routine==0xc110a4) {
        const PostflightMessageHooks messages={result_message_child,NULL,game};
        prepare_postflight_messages(0x4900,&messages);
    }
    else if(!native_setup_stage(game,routine)) { fprintf(stderr,"native flight stage unavailable: %08X\n",routine); abort(); }
}
static MainTimerBounds timer_child(void *context,enum MainTimerChild child) {
    (void)context;
    switch(child) {
    case MT_SAMPLE_BEGIN: case MT_SAMPLE_POLL: native_clock_sample(); break;
    case MT_COUNT_FIRST: tick_timer(0xc45886u); break;
    case MT_COUNT_SECOND: tick_timer(0xc45891u); break;
    case MT_COUNT_THIRD: tick_timer(TONE_MUTE); break;
    default: fprintf(stderr,"native frame timer child unavailable: %u\n",(unsigned)child); abort();
    }
    return (MainTimerBounds){0};
}
static int finish_frame_clock(NativeFrontend *game) {
    const MainTimerHooks hooks={.consume=timer_child};
    if(!poll_main_loop_timers(&hooks)) { ++game->timer_yields; return 0; }
    const uint16_t saved_tick=game->flight_saved_tick;
    if((saved_tick&7)==7) sample_main_loop_readout(&hooks);
    /* C53F9C releases graphics sprite zero. This runner never allocates
     * hardware sprites; cockpit/scenery rendering uses host plane storage. */
    if((saved_tick&31)==8) clear_page_plane_tops();
    else if(!rd_u8(ORIGIN_DETAIL_MODE) && (saved_tick&31)==16)
        request_cockpit_redraw(); /* C082B8; C10B90 is the aircraft reset. */
    if(!rd_u8(ORIGIN_GATE_A)) wr_u16(UPDATE_TICK,(uint16_t)(rd_u16(UPDATE_TICK)+1));
    game->flight_timer_pending=0;
    native_frame_debug_overlay();
    return 1;
}
int native_flight_enabled(const NativeFrontend *game) {
    const uint8_t mode=rd_u8(MODE_SELECT);
    return (mode==1 || mode==9 || mode==127 || (mode==3 && rd_u8(RECORDER_MODE)==3)) &&
        (game->screen==NATIVE_MODE_INTRO || game->screen==NATIVE_SCENE_SETUP);
}
int native_flight_tick(NativeFrontend *game) {
    if(!native_flight_enabled(game)) return 1;
    if(game->flight_timer_pending) return finish_frame_clock(game);
    const uint16_t saved_tick=rd_u16(UPDATE_TICK);
    native_input_process(game); /* C0F3C4, before the C0F5F8 stage tick. */
    const PostInputTickHooks hooks={stage,NULL,game};
    run_post_input_tick(&hooks);
    tick_notification_cadence(); /* C11B44 at C0EFEA. */
    /* C0EFD4 follows its stage tick with the record/context work while
     * POST_INPUT_AUX permits updates. View/control, projection, terrain and
     * the HUD/panel slice follow the record/context work. Complete C0EFD4
     * ownership and remaining end-of-frame drawing are pending. */
    if(rd_u8(POST_INPUT_AUX)) {
        update_view_controls(); /* C0F002, before the C1C63E record pass. */
        native_records_update();
        ++game->record_updates;
        native_scene_project();
        const ContextRefreshHooks refresh={refresh_child,NULL,game};
        refresh_context_packet(&refresh);
        if(!native_scene_draw(game)) return 1;
        update_message(); /* C11BFC at C0F12C, before instruments. */
        update_control_actions(NULL,NULL); /* C12950 at C0F132. */
        ++game->control_frames;
        native_hud_draw(saved_tick);
        ++game->hud_frames;
        native_frame_selection_cleanup();
        /* C25312/C2548A precede C0EFD4's game counter increment. Polls
         * resume on later PAL ticks without repeating physics or drawing. */
        game->flight_saved_tick=saved_tick;
        game->flight_timer_pending=1;
        const MainTimerHooks timers={.consume=timer_child};
        begin_main_loop_timers(&timers);
        return finish_frame_clock(game);
    }
    update_control_actions(NULL,NULL); /* C12950 idle branch at C0F370. */
    ++game->control_frames;
    native_frame_debug_overlay();
    return 1;
}
