/* Native menu composition: C1AD74 -> C1BC72/C1BD78 -> C0FCB4.
 * C1017E publishes available missions; C24E8A formats the pilot log.
 * No source instruction, CPU adapter or device register is executed. */
#include "menu.h"
#include "viewport.h"
#include "../fault.h"
#include "../globals.h"
#include "../command_selection.h"
#include "../command_publication.h"
#include "../context_publication.h"
#include "../context_commands.h"
#include "../audio.h"
#include "../indexed_commands.h"
#include "../menu_transition.h"
#include "../menu_cold.h"
#include "../numbers.h"
#include "../stages.h"
#include "../flight_commands.h"
#include "../player_input.h"
#include "../command_dispatch.h"
#include "../view.h"
#include "../cockpit.h"
#include "../messages.h"
#include "../../amiga/host_keys.h"
#include <stdio.h>
#include <stdlib.h>

void native_menu_initialize(void) {
    /* C0EE5C uses half of available memory, capped at $1E848, then /16.
     * Host storage can satisfy the cap. C09266 initializes playback cursors.
     * These are ordinary host buffers, not captured recorder contents. */
    const unsigned capacity=0x1e848u>>4;
    wr_u32(RECORDER_START,0x20000); wr_u32(RECORDER_SIZE,capacity);
    wr_u32(RECORDER_WORDS,0x28000);
    wr_u32(RECORDER_CURSOR,rd_u32(RECORDER_START));
    wr_u32(RECORDER_WORD_CURSOR,rd_u32(RECORDER_WORDS));
    wr_u32(PLAYBACK_BYTES,rd_u32(RECORDER_START));
    wr_u32(PLAYBACK_WORDS,rd_u32(RECORDER_WORDS)+4);
    /* Source startup's command mode gate permits indexed menu selection. */
    wr_u8(COMMAND_MODE_GATE,1);
}
static uint32_t mode_changed(void *context) {
    (void)context;
    play_status_tone(); /* C3318E, including its TONE_MUTE/source voice gates. */
    return 2; /* C3318E publishes tone kind 2 in D0 before its mute gates. */
}
unsigned native_menu_selected_mode(const NativeFrontend *game) {
    (void)game; return rd_u8(MODE_SELECT);
}
static ContextCommandResult context_child(void *context,enum ContextCommandChild which,const ContextCommandInput *input) {
    (void)context; (void)input;
    /* Both source call sites invoke C0F4A6; map entry uses the same owner
     * as the already-connected request command. */
    if(which==CONTEXT_COMMAND_REQUEST_VOICES || which==CONTEXT_COMMAND_MAP_VOICES) {
        free_voice(0); free_voice(1); free_voice(2); free_voice(3);
        return (ContextCommandResult){12,{0,0,0}};
    }
    fprintf(stderr,"native input context child unavailable: %u\n",(unsigned)which); abort();
}
static void callback_reset_child(NativeFrontend *game,enum CommandDispatchChild child) {
    /* C1C2B8 -> C06BF0 removes the PAL callback, calls the release build's
     * empty C06C02, then reinstalls it. Do not reset mouse counters/bounds. */
    switch(child) {
    case COMMAND_RESET_BEGIN: native_viewport_remove_callback(game);break;
    case COMMAND_RESET_FAULT: case COMMAND_INVALID_INPUT_FAULT: fault_hook();break;
    case COMMAND_RESET_FINISH: native_viewport_install_callback(game);break;
    default: abort();
    }
}
typedef struct {
    CommandRequest request;
    int16_t carried_event;
} NativeFlightCommand;
static FlightCommandResult flight_key_child(void *context,enum FlightCommandChild child) {
    const NativeFlightCommand *command=context;
    const CommandRequest *request=&command->request;
    switch(child) {
    case FLIGHT_EJECT_TOGGLE: {
        const ContextPublicationHooks hooks={0};
        const uint8_t event=publish_context_toggle_command((uint8_t)request->raw_event,BAR_E_FLAG,&hooks);
        return (FlightCommandResult){(request->raw_event&0xffffff00u)|event,command->carried_event};
    }
    case FLIGHT_Y_UP: set_stick_y(STICK_UP);break;
    case FLIGHT_Y_DOWN: set_stick_y(STICK_DOWN);break;
    case FLIGHT_Y_RELEASE: set_stick_y(0);break;
    /* Historical child names: C1B558 supplies $08; C1B55C supplies $04. */
    case FLIGHT_X_RIGHT: set_stick_x(STICK_LEFT);break;
    case FLIGHT_X_LEFT: set_stick_x(STICK_RIGHT);break;
    case FLIGHT_X_RELEASE: set_stick_x(0);break;
    case FLIGHT_THROTTLE_RELEASE: case FLIGHT_THROTTLE_MODE_RELEASE:
        reset_throttle_input_state();break;
    case FLIGHT_SPACE_PRESS: dispatch_space_command_effect();break;
    case FLIGHT_SPACE_RELEASE: set_event_bit_and_clear_command_word_bit();break;
    case FLIGHT_THROTTLE_MODE: case FLIGHT_HOOK: case FLIGHT_WEAPON_ENABLE:
    case FLIGHT_WEAPON_MODE: case FLIGHT_GEAR: case FLIGHT_NEXT_TARGET:
    case FLIGHT_RADAR_RANGE: case FLIGHT_TARGET: case FLIGHT_ECM:
        play_status_tone_outside_context();
        return (FlightCommandResult){rd_u8(CONTEXT_SELECT)?request->raw_event:2,0};
    case FLIGHT_HOOK_SOUND: case FLIGHT_WEAPON_SOUND:
        post_message((uint16_t)request->raw_event);
        return (FlightCommandResult){request->raw_event&0xffffff00u,0};
    case FLIGHT_FLARE_SOUND: case FLIGHT_CHAFF_SOUND:
        /* C25704 masks the event's low byte and preserves the word saved
         * at C1C0FC/C1C18A. These children post messages, not direct tones. */
        post_message((uint16_t)request->raw_event);
        return (FlightCommandResult){request->raw_event&0xffffff00u,command->carried_event};
    case FLIGHT_FLARE_SPAWN:
        /* C1C164 -> C17F8C: SHIFT-F in mode 6 starts sound 6. */
        start_sound_6(0x1c,0x30);break;
    default: fprintf(stderr,"native flight key child unavailable: %u\n",(unsigned)child);abort();
    }
    return (FlightCommandResult){request->raw_event,0};
}
static void flight_arguments(void *context,enum FlightCommandPhase phase,
                             uint32_t value,uint32_t limit,gaddr address) {
    NativeFlightCommand *command=context;
    CommandRequest *arguments=&command->request;
    (void)limit;(void)address;
    if(phase==FLIGHT_SOUND_SWAP) arguments->raw_event=value;
    else if(phase==FLIGHT_SOUND_WORD)
        arguments->raw_event=(arguments->raw_event&0xffff0000u)|(uint16_t)value;
    else if(phase==FLIGHT_SOUND_CARRY) command->carried_event=(int16_t)value;
}
unsigned native_menu_raw_key(int key,int down) {
    /* Host/replay keys identify physical keys. C331CE translates raw events
     * for typed text and deliberately omits rudder and other control keys. */
    const int raw=key>=256 && key<0x40000000
        ?amiga_host_legacy_raw_key(key):amiga_host_raw_key(key);
    return raw<0?0xffu:(unsigned)raw|(down?0u:0x80u);
}
void native_menu_key(NativeFrontend *game,int key,int down) {
    unsigned raw=native_menu_raw_key(key,down);
    if(raw==0xff) return;
    CommandRequest request=select_keyboard_command(raw,NULL);
    uint32_t event=request.raw_event;
    if(is_indexed_command(request.action)) {
        const IndexedCommandHooks hooks={mode_changed,NULL,game};
        event=execute_indexed_command(&request,0,&hooks);
    } else if(request.action==COMMAND_SIGN_INPUT) {
        /* C1C224, same store as execute_flight_command. */
        wr_u8(SEQUENCE_PHASE,request.modifier?0xff:1);
    } else if(is_flight_command(request.action)) {
        NativeFlightCommand arguments={request,0};
        const FlightCommandHooks hooks={flight_key_child,flight_arguments,&arguments};
        event=execute_flight_command(&request,0,&hooks);
    } else if(is_context_command(request.action)) {
        const ContextCommandHooks hooks={context_child,NULL,game};
        event=execute_context_command(&request,&hooks);
    } else if(request.action==COMMAND_RESET_CONTEXT) {
        callback_reset_child(game,COMMAND_RESET_BEGIN);
        callback_reset_child(game,COMMAND_RESET_FAULT);
        callback_reset_child(game,COMMAND_RESET_FINISH);
        return;
    } else if(request.action!=COMMAND_QUEUE_ONLY && request.action!=COMMAND_COUNTER_WAIT && request.action!=COMMAND_FINISH_EVENT) {
        fprintf(stderr,"native key action unavailable: %u\n",(unsigned)request.action); abort();
    }
    /* C1AD72 and C1C2B6 return without queue publication. Other connected
     * action bodies flow through C1C23C, the queue publication owner. */
    if(request.action==COMMAND_COUNTER_WAIT) return;
    if(request.action==COMMAND_FINISH_EVENT) return;
    if(game->screen==NATIVE_SCENE_SETUP || game->screen==NATIVE_MODE_INTRO)
        publish_command_event((uint8_t)event,NULL);
}

typedef struct {
    NativeFrontend *game;
    NativeFlightCommand flight;
    int selection_known;
} NativeCommand;
static void prepare_action(void *context,const CommandRequest *request) {
    ((NativeCommand *)context)->flight.request=*request;
}
static void selection_value(void *context,enum CommandSelectionPhase phase,uint32_t value,uint32_t limit) {
    NativeCommand *command=context;
    (void)limit;
    if(phase==COMMAND_READ_BLOCK) {
        /* C1AE02/C1AE08 load and mask the blocked-command byte before
         * direct keys. A selected flare/chaff therefore carries low byte 0.
         * Only that byte can reach C1C23C's queue; the high byte is dead. */
        command->flight.carried_event=(int16_t)value;
        command->selection_known=1;
    }
}
static int16_t carried_selection(void *context) {
    NativeCommand *command=context;
    const CommandRequest *r=&command->flight.request;
    if(r->action==COMMAND_CHAFF || (r->action==COMMAND_FLARE && !r->modifier)) {
        const gaddr stock=r->action==COMMAND_CHAFF?MISSION_LEVEL_A:MISSION_LEVEL_B;
        /* A successful pending action saves its own event (0) before the
         * child. Once C1C23C's KEY_TAKEN gate is set, the inherited event
         * is dead: publication skips both its release test and queue writes.
         * C25704 still owns the stock message and the exit clears modifiers. */
        if(command->selection_known) return command->flight.carried_event;
        if(rd_s8(stock)>1) return 0;
        if(rd_u8(KEY_TAKEN)) return 0;
        if(command->game->completed_input_return.owner!=NATIVE_INPUT_RETURN_UNKNOWN) {
            command->flight.carried_event=command->game->completed_input_return.value;
            return command->flight.carried_event;
        }
        fputs("native input missing depleted recorder countermeasure carry\n",stderr);abort();
    }
    /* C1BCEE's recorder $FD arm changes only the inherited selection,
     * then C1BEDA unconditionally publishes the original event. That
     * selection is dead to this action's RAM and queue behavior. Other
     * indexed arms construct their own selection before using it. */
    return 0;
}
static uint32_t view_child(void *context,enum ViewCommandChild child) {
    const NativeCommand *command=context;
    if(child==VIEW_COMMAND_ZOOM_MAXIMUM) set_zoom_maximum();
    else if(child==VIEW_COMMAND_REDRAW) request_cockpit_redraw();
    else abort();
    /* C08324/C082B8 preserve the event in D0. */
    return command->flight.request.raw_event;
}
static void dispatch_child(void *context,enum CommandDispatchChild child) {
    NativeCommand *command=context;
    callback_reset_child(command->game,child);
}
static void dispatch(NativeFrontend *game,uint8_t raw,int pending) {
    NativeCommand command={.game=game};
    const CommandSelectionHooks selection={selection_value,&command};
    const CommandPublicationHooks publication={0};
    const FlightCommandHooks flight={flight_key_child,flight_arguments,&command.flight};
    const ViewCommandHooks view={view_child,NULL,&command};
    const IndexedCommandHooks indexed={mode_changed,NULL,game};
    const ContextCommandHooks actions={context_child,NULL,game};
    const CommandDispatchHooks hooks={&selection,&publication,&flight,&view,&indexed,&actions,
        carried_selection,dispatch_child,NULL,&command,prepare_action};
    const CommandDispatchResult result=pending?dispatch_pending_command_result(&hooks):
        dispatch_keyboard_command_result(raw,&hooks);
    if(result.publication_ran && result.publication.queued) {
        game->completed_input_return=(NativeInputReturn){(uint8_t)result.publication.translated_index,
            NATIVE_INPUT_RETURN_COMMAND_QUEUE};
    } else if(result.action==COMMAND_PENDING_EMPTY || result.action==COMMAND_COUNTER_WAIT ||
              result.action==COMMAND_FINISH_EVENT || result.action==COMMAND_QUEUE_ONLY) {
        /* C1AD72/C1AD70/C1C2B6 and queue-only skips assign no action output.
         * C1AE02/C1AE08's actual masked block load can still supersede prior. */
        if(command.selection_known)
            game->completed_input_return=(NativeInputReturn){(uint8_t)command.flight.carried_event,
                NATIVE_INPUT_RETURN_COMMAND_SELECTION};
    } else game->completed_input_return.owner=NATIVE_INPUT_RETURN_UNKNOWN;
}
void native_menu_dispatch_raw(NativeFrontend *game,uint8_t raw) { dispatch(game,raw,0); }
void native_menu_dispatch_pending(NativeFrontend *game) { dispatch(game,0,1); }
typedef struct { NativeFrontend *game; gaddr field; unsigned offset,width; } MenuContext;
static void observe(void *context,enum MenuTransitionPhase phase,uint32_t value,uint32_t extra,gaddr address) {
    MenuContext *menu=context;
    (void)value;
    if(phase==MENU_FORMAT_FIELD) { menu->field=address; menu->offset=extra>>8; menu->width=extra&255; }
}
static MenuTransitionResult consume(void *context,enum MenuTransitionCall call,uint32_t value) {
    MenuContext *menu=context;
    switch(call) {
    case MENU_PHASE_RESET: case MENU_POSITIVE_RESET: case MENU_NEGATIVE_RESET:
        reset_message_sequence(); break;
    case MENU_POSITIVE_CLEAR: case MENU_NEGATIVE_CLEAR: case MENU_OTHER_CLEAR:
        native_frontend_clear_text(); break;
    case MENU_SUMMARY: {
        const MenuTransitionHooks hooks={consume,observe,menu};
        format_menu_summary(&hooks); break;
    }
    default:
        if(call>=MENU_FIELD_FIRST && call<=MENU_FIELD_LAST)
            print_number(menu->field,(int16_t)menu->offset,value,(int8_t)menu->width);
        else { fprintf(stderr,"native menu child unavailable: %u\n",(unsigned)call); abort(); }
        break;
    }
    /* C24F76 resets the primary value and reloads the pilot-record pointer. */
    return (MenuTransitionResult){0,rd_u32(MODE_TABLE)};
}
static void available_child(void *context,enum MenuColdChild child) {
    (void)context;
    if(child==MENU_COLD_CLEAR_QUEUE) native_frontend_clear_text();
    else { fprintf(stderr,"native mission menu child unavailable: %u\n",(unsigned)child); abort(); }
}
static void table_child(void *context,enum MenuColdChild child) {
    NativeFrontend *game=context;
    switch(child) {
    case MENU_COLD_LOAD: native_frontend_save_log(game); break;
    case MENU_COLD_CLEAR_TABLE: {
        const MenuColdHooks hooks={table_child,NULL,game};
        clear_menu_mode_table(&hooks); break;
    }
    case MENU_COLD_RESET: reset_message_sequence(); break;
    case MENU_COLD_CLEAR_SUMMARY: native_frontend_clear_text(); break;
    case MENU_COLD_SUMMARY: {
        MenuContext menu={game,0,0,0};
        const MenuTransitionHooks hooks={consume,observe,&menu};
        format_menu_summary(&hooks); break;
    }
    default: fprintf(stderr,"native pilot log child unavailable: %u\n",(unsigned)child); abort();
    }
}
void native_menu_tick(NativeFrontend *game) {
    if(game->screen!=NATIVE_MENU && game->screen!=NATIVE_MISSIONS && game->screen!=NATIVE_PILOT_LOG) return;
    if(game->screen==NATIVE_PILOT_LOG) {
        const MenuColdHooks hooks={table_child,NULL,game};
        consume_menu_table_action(&hooks);
        if(rd_u32(STAGE_CALLBACK)==0xc0fbe0) {
            wr_u8(SEQUENCE_PHASE,0); native_frontend_start_menu(game);
        } else if(rd_u32(STAGE_CALLBACK)==0xc114d2) native_frontend_enlist(game);
        return;
    }
    MenuContext context={game,0,0,0};
    const MenuTransitionHooks hooks={consume,NULL,&context};
    follow_top_level_menu(&hooks);
    uint32_t next=rd_u32(STAGE_CALLBACK);
    if(next==0xc0fbe0) { native_frontend_start_menu(game); return; }
    if(next==0xc1017e) {
        const MenuColdHooks available={available_child,NULL,game};
        queue_available_menu_modes(&available);
        game->screen=NATIVE_MISSIONS; game->screen_ticks=0;
    } else if(next==0xc0fe36) { game->screen=NATIVE_PILOT_LOG; game->screen_ticks=0; }
    else if(next==0xc0fece) {
        game->screen=NATIVE_MODE_INTRO; game->screen_ticks=0;
        game->scene_selected=0;
    }
}
