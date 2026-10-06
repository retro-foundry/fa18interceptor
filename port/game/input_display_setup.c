/* Complete C16D4C/C16FF4/C17066/C1787A/C1612C game owners. Actual services
 * remain child boundaries; frame locals and globals are reloaded as in source. */
#include "input_display_setup.h"
#include "globals.h"
#include <stdlib.h>
#define GAMEPORT_REQUEST 0xc1abceu
#define GAMEPORT_PORT 0xc1abecu
#define GAMEPORT_BUFFER 0xc1abe8u
#define ACTIVITY_COUNT 0xc45899u
static void observe(const InputDisplayHooks *h,enum InputDisplayPhase p,uint32_t v,uint32_t other) {
    if(h && h->observe) h->observe(h->context,p,v,other);
}
static uint32_t consume(const InputDisplayHooks *h,enum InputDisplayChild child) {
    if(h && h->consume) return (uint32_t)h->consume(h->context,child);
    abort();
}
static void byte(const InputDisplayHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,IDS_STORE_BYTE,v,0); }
static void word(const InputDisplayHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,IDS_STORE_WORD,v,0); }
static void longword(const InputDisplayHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,IDS_STORE_LONG,v,0); }
static void full(const InputDisplayHooks *h,unsigned reg,uint32_t v) { observe(h,reg?IDS_D1_LONG:IDS_D0_LONG,v,0); }
static gaddr address(const InputDisplayHooks *h,unsigned reg,gaddr a) { observe(h,reg?IDS_A1:IDS_A0,a,0); return a; }
static uint8_t byte_d0(const InputDisplayHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,IDS_D0_BYTE,v,0); return v; }
static uint16_t word_d0(const InputDisplayHooks *h,gaddr a) { uint16_t v=rd_u16(a); observe(h,IDS_D0_WORD,v,0); return v; }
static int test_long(const InputDisplayHooks *h,uint32_t v) { observe(h,IDS_TEST_LONG,v,0); return v!=0; }
static int test_byte(const InputDisplayHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,IDS_TEST_BYTE,v,0); return v!=0; }
static void set_bit(const InputDisplayHooks *h,gaddr a,unsigned bit) {
    uint8_t v=rd_u8(a); observe(h,IDS_BIT_TEST,v,bit); wr_u8(a,(uint8_t)(v|(1u<<bit)));
}
void open_gameport_device(gaddr frame,const InputDisplayHooks *h) {
    uint32_t result; gaddr request;
    longword(h,GAMEPORT_BUFFER,0xc1abd2);
    result=consume(h,IDS_CREATE_PORT); longword(h,GAMEPORT_PORT,result);
    if(!test_long(h,result)) consume(h,IDS_PORT_FAILURE);
    result=consume(h,IDS_CREATE_REQUEST); longword(h,GAMEPORT_REQUEST,result);
    if(!test_long(h,result)) { consume(h,IDS_DELETE_FAILED_PORT); consume(h,IDS_REQUEST_FAILURE); }
    result=consume(h,IDS_OPEN_GAMEPORT); word(h,frame-2,(uint16_t)result); observe(h,IDS_TEST_WORD,result,0);
    if((uint16_t)result) { consume(h,IDS_DELETE_OPEN_REQUEST); consume(h,IDS_DELETE_OPEN_PORT); consume(h,IDS_OPEN_FAILURE); }
    longword(h,0xc1abca,0xc1abd2);
    result=consume(h,IDS_CONTROLLER_TYPE);
    if(test_long(h,result)) { consume(h,IDS_DELETE_TYPE_REQUEST); consume(h,IDS_DELETE_TYPE_PORT); consume(h,IDS_TYPE_FAILURE); }
    result=consume(h,IDS_TRIGGER);
    if(test_long(h,result)) { consume(h,IDS_DELETE_TRIGGER_REQUEST); consume(h,IDS_DELETE_TRIGGER_PORT); consume(h,IDS_TRIGGER_FAILURE); }
    request=address(h,0,rd_u32(GAMEPORT_REQUEST)); word(h,request+28,9);
    request=address(h,0,rd_u32(GAMEPORT_REQUEST)); longword(h,request+40,0xc1abd2);
    full(h,0,22); request=address(h,0,rd_u32(GAMEPORT_REQUEST)); longword(h,request+36,22); byte(h,request+30,0);
    request=address(h,0,rd_u32(GAMEPORT_REQUEST)); longword(h,request+36,22); consume(h,IDS_READ_GAMEPORT);
}
void set_gameport_controller_type(gaddr frame,const InputDisplayHooks *h) {
    gaddr request,buffer; uint16_t type; int32_t error;
    request=address(h,0,rd_u32(GAMEPORT_REQUEST)); word(h,request+28,11);
    full(h,0,1); request=address(h,0,rd_u32(GAMEPORT_REQUEST)); longword(h,request+36,1);
    request=address(h,0,rd_u32(GAMEPORT_REQUEST)); longword(h,request+40,rd_u32(GAMEPORT_BUFFER));
    type=word_d0(h,frame+10); buffer=address(h,0,rd_u32(GAMEPORT_BUFFER)); byte(h,buffer,(uint8_t)type);
    consume(h,IDS_SEND_CONTROLLER_TYPE); consume(h,IDS_WAIT_CONTROLLER_TYPE); consume(h,IDS_GET_CONTROLLER_REPLY);
    request=address(h,0,rd_u32(GAMEPORT_REQUEST)); error=(int8_t)byte_d0(h,request+31);
    observe(h,IDS_EXT_WORD,(uint16_t)error,0); observe(h,IDS_EXT_LONG,(uint32_t)error,0);
}
void configure_gameport_events(gaddr frame,const InputDisplayHooks *h) {
    gaddr request,trigger;
    request=address(h,0,rd_u32(GAMEPORT_REQUEST)); word(h,request+28,13);
    full(h,0,8); request=address(h,0,rd_u32(GAMEPORT_REQUEST)); longword(h,request+36,8);
    trigger=address(h,0,frame-8); request=address(h,1,rd_u32(GAMEPORT_REQUEST)); longword(h,request+40,trigger);
    word(h,frame-8,3); word(h,frame-6,0); full(h,0,1); word(h,frame-4,1); word(h,frame-2,1);
    consume(h,IDS_SEND_TRIGGER);
}
static void add_descriptor_offset(const InputDisplayHooks *h,gaddr descriptor,uint32_t amount) {
    uint32_t old=rd_u32(descriptor); observe(h,IDS_ADD_MEMORY_LONG,old,amount); wr_u32(descriptor,old+amount);
}
static void remap_text_offset(const InputDisplayHooks *h,gaddr source,gaddr destination,uint32_t amount) {
    uint32_t offset=rd_u32(source+4); full(h,0,offset);
    offset&=0x7fffffffu; observe(h,IDS_AND_D0_LONG,0x7fffffffu,0);
    offset-=amount; observe(h,IDS_SUB_D0_LONG,amount,0);
    offset|=0x80000000u; observe(h,IDS_OR_D0_LONG,0x80000000u,0); longword(h,destination+4,offset);
}
void load_setup_text_resources(gaddr frame,const InputDisplayHooks *h) {
    gaddr a,b; uint32_t result;
    word(h,frame-2,0); result=consume(h,IDS_LOAD_TEXT_2);
    if(test_long(h,result)) word(h,frame-2,1);
    else { result=consume(h,IDS_LOAD_TEXT_2_ALTERNATE); if(test_long(h,result)) word(h,frame-2,1); }
    observe(h,IDS_TEST_WORD,rd_u16(frame-2),0);
    if(rd_u16(frame-2)) {
        result=consume(h,IDS_DUPLICATE_TEXT_2);
        if(test_long(h,result)) {
            full(h,0,0xffffffffu); a=address(h,0,rd_u32(0xc0a440)); longword(h,a+16,0xffffffffu);
            a=address(h,0,rd_u32(0xc0a444)); longword(h,a+16,0xffffffffu);
            add_descriptor_offset(h,a,0x370); remap_text_offset(h,a,a,0x370); set_bit(h,0xc45b5b,1);
        }
    }
    result=consume(h,IDS_ALLOCATE_TEXT_4);
    if(test_long(h,result)) {
        address(h,0,rd_u32(0xc0a448)); consume(h,IDS_CLEAR_TEXT_4);
        a=address(h,0,rd_u32(0xc0a448)); longword(h,a+48,0xc50b78); full(h,0,0xffffffffu); longword(h,a+16,0xffffffffu);
    }
    result=consume(h,IDS_ALLOCATE_TEXT_6);
    if(test_long(h,result)) {
        address(h,0,rd_u32(0xc0a450)); consume(h,IDS_CLEAR_TEXT_6);
        a=address(h,0,rd_u32(0xc0a450)); full(h,0,0xffffffffu); longword(h,a+16,0xffffffffu);
        longword(h,a+48,0xc50bd8); set_bit(h,0xc45b5a,0);
    }
    result=consume(h,IDS_LOAD_TEXT_5); if(test_long(h,result)) set_bit(h,0xc45b5b,2);
    if(test_long(h,rd_u32(0xc0a450))) {
        result=consume(h,IDS_DUPLICATE_TEXT_6);
        if(test_long(h,result)) { a=address(h,0,rd_u32(0xc0a460)); longword(h,a+48,0xc50c70); longword(h,a+8,0x960000); set_bit(h,0xc45b5b,5); }
    }
    result=consume(h,IDS_LOAD_TEXT_11);
    if(test_long(h,result)) { full(h,0,0xffffffffu); a=address(h,0,rd_u32(0xc0a464)); longword(h,a+16,0xffffffffu); set_bit(h,0xc45b5b,6); }
    if(test_long(h,rd_u32(0xc0a440))) {
        result=consume(h,IDS_DUPLICATE_TEXT_2_TO_12);
        if(test_long(h,result)) { a=address(h,0,rd_u32(0xc0a468)); longword(h,a+48,0xc50c30); set_bit(h,0xc45b5a,1); }
    }
    result=consume(h,IDS_LOAD_TEXT_0);
    if(test_long(h,result)) {
        result=consume(h,IDS_DUPLICATE_TEXT_0);
        if(test_long(h,result)) {
            full(h,0,0xffffffffu); a=address(h,0,rd_u32(0xc0a438)); longword(h,a+16,0xffffffffu);
            a=address(h,0,rd_u32(0xc0a43c)); longword(h,a+16,0xffffffffu); longword(h,a+4,0x800015cc); set_bit(h,0xc45b5b,0);
        }
    }
    result=consume(h,IDS_LOAD_TEXT_8);
    if(test_long(h,result)) {
        result=consume(h,IDS_DUPLICATE_TEXT_8);
        if(test_long(h,result)) {
            a=address(h,0,rd_u32(0xc0a45c)); full(h,0,0xffffffffu); longword(h,a+16,0xffffffffu);
            b=address(h,1,rd_u32(0xc0a458)); longword(h,b+16,0xffffffffu); add_descriptor_offset(h,a,0x792);
            b=address(h,1,rd_u32(0xc0a444)); remap_text_offset(h,b,a,0x792); set_bit(h,0xc45b5b,4);
        }
    }
}
static void publish_outer_pair(gaddr frame,const InputDisplayHooks *h) {
    uint16_t index=word_d0(h,DRAW_PAGE); uint32_t offset; gaddr a;
    word(h,frame-2,index); observe(h,IDS_EXT_LONG,(uint32_t)(int32_t)(int16_t)index,0); observe(h,IDS_SHIFT_LONG,2,0);
    offset=(uint32_t)(int32_t)(int16_t)index<<2; a=address(h,0,offset); a=address(h,0,a+0xc182ba); longword(h,0xc1821c,rd_u32(a));
    index=word_d0(h,frame-2); observe(h,IDS_EXT_LONG,(uint32_t)(int32_t)(int16_t)index,0); observe(h,IDS_SHIFT_LONG,2,0);
    offset=(uint32_t)(int32_t)(int16_t)index<<2; a=address(h,0,offset); a=address(h,0,a+0xc182c2); longword(h,0xc18232,rd_u32(a));
}
int advance_outer_display(gaddr frame,const InputDisplayHooks *h,
    OuterDisplayState *state,int (*await_child)(void *,enum InputDisplayChild)) {
    uint8_t count; uint16_t page,status;
    /* Each continuation boundary follows a complete source child. Nothing
     * before a pending wait, including palette loads, is executed twice. */
#define SERVICE(child,next) do { \
    if(await_child && !await_child(h->context,child)) return 0; \
    consume(h,child); state->phase=next; \
} while(0)
    for(;;) switch(state->phase) {
    case OUTER_WAIT_PUBLICATION: SERVICE(IDS_WAIT_PUBLICATION,OUTER_PUBLISH); break;
    case OUTER_PUBLISH:
        publish_outer_pair(frame,h); consume(h,IDS_LOAD_VIEW);
        state->phase=OUTER_TEST_ACTIVITY; break;
    case OUTER_TEST_ACTIVITY:
        state->phase=test_byte(h,ACTIVITY_COUNT)?OUTER_WAIT_ACTIVITY:OUTER_TEST_CLEAR; break;
    case OUTER_WAIT_ACTIVITY: SERVICE(IDS_WAIT_ACTIVITY,OUTER_WAIT_BLIT); break;
    case OUTER_WAIT_BLIT: SERVICE(IDS_WAIT_BLIT,OUTER_TEST_COUNT); break;
    case OUTER_TEST_COUNT:
        count=byte_d0(h,ACTIVITY_COUNT); observe(h,IDS_TEST_BYTE,count,0);
        state->phase=(int8_t)count>0?OUTER_STATIC_PALETTE:OUTER_SWAP_PAGE; break;
    case OUTER_STATIC_PALETTE: SERVICE(IDS_LOAD_STATIC_PALETTE,OUTER_STATIC_FIRST); break;
    case OUTER_STATIC_FIRST: SERVICE(IDS_WAIT_STATIC_FIRST,OUTER_STATIC_SECOND); break;
    case OUTER_STATIC_SECOND: SERVICE(IDS_WAIT_STATIC_SECOND,OUTER_DYNAMIC_PALETTE); break;
    case OUTER_DYNAMIC_PALETTE: SERVICE(IDS_LOAD_DYNAMIC_PALETTE,OUTER_DYNAMIC_FIRST); break;
    case OUTER_DYNAMIC_FIRST: SERVICE(IDS_WAIT_DYNAMIC_FIRST,OUTER_DYNAMIC_SECOND); break;
    case OUTER_DYNAMIC_SECOND: SERVICE(IDS_WAIT_DYNAMIC_SECOND,OUTER_DECREMENT_ACTIVITY); break;
    case OUTER_DECREMENT_ACTIVITY:
            count=byte_d0(h,ACTIVITY_COUNT); observe(h,IDS_SUB_D0_BYTE,1,0); byte(h,ACTIVITY_COUNT,(uint8_t)(count-1));
        state->phase=OUTER_TEST_COUNT; break;
    case OUTER_TEST_CLEAR:
        state->phase=OUTER_SWAP_PAGE;
        if(!test_byte(h,TABLE_CLEAR_MODE)) break;
        count=byte_d0(h,TABLE_CLEAR_MODE); observe(h,IDS_SUB_D0_BYTE,1,0); byte(h,TABLE_CLEAR_MODE,(uint8_t)(count-1));
        status=word_d0(h,0xc458d2); observe(h,IDS_BIT_TEST,status,8);
        if(!(status&0x100)) state->phase=OUTER_CLEAR_WAIT;
        break;
    case OUTER_CLEAR_WAIT: SERVICE(IDS_WAIT_CLEAR_PALETTE,OUTER_CLEAR_PALETTE); break;
    case OUTER_CLEAR_PALETTE: SERVICE(IDS_LOAD_CLEAR_PALETTE,OUTER_SWAP_PAGE); break;
    case OUTER_SWAP_PAGE:
        page=word_d0(h,DRAW_PAGE); full(h,1,1); observe(h,IDS_SUB_D1_WORD,page,0); word(h,DRAW_PAGE,(uint16_t)(1-page));
        state->phase=OUTER_COMPLETE; break;
    case OUTER_COMPLETE: return 1;
    default: abort();
    }
#undef SERVICE
}
void synchronize_outer_display(gaddr frame,const InputDisplayHooks *h) {
    OuterDisplayState state={0};
    (void)advance_outer_display(frame,h,&state,NULL);
}
