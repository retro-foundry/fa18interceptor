/* C0FE36/C16406: pilot-log actions, independent of scene placement. */
#include "menu_cold.h"
#include "globals.h"
#include <stdlib.h>
static void observe(const MenuColdHooks *h,enum MenuColdPhase phase,uint32_t value,gaddr address) {
    if(h && h->observe) h->observe(h->context,phase,value,address);
}
static void consume(const MenuColdHooks *h,enum MenuColdChild child) {
    if(!h || !h->consume) abort();
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
