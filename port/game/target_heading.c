/* Complete selected-record heading and original decimal rounding, C25070. */
#include "target_heading.h"
#include "globals.h"
#include "matrix.h"
#include "numbers.h"
#include "tracking.h"

static void observe(const MenuHeadingHooks *h,enum MenuHeadingPhase phase,uint32_t value,gaddr address) {
    if(h && h->observe) h->observe(h->context,phase,value,address);
}
static uint8_t decimal_sum(uint8_t source,uint8_t value,unsigned *carry) {
    unsigned sum=(source&15u)+(value&15u)+*carry;
    if(sum>9) sum+=6;
    sum+=(source&0xf0u)+(value&0xf0u); *carry=sum>0x99u;
    if(*carry) sum-=0xa0u;
    return (uint8_t)sum;
}
int refresh_menu_heading(const MenuHeadingHooks *h) {
    gaddr item=rd_u32(POST_INPUT_RECORD_LIST),record=0,text=POST_INPUT_HEADING_TEXT;
    gaddr digit_address=DISPLAY_VALUE_BCD+2,step=POSTFLIGHT_BCD_STEP;
    uint8_t type,digit,source,value; uint16_t index,heading;
    uint32_t point[3],player_x,player_z,dividend,quotient,packed;
    unsigned carry=0,i;
    observe(h,MH_LIST_START,item,0);
    for(;;) {
        uint16_t state=rd_u16(item); observe(h,MH_LIST_TEST,state,0);
        if((int16_t)state<0) { observe(h,MH_NOT_FOUND,0xffffffffu,0); return -1; }
        index=rd_u16(item+4); item+=10; observe(h,MH_RECORD_OFFSET,index,0);
        record=CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)(uint16_t)(index<<9);
        type=rd_u8(record+0x62); observe(h,MH_TYPE,type,0); observe(h,MH_TYPE_COMPARE,type,0x15);
        if(type==0x15) continue;
        type&=0xf0u; observe(h,MH_TYPE_MASK,type,0); observe(h,MH_TYPE_COMPARE,type,0x10);
        if(type!=0x10) continue;
        value=rd_u8(record+1); observe(h,MH_RECORD_FLAG,value,0);
        if(value&0x40u) break;
    }
    observe(h,MH_RECORD_SELECTED,record,0); observe(h,MH_WORLD_INPUT,0,0);
    if(h && h->world) h->world(h->context,record,point);
    else {
        int32_t native_point[3]; local_to_world(record,record+RECORD_INVERSE,0,0,0x1f4,native_point);
        for(i=0;i<3;++i) point[i]=(uint32_t)native_point[i];
    }
    player_x=rd_u32(CONTROL_RECORDS+0x14); player_z=rd_u32(CONTROL_RECORDS+0x1c);
    observe(h,MH_DIRECTION,player_x,player_z);
    if(h && h->track) h->track(h->context,(uint32_t)((int32_t)(point[0]-player_x)>>8),(uint32_t)((int32_t)(point[2]-player_z)>>8));
    else {
        int32_t elevation=0,azimuth=0;
        track_direction(&elevation,&azimuth,(int32_t)(point[0]-player_x)>>8,0,(int32_t)(point[2]-player_z)>>8,-1);
    }
    heading=rd_u16(TRACKED_HEADING); observe(h,MH_DIVIDE,heading,0);
    dividend=(uint32_t)(int32_t)(int16_t)heading; quotient=dividend/0x50u;
    wr_u32(DISPLAY_VALUE,(uint32_t)(int32_t)(int16_t)(quotient<=0xffffu?quotient:dividend));
    if(h && h->pack) h->pack(h->context); else pack_display_value();
    packed=rd_u32(DISPLAY_VALUE_BCD); observe(h,MH_PACKED,packed,0);
    value=(uint8_t)(rd_u8(DISPLAY_VALUE_BCD+3)&0xf0u); wr_u8(DISPLAY_VALUE_BCD+3,value);
    observe(h,MH_NIBBLE_CLEAR,value,0);
    digit=(uint8_t)(packed&15u); observe(h,MH_DIGIT_MASK,digit,0); observe(h,MH_DIGIT_COMPARE,digit,5);
    if(digit>=5) {
        digit_address+=2; wr_u32(POSTFLIGHT_BCD_TICK,10); observe(h,MH_ROUND_BEGIN,digit,0);
        for(i=0;i<2;++i) {
            source=rd_u8(--step); value=rd_u8(--digit_address);
            observe(h,MH_DECIMAL,((uint32_t)source<<8)|value,0);
            wr_u8(digit_address,decimal_sum(source,value,&carry));
        }
        packed=rd_u32(DISPLAY_VALUE_BCD); observe(h,MH_ROUND_COMPARE,packed,0x360);
        if((int32_t)packed>=0x360) { wr_u16(DISPLAY_VALUE_BCD+2,0); observe(h,MH_WORD_CLEAR,0,0); }
    }
    value=rd_u8(digit_address++); observe(h,MH_DIGIT_LOAD,value,1);
    digit=(uint8_t)(value&15u); observe(h,MH_DIGIT_MASK,digit,0);
    digit=(uint8_t)(digit+0x30); observe(h,MH_ASCII,0x30,0); wr_u8(text++,digit); observe(h,MH_CHARACTER,digit,0);
    value=rd_u8(digit_address); observe(h,MH_DIGIT_LOAD,value,0);
    value>>=4; observe(h,MH_DIGIT_SHIFT,4,0); digit=(uint8_t)(value&15u); observe(h,MH_DIGIT_MASK,digit,0);
    digit=(uint8_t)(digit+0x30); observe(h,MH_ASCII,0x30,0); wr_u8(text++,digit); observe(h,MH_CHARACTER,digit,0);
    value=rd_u8(digit_address); observe(h,MH_DIGIT_LOAD,value,0);
    digit=(uint8_t)(value&15u); observe(h,MH_DIGIT_MASK,digit,0);
    digit=(uint8_t)(digit+0x30); observe(h,MH_ASCII,0x30,0); wr_u8(text,digit); observe(h,MH_CHARACTER,digit,0);
    observe(h,MH_COMPLETE,0,0); return 0;
}
int refresh_post_input_heading(void) { return refresh_menu_heading(NULL); }
