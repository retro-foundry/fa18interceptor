#include "command_publication.h"
#include "globals.h"

static void observe(const CommandPublicationHooks *h,enum CommandPublicationPhase phase,
                    uint32_t value,gaddr address) {
    if(h && h->observe) h->observe(h->context,phase,value,address);
}
uint8_t publish_command_event(uint8_t raw,const CommandPublicationHooks *h) {
    uint8_t taken=rd_u8(KEY_TAKEN),result=raw,count;
    observe(h,COMMAND_PUBLICATION_TAKEN,taken,0);
    if(!taken) {
        observe(h,COMMAND_PUBLICATION_RELEASE,raw&0x80u,0);
        if(!(raw&0x80u)) {
            int8_t index;
            wr_u8(KEY_TAKEN,1); observe(h,COMMAND_PUBLICATION_CLAIM,1,0);
            count=rd_u8(KEY_COUNT); observe(h,COMMAND_PUBLICATION_COUNT,count,0);
            if((int8_t)count<10) {
                index=rd_s8(KEY_WRITE);
                observe(h,COMMAND_PUBLICATION_WRITE_INDEX,(uint8_t)index,0);
                if(index>=10) {
                    index=0; observe(h,COMMAND_PUBLICATION_RESET_INDEX,0,0);
                }
                wr_u8(KEY_RAW+(gaddr)(int32_t)index,raw);
                observe(h,COMMAND_PUBLICATION_RAW,raw,KEY_RAW+(gaddr)(int32_t)index);
                result=rd_u8(KEY_TABLE+raw);
                observe(h,COMMAND_PUBLICATION_TRANSLATED,result,0);
                observe(h,COMMAND_PUBLICATION_ADVANCE_INDEX,(uint8_t)index,0);
                wr_u8(KEY_WRITE,(uint8_t)(index+1));
                /* The raw store can alias queue globals for a negative index. */
                count=rd_u8(KEY_COUNT);
                wr_u8(KEY_COUNT,(uint8_t)(count+1));
                observe(h,COMMAND_PUBLICATION_ADVANCE_COUNT,count,0);
                index=rd_s8(KEY_TRANSLATED_WRITE);
                observe(h,COMMAND_PUBLICATION_TRANSLATED_INDEX,(uint8_t)index,0);
                wr_u8(KEY_TRANSLATED+(gaddr)(int32_t)index,result);
                observe(h,COMMAND_PUBLICATION_TRANSLATED_WRITE,result,0);
            }
        }
    }
    wr_u8(KEY_STATE,0); observe(h,COMMAND_PUBLICATION_CLEAR,0,0);
    wr_u8(KEY_STATE+1,0); observe(h,COMMAND_PUBLICATION_CLEAR,0,0);
    wr_u8(KEY_STATE+2,0); observe(h,COMMAND_PUBLICATION_CLEAR,0,0);
    return result;
}
