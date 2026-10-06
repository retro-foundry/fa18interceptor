/* Native menu composition: C1AD74 -> C1BC72/C1BD78 -> C0FCB4.
 * C1017E publishes available missions; C24E8A formats the pilot log.
 * No source instruction, CPU adapter or device register is executed. */
#include "menu.h"
#include "../globals.h"
#include "../command_selection.h"
#include "../indexed_commands.h"
#include "../menu_transition.h"
#include "../menu_cold.h"
#include "../numbers.h"
#include "../stages.h"
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
    /* C3318E -> C3319A: the selected positive TONE_MUTE path returns. */
    if(rd_s8(TONE_MUTE)<=0) { fputs("native menu audio unavailable\n",stderr); abort(); }
    return 0;
}
unsigned native_menu_selected_mode(const NativeFrontend *game) {
    (void)game; return rd_u8(MODE_SELECT);
}
void native_menu_key(NativeFrontend *game,int key,int down) {
    unsigned raw=0xff;
    if(key>='1' && key<='9') raw=(unsigned)(key-'0');
    else if(key=='0') raw=10;
    else if(key>=282 && key<=291) raw=(unsigned)(key-282+0x50); /* E9K SDL1 F1-F10 */
    else if(key==27) raw=0x45;
    else if(key=='\r') raw=0x44;
    else if(key=='\b') raw=0x41;
    else if(key==' ') raw=0x40;
    else if(key>=32 && key<127) {
        for(unsigned i=0;i<0x40;++i) if(rd_u8(0xc331ceu+i)==(uint8_t)key) { raw=i; break; }
    }
    if(raw==0xff) return;
    if(!down) raw|=0x80;
    CommandRequest request=select_keyboard_command(raw,NULL);
    if(is_indexed_command(request.action)) {
        const IndexedCommandHooks hooks={mode_changed,NULL,game};
        execute_indexed_command(&request,0,&hooks);
    } else if(request.action==COMMAND_SIGN_INPUT) {
        /* C1C224, same store as execute_flight_command. */
        wr_u8(SEQUENCE_PHASE,request.modifier?0xff:1);
    }
}
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
        /* TODO(port): C0FECE's delayed flight scene/root/view owners. The
         * source-selected transition message is rendered; flight is absent. */
    }
}
