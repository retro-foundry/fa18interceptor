/* C1017E: selectable mission queue; separated from scene placement. */
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
    wr_u32(STAGE_CALLBACK,0xc0fcb4); observe(h,MC_CALLBACK,0xc0fcb4,1);
}
