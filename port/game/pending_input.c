#include "pending_input.h"
#include "globals.h"
#include <stdlib.h>
static void observe(const PendingInputHooks *h,enum PendingInputPhase phase,
                    uint32_t value,uint32_t previous) {
    PendingInputEvent event={phase,value,previous};
    if(h->observe) h->observe(h->context,&event);
}
void process_pending_key_events(const PendingInputHooks *h) {
    uint16_t value,previous; uint8_t mode;
    if(!h || !h->consume) abort();
    observe(h,PENDING_BEGIN,0,0);
    h->consume(h->context,PENDING_PREPARE,0);
    value=(uint16_t)h->consume(h->context,PENDING_BUTTONS,0);
    previous=rd_u16(INPUT_STATE_WORD); observe(h,PENDING_INPUT_WORD,value,previous);
    if(value!=previous) {
        wr_u16(INPUT_STATE_WORD,value); wr_u16(INPUT_STATE_MIRROR,value);
        observe(h,PENDING_CHANGED_WORD,value,0); h->consume(h->context,PENDING_CHANGED,0);
    }
    mode=rd_u8(RECORDER_MODE); observe(h,PENDING_MODE,mode,0);
    if(mode==1 || mode==2) {
        for(;;) {
            value=rd_u16(RECORD_WORD_A); observe(h,PENDING_WORD_TEST,value,0);
            if(!value) break;
            h->consume(h->context,PENDING_WAIT_ONE,0);
        }
    } else {
        observe(h,PENDING_MODE_THREE,mode,0);
        if(mode==3) for(;;) {
            value=rd_u16(RECORD_WORD_A); previous=rd_u16(RECORD_WORD_B);
            observe(h,PENDING_WORD_PAIR,value,previous);
            if(!(value|previous)) break;
            h->consume(h->context,PENDING_WAIT_BOTH,0);
        }
    }
    for(;;) {
        uint8_t key=(uint8_t)h->consume(h->context,PENDING_POLL_KEY,0);
        observe(h,PENDING_KEY,key,0);
        if(key==0xff) break;
        observe(h,PENDING_ARGUMENT,key,0); h->consume(h->context,PENDING_DISPATCH_KEY,key);
        observe(h,PENDING_DROP_ARGUMENT,0,0);
    }
    observe(h,PENDING_MODE_END,mode,0);
    if(!mode) {
        value=rd_u16(PENDING_COMMAND_WORD_A); wr_u16(RECORD_WORD_A,value);
        observe(h,PENDING_COPY_WORD,value,0);
        value=rd_u16(PENDING_COMMAND_WORD_B); wr_u16(RECORD_WORD_B,value);
        observe(h,PENDING_COPY_WORD,value,0);
        wr_u16(PENDING_COMMAND_WORD_A,0); wr_u16(PENDING_COMMAND_WORD_B,0);
        observe(h,PENDING_CLEAR_WORDS,0,0);
    } else {
        observe(h,PENDING_MODE_ONE,mode,0);
        if(mode==1) {
            value=rd_u16(PENDING_COMMAND_WORD_B); wr_u16(RECORD_WORD_B,value);
            observe(h,PENDING_COPY_WORD,value,0);
            wr_u16(PENDING_COMMAND_WORD_B,0); observe(h,PENDING_CLEAR_SECOND,0,0);
        }
    }
    observe(h,PENDING_END,0,0);
}
