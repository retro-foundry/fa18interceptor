#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_input_display_setup.h"
#include "input_display_setup.h"
#include "globals.h"
static void full(unsigned reg,uint32_t v) { D(reg)=v; flags_logic_l(v); }
static void push(uint32_t v) { m68ki_push_32(v); flags_logic_l(v); }
static void pair(uint32_t first,uint32_t second) { full(0,second); push(D(0)); full(0,first); push(D(0)); }
static void text_file(uint32_t filename,uint32_t slot) { full(0,slot); push(D(0)); m68ki_push_32(filename); }
static void palette(uint32_t words) { full(0,32); push(D(0)); push(words); m68ki_push_32(0xc1822a); }
static int32_t consume(void *context,enum InputDisplayChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc53ce8,0xc16d66},{0xc50de8,0xc16d7c},{0xc53de8,0xc16d8a},{0xc53d90,0xc16da2},
        {0xc50de8,0xc16dae},{0xc53c44,0xc16dc8},{0xc53e00,0xc16de0},{0xc53d90,0xc16dee},
        {0xc50de8,0xc16dfa},{0xc16ff4,0xc16e0e},{0xc53e00,0xc16e20},{0xc53d90,0xc16e2e},
        {0xc50de8,0xc16e3a},{0xc17066,0xc16e40},{0xc53e00,0xc16e50},{0xc53d90,0xc16e5e},
        {0xc50de8,0xc16e6a},{0xc53c8c,0xc16ea8},
        {0xc53c8c,0xc17036},{0xc53c1c,0xc17044},{0xc53c08,0xc17052},{0xc53c78,0xc170ac},
        {0xc5058e,0xc17892},{0xc5058e,0xc178b0},{0xc50614,0xc178d0},
        {0xc5046c,0xc17922},{0xc50a58,0xc1793a},{0xc5046c,0xc17960},{0xc50b36,0xc17978},
        {0xc5058e,0xc179a6},{0xc50614,0xc179ca},{0xc5058e,0xc179fe},{0xc50614,0xc17a2e},
        {0xc5058e,0xc17a58},{0xc50614,0xc17a6a},{0xc5058e,0xc17aa6},{0xc50614,0xc17aba},
        {0xc53f88,0xc1613c},{0xc53f30,0xc1617c},{0xc53f88,0xc16194},{0xc53f44,0xc1619c},
        {0xc53ec0,0xc161be},{0xc53f88,0xc161ce},{0xc53f88,0xc161dc},
        {0xc53ec0,0xc161f4},{0xc53f88,0xc16204},{0xc53f88,0xc16212},
        {0xc53f88,0xc16254},{0xc53ec0,0xc1626c}
    };
    unsigned arguments=4; int32_t result; (void)context;
    switch(child) {
    case IDS_CREATE_PORT: full(0,0); push(D(0)); push(D(0)); arguments=8; break;
    case IDS_PORT_FAILURE: full(0,0xffffffffu); push(D(0)); break;
    case IDS_REQUEST_FAILURE: full(0,0xfffffffeu); push(D(0)); break;
    case IDS_OPEN_FAILURE: full(0,0xfffffffdu); push(D(0)); break;
    case IDS_TYPE_FAILURE: case IDS_TRIGGER_FAILURE: full(0,0xfffffffcu); push(D(0)); break;
    case IDS_CREATE_REQUEST: case IDS_DELETE_FAILED_PORT: case IDS_DELETE_OPEN_PORT:
    case IDS_DELETE_TYPE_PORT: case IDS_DELETE_TRIGGER_PORT: case IDS_WAIT_CONTROLLER_TYPE: case IDS_GET_CONTROLLER_REPLY:
        push(rd_u32(0xc1abec)); break;
    case IDS_DELETE_OPEN_REQUEST: case IDS_DELETE_TYPE_REQUEST: case IDS_DELETE_TRIGGER_REQUEST: case IDS_SEND_CONTROLLER_TYPE:
        push(rd_u32(0xc1abce)); break;
    case IDS_OPEN_GAMEPORT: push(0); push(rd_u32(0xc1abce)); full(0,1); push(D(0)); m68ki_push_32(0xc0819a); arguments=16; break;
    case IDS_CONTROLLER_TYPE: full(0,3); push(D(0)); break;
    case IDS_TRIGGER: case IDS_WAIT_BLIT: arguments=0; break;
    case IDS_READ_GAMEPORT: case IDS_SEND_TRIGGER: push(A(child==IDS_SEND_TRIGGER?1:0)); break;
    case IDS_LOAD_TEXT_2: text_file(0xc0823b,2); arguments=8; break;
    case IDS_LOAD_TEXT_2_ALTERNATE: text_file(0xc0824c,2); arguments=8; break;
    case IDS_DUPLICATE_TEXT_2: pair(2,3); arguments=8; break;
    case IDS_ALLOCATE_TEXT_4: pair(32,4); arguments=8; break;
    case IDS_ALLOCATE_TEXT_6: full(0,6); push(D(0)); push(0x800); arguments=8; break;
    case IDS_CLEAR_TEXT_4: case IDS_CLEAR_TEXT_6: push(rd_u32(A(0)+4)); push(rd_u32(A(0))); arguments=8; break;
    case IDS_LOAD_TEXT_5: text_file(0xc0825e,5); arguments=8; break;
    case IDS_DUPLICATE_TEXT_6: pair(6,10); arguments=8; break;
    case IDS_LOAD_TEXT_11: text_file(0xc0826f,11); arguments=8; break;
    case IDS_DUPLICATE_TEXT_2_TO_12: pair(2,12); arguments=8; break;
    case IDS_LOAD_TEXT_0: push(0); m68ki_push_32(0xc08280); arguments=8; break;
    case IDS_DUPLICATE_TEXT_0: full(0,1); push(D(0)); push(0); arguments=8; break;
    case IDS_LOAD_TEXT_8: text_file(0xc08291,8); arguments=8; break;
    case IDS_DUPLICATE_TEXT_8: pair(8,9); arguments=8; break;
    case IDS_LOAD_VIEW: m68ki_push_32(0xc18218); break;
    case IDS_LOAD_STATIC_PALETTE: palette(0xc084d0); arguments=12; break;
    case IDS_LOAD_DYNAMIC_PALETTE: case IDS_LOAD_CLEAR_PALETTE: palette(rd_u32(0xc45660)); arguments=12; break;
    default: m68ki_push_32(0xc1822a); break;
    }
    result=glue_complete_child(sites[child].entry,sites[child].ret); A(7)+=arguments; return result;
}
static void outputs(void *context,enum InputDisplayPhase p,uint32_t value,uint32_t other) {
    uint32_t temporary; (void)context;
    switch(p) {
    case IDS_D0_BYTE: SET_B(D(0),value); flags_logic_b(value); break;
    case IDS_D0_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case IDS_D0_LONG: full(0,value); break;
    case IDS_D1_LONG: full(1,value); break;
    case IDS_A0: A(0)=value; break;
    case IDS_A1: A(1)=value; break;
    case IDS_STORE_BYTE: case IDS_TEST_BYTE: flags_logic_b(value); break;
    case IDS_STORE_WORD: case IDS_TEST_WORD: flags_logic_w(value); break;
    case IDS_STORE_LONG: case IDS_TEST_LONG: flags_logic_l(value); break;
    case IDS_EXT_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case IDS_EXT_LONG: full(0,value); break;
    case IDS_SHIFT_LONG: step_asl_long(&D(0),value); break;
    case IDS_SUB_D1_WORD: step_subtract_word(&D(1),(uint16_t)value); break;
    case IDS_SUB_D0_BYTE: step_subtract_byte(&D(0),(uint8_t)value); break;
    case IDS_ADD_MEMORY_LONG: temporary=value; step_add_long(&temporary,other); break;
    case IDS_AND_D0_LONG: D(0)&=value; flags_logic_l(D(0)); break;
    case IDS_SUB_D0_LONG: step_subtract_long(&D(0),value); break;
    case IDS_OR_D0_LONG: D(0)|=value; flags_logic_l(D(0)); break;
    case IDS_BIT_TEST: FLAG_Z=value&(1u<<other); break;
    }
}
static const InputDisplayHooks hooks={consume,outputs,NULL};
static void link_frame(unsigned size) { m68ki_push_32(A(6)); A(6)=A(7); A(7)-=size; }
static int leave_frame(void) { A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
int glue_C16D4C(void) { link_frame(2); open_gameport_device(A(6),&hooks); return leave_frame(); }
int glue_C16FF4(void) { link_frame(0); set_gameport_controller_type(A(6),&hooks); return leave_frame(); }
int glue_C17066(void) { link_frame(8); configure_gameport_events(A(6),&hooks); return leave_frame(); }
int glue_C1787A(void) { link_frame(2); load_setup_text_resources(A(6),&hooks); return leave_frame(); }
int glue_C1612C(void) { link_frame(2); synchronize_outer_display(A(6),&hooks); return leave_frame(); }
