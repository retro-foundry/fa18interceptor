#include "menu_cold.h"
#include "globals.h"
#include "view.h"
#include <stdlib.h>
static void observe(const MenuColdHooks *h,enum MenuColdPhase phase,uint32_t value,gaddr address) {
    if(h->observe) h->observe(h->context,phase,value,address);
}
static void consume(const MenuColdHooks *h,enum MenuColdChild child) {
    if(!h->consume) abort();
    h->consume(h->context,child);
}
static void byte(const MenuColdHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,MC_BYTE_STORE,value,address);
}
static void callback(const MenuColdHooks *h,gaddr value,int lea) {
    wr_u32(STAGE_CALLBACK,value); observe(h,MC_CALLBACK,value,(gaddr)lea);
}
void consume_menu_table_action(const MenuColdHooks *h) {
    uint8_t action=rd_u8(MENU_TABLE_ACTION),changed;
    uint16_t value;
    observe(h,MC_BYTE_TEST,action,0);
    if(action) {
        action=rd_u8(MENU_TABLE_ACTION); observe(h,MC_BYTE_D0_SUBTRACT,action,1);
        if(action==1) {
            changed=rd_u8(MODE_TABLE_CHANGED); observe(h,MC_BYTE_TEST,changed,0);
            if(changed) {
                value=rd_u16(MENU_TABLE_STATUS); observe(h,MC_WORD_D0,value,0);
                if(!value) {
                    consume(h,MENU_COLD_LOAD); byte(h,COMMAND_EVENT_COUNTER,0xff);
                    value=rd_u16(rd_u32(MODE_TABLE)+4);
                    observe(h,MC_WORD_D0,value,rd_u32(MODE_TABLE));
                    observe(h,MC_WORD_COMPARE,value,1);
                    if(value<=1) callback(h,0xc114d2,0);
                }
            }
        } else {
            action=rd_u8(MENU_TABLE_ACTION); observe(h,MC_BYTE_D0_SUBTRACT,action,2);
            if(action==2) {
                consume(h,MENU_COLD_CLEAR_TABLE); consume(h,MENU_COLD_RESET);
                consume(h,MENU_COLD_CLEAR_SUMMARY); consume(h,MENU_COLD_SUMMARY);
                wr_u16(MESSAGE_QUEUE,0x57); observe(h,MC_WORD_STORE,0x57,MESSAGE_QUEUE);
            }
        }
        byte(h,MENU_TABLE_ACTION,0);
    } else {
        action=rd_u8(SEQUENCE_PHASE); observe(h,MC_BYTE_TEST,action,0);
        if(action) callback(h,0xc0fbe0,1);
    }
    byte(h,MODE_SELECT,0);
}
void queue_available_menu_modes(const MenuColdHooks *h) {
    gaddr cursor=MESSAGE_QUEUE,code_cursor=MENU_AVAILABLE_CODES;
    uint8_t mode;
    consume(h,MENU_COLD_CLEAR_QUEUE);
    wr_u16(cursor,0x40); observe(h,MC_QUEUE_START,0x40,cursor); cursor+=2;
    observe(h,MC_QUEUE_LOCAL,cursor,code_cursor);
    for(mode=3;mode<9;++mode) {
        uint8_t enabled;
        observe(h,MC_QUEUE_MODE,mode,rd_u32(MODE_TABLE)+0x12u+mode);
        enabled=rd_u8(rd_u32(MODE_TABLE)+0x12u+mode);
        observe(h,MC_QUEUE_ENABLED,enabled,0);
        if(enabled) {
            uint16_t code=rd_u16(code_cursor);
            wr_u16(cursor,code); observe(h,MC_QUEUE_CODE,code,code_cursor);
            code_cursor+=2; cursor+=2; observe(h,MC_QUEUE_NEXT,cursor,code_cursor);
        }
        observe(h,MC_QUEUE_ADVANCE,(uint8_t)(mode+1u),0);
    }
    observe(h,MC_QUEUE_END,9,cursor);
    wr_u16(cursor,0x806e); observe(h,MC_WORD_STORE,0x806e,cursor);
    cursor+=2; observe(h,MC_QUEUE_NEXT,cursor,0);
    wr_u16(cursor,0); observe(h,MC_WORD_STORE,0,cursor);
    callback(h,0xc0fcb4,1);
}
void leave_menu_after_countdown(const MenuColdHooks *h,int set_context) {
    uint16_t countdown;
    if(set_context) byte(h,CONTEXT_GATE,1);
    countdown=rd_u16(POST_INPUT_COUNTDOWN); observe(h,MC_WORD_D0,countdown,0);
    if((int16_t)countdown<0) {
        observe(h,MC_WORD_D0,0,1);
        byte(h,POST_INPUT_AUX,0); byte(h,VIEWPORT_TARGET,15); byte(h,VIEWPORT_MODE,0);
        callback(h,set_context?0xc10418:0xc1029e,1);
    }
}
void set_menu_position_preset(const MenuColdHooks *h,int alternate) {
    uint32_t x=alternate?0x0f248000u:0x10800000u;
    uint32_t y=alternate?0x5000u:0x03000000u;
    uint32_t z=alternate?0x0fccf000u:0x10c00000u;
    set_observer_position((int32_t)x,(int32_t)y,(int32_t)z);
    observe(h,MC_POSITION_PRESET,(uint32_t)alternate,0);
}
void refresh_menu_cockpit(const MenuColdHooks *h) {
    uint16_t flags;
    consume(h,MENU_COLD_POSITION);
    flags=rd_u16(COCKPIT_FLAGS)|0x800u;
    observe(h,MC_COCKPIT,flags,0); wr_u16(COCKPIT_FLAGS,flags);
    consume(h,MENU_COLD_UPDATE);
}
void clear_menu_mode_table(const MenuColdHooks *h) {
    gaddr cursor=rd_u32(MODE_TABLE);
    unsigned count;
    observe(h,MC_TABLE_START,cursor,0);
    for(count=0;count<39;++count) {
        wr_u16(cursor,0); observe(h,MC_TABLE_WORD,0,cursor); cursor+=2;
        observe(h,MC_TABLE_NEXT,count+1u,cursor);
    }
    observe(h,MC_TABLE_END,39,0); byte(h,MODE_TABLE_CHANGED,1);
}
