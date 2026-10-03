#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "ports_glue.h"
#include "menu_cold.h"
#include "globals.h"
static void consume(void *context,enum MenuColdChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc1643a,0xc0fe60},{0xc16406,0xc0fe94},{0xc11312,0xc0fe98},
        {0xc2fd22,0xc0fe9e},{0xc24e8a,0xc0fea4},{0xc2fd22,0xc10188},
        {0xc09266,0xc10b96},{0xc22c80,0xc10bac}
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret);
}
static void outputs(void *context,enum MenuColdPhase phase,uint32_t value,gaddr address) {
    (void)context;
    switch(phase) {
    case MC_BYTE_TEST: case MC_BYTE_STORE: flags_logic_b(value); break;
    case MC_WORD_STORE: flags_logic_w(value); break;
    case MC_BYTE_D0_SUBTRACT: SET_B(D(0),value); step_subtract_byte(&D(0),(uint8_t)address); break;
    case MC_WORD_D0:
        if(address==1) { D(0)=0; flags_logic_l(0); }
        else { if(address) A(0)=address; SET_W(D(0),value); flags_logic_w(value); } break;
    case MC_WORD_COMPARE: step_compare_word((uint16_t)address,(uint16_t)value); break;
    case MC_CALLBACK: if(address) A(0)=value; flags_logic_l(value); break;
    case MC_QUEUE_START: A(0)=address+2; flags_logic_w(value); break;
    case MC_QUEUE_LOCAL:
        wr_u32(A(6)-8,address); flags_logic_l(address);
        wr_u8(A(6)-9,3); flags_logic_b(3); wr_u32(A(6)-4,value); flags_logic_l(value); break;
    case MC_QUEUE_MODE:
        SET_B(D(0),value); step_compare_byte(9,(uint8_t)D(0));
        SET_W(D(0),(uint16_t)(int16_t)(int8_t)D(0)); D(0)=(uint32_t)(int32_t)(int16_t)D(0);
        A(0)=address; break;
    case MC_QUEUE_ENABLED: flags_logic_b(value); break;
    case MC_QUEUE_CODE:
        A(0)=address; A(1)=rd_u32(A(6)-4); flags_logic_w(value); break;
    case MC_QUEUE_NEXT:
        if(address) { uint32_t cursor=rd_u32(A(6)-8); step_add_long(&cursor,2); wr_u32(A(6)-8,cursor); }
        { uint32_t cursor=rd_u32(A(6)-4); step_add_long(&cursor,2); wr_u32(A(6)-4,cursor); }
        if(!address) A(0)=value; break;
    case MC_QUEUE_ADVANCE:
        { uint32_t counter=rd_u8(A(6)-9); renderer_add_byte(&counter,1); wr_u8(A(6)-9,(uint8_t)counter); } break;
    case MC_QUEUE_END: SET_B(D(0),value); step_compare_byte(9,(uint8_t)D(0)); A(0)=address; break;
    case MC_TABLE_START:
        wr_u32(A(6)-6,value); flags_logic_l(value); wr_u16(A(6)-2,0); flags_logic_w(0); break;
    case MC_TABLE_WORD: step_compare_word(39,rd_u16(A(6)-2)); A(0)=address; flags_logic_w(0); break;
    case MC_TABLE_NEXT:
        { uint32_t cursor=rd_u32(A(6)-6),counter=rd_u16(A(6)-2);
          step_add_long(&cursor,2); wr_u32(A(6)-6,cursor);
          step_add_word(&counter,1); wr_u16(A(6)-2,(uint16_t)counter); } break;
    case MC_TABLE_END: step_compare_word(39,(uint16_t)value); break;
    case MC_COCKPIT: SET_W(D(0),value); flags_logic_w(value); break;
    case MC_POSITION_PRESET:
        D(0)=value?0x0f248000u:0x10800000u; D(1)=value?0x5000u:0x03000000u;
        D(2)=value?0x0fccf000u:0x10c00000u;
        D(0)&=0x3fffffu; D(2)&=0x3fffffu;
        renderer_negate(&D(0),4); renderer_negate(&D(1),4); renderer_negate(&D(2),4);
        flags_logic_l(D(2)); break; /* Final MOVE.L retains X and clears C/V. */
    }
}
static const MenuColdHooks hooks={consume,outputs,NULL};
static void link_frame(unsigned bytes) { m68ki_push_32(A(6)); A(6)=A(7); A(7)-=bytes; }
static int leave_frame(void) { A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
int glue_C0FE36(void) { consume_menu_table_action(&hooks); return glue_return(); }
int glue_C1017E(void) { link_frame(10); queue_available_menu_modes(&hooks); return leave_frame(); }
int glue_C10272(void) { leave_menu_after_countdown(&hooks,0); return glue_return(); }
int glue_C103E4(void) { leave_menu_after_countdown(&hooks,1); return glue_return(); }
int glue_C09120(void) { set_menu_position_preset(&hooks,0); return glue_return(); }
int glue_C09148(void) { set_menu_position_preset(&hooks,1); return glue_return(); }
int glue_C29490(void) { return glue_origin_candidate_preset(ORIGIN_ALTERNATE_PRESET); }
int glue_C2949A(void) { return glue_origin_candidate_preset(ORIGIN_ROOT_PRESET); }
int glue_C10B90(void) { refresh_menu_cockpit(&hooks); return glue_return(); }
int glue_C16406(void) { link_frame(6); clear_menu_mode_table(&hooks); return leave_frame(); }
