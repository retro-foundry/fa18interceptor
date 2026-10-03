#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_input_device_callbacks.h"
#include "input_device_callbacks.h"
#include "globals.h"
static void full(unsigned reg,uint32_t v) { D(reg)=v; flags_logic_l(v); }
static void push(uint32_t v) { m68ki_push_32(v); flags_logic_l(v); }
static void palette_arguments(uint32_t palette) { full(0,16); push(D(0)); push(palette); m68ki_push_32(0xc1822a); }
static int32_t consume(void *context,enum InputDeviceChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc53ec0,0xc17360},{0xc53ec0,0xc173ae},{0xc53ec0,0xc17442},
        {0xc24fe8,0xc1744c},{0xc53b00,0xc17488},{0xc53b18,0xc1749c},
        {0xc53bb8,0xc174c6},{0xc53b74,0xc174d8},{0xc53cc8,0xc174ee},
        {0xc53c44,0xc16cf4},{0xc53c44,0xc16ba4},{0xc174a0,0xc16bba},
        {0xc53c8c,0xc16bee},{0xc17456,0xc17158}
    };
    unsigned arguments=0; int32_t result; (void)context;
    switch(child) {
    case IDC_PALETTE_FIRST: arguments=12; break;
    case IDC_PALETTE_SECOND: palette_arguments(rd_u32(A(6)-16)); arguments=12; break;
    case IDC_PALETTE_STABLE: palette_arguments(rd_u32(LONG_TABLE)); arguments=12; break;
    case IDC_ADD_SERVER: case IDC_REMOVE_SERVER:
        m68ki_push_32(0xc1abf0); full(0,5); push(D(0)); arguments=8; break;
    case IDC_ALLOCATE_SIGNAL: full(0,0xffffffffu); push(D(0)); arguments=4; break;
    case IDC_FIND_TASK: push(0); arguments=4; break;
    case IDC_SET_PORT_LIST: push(A(0)); arguments=4; break;
    case IDC_OPEN_TIMER:
        full(0,0); push(D(0)); m68ki_push_32(0xc1ab84); push(D(0)); m68ki_push_32(0xc08188); arguments=16; break;
    case IDC_OPEN_INPUT:
        full(0,0); push(D(0)); push(rd_u32(0xc08134)); push(D(0)); m68ki_push_32(0xc08160); arguments=16; break;
    case IDC_PREPARE_INPUT_PORT: case IDC_SEND_INPUT_REQUEST: arguments=4; break;
    default: break;
    }
    result=glue_complete_child(sites[child].entry,sites[child].ret); A(7)+=arguments; return result;
}
static void outputs(void *context,enum InputDevicePhase p,uint32_t value,uint32_t other) {
    uint32_t temporary; (void)context;
    switch(p) {
    case IDC_D0_BYTE: SET_B(D(0),value); flags_logic_b(value); break;
    case IDC_D1_BYTE: SET_B(D(1),value); flags_logic_b(value); break;
    case IDC_D2_BYTE: SET_B(D(2),value); flags_logic_b(value); break;
    case IDC_D0_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case IDC_D1_WORD: SET_W(D(1),value); flags_logic_w(value); break;
    case IDC_D0_LONG: full(0,value); break;
    case IDC_D1_LONG: full(1,value); break;
    case IDC_D0_FROM_D1: full(0,D(1)); break;
    case IDC_EXT_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case IDC_EXT_D0_LONG: full(0,value); break;
    case IDC_EXT_D1_LONG: full(1,value); break;
    case IDC_STORE_BYTE: case IDC_TEST_BYTE: flags_logic_b(value); break;
    case IDC_STORE_WORD: case IDC_TEST_WORD: flags_logic_w(value); break;
    case IDC_STORE_LONG: flags_logic_l(value); break;
    case IDC_COMPARE_BYTE: step_compare_byte((uint8_t)other,(uint8_t)value); break;
    case IDC_COMPARE_WORD: step_compare_word((uint16_t)other,(uint16_t)value); break;
    case IDC_A0: A(0)=value; break;
    case IDC_A1: A(1)=value; break;
    case IDC_AND_D0_WORD: SET_W(D(0),D(0)&value); flags_logic_w(D(0)); break;
    case IDC_AND_D1_WORD: SET_W(D(1),D(1)&value); flags_logic_w(D(1)); break;
    case IDC_SUB_D0_WORD: step_subtract_word(&D(0),(uint16_t)value); break;
    case IDC_SUB_D1_WORD: step_subtract_word(&D(1),(uint16_t)value); break;
    case IDC_ADD_D0_WORD: step_add_word(&D(0),(uint16_t)value); break;
    case IDC_ADD_D0_BYTE: renderer_add_byte(&D(0),(uint8_t)value); break;
    case IDC_SUB_D0_BYTE: step_subtract_byte(&D(0),(uint8_t)value); break;
    case IDC_SUB_D2_BYTE: step_subtract_byte(&D(2),(uint8_t)value); break;
    case IDC_ASR_D0_WORD: renderer_asr_word(&D(0),value); break;
    case IDC_ASR_D1_WORD: renderer_asr_word(&D(1),value); break;
    case IDC_ASL_D0_LONG: step_asl_long(&D(0),value); break;
    case IDC_ASL_D1_LONG: step_asl_long(&D(1),value); break;
    case IDC_SUB_D1_LONG: step_subtract_long(&D(1),value); break;
    case IDC_ADD_MEMORY_WORD: temporary=value; step_add_word(&temporary,(uint16_t)other); break;
    case IDC_SUB_MEMORY_WORD: temporary=value; step_subtract_word(&temporary,(uint16_t)other); break;
    case IDC_ADD_MEMORY_LONG: temporary=value; step_add_long(&temporary,other); break;
    case IDC_PUSH_FIRST_PALETTE: palette_arguments(A(0)); break;
    case IDC_PUSH_INPUT_PORT: push(rd_u32(0xc0815c)); break;
    case IDC_PUSH_INPUT_REQUEST: push(A(0)); break;
    case IDC_READ_COUNTERS: REG_PPC=value?0xc17196:0xc17130; break;
    }
}
static const InputDeviceHooks hooks={consume,outputs,NULL};
static void link_frame(unsigned size) { m68ki_push_32(A(6)); A(6)=A(7); A(7)-=size; }
static int leave_frame(void) { A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
int glue_C1718E(void) { link_frame(22); m68ki_push_32(D(2)); advance_input_device_callback(A(6),&hooks); D(2)=m68ki_pull_32(); return leave_frame(); }
int glue_C17456(void) { install_input_device_callback(&hooks); return glue_return(); }
int glue_C1748C(void) { remove_input_device_callback(&hooks); return glue_return(); }
int glue_C174A0(void) { link_frame(0); prepare_input_device_port(A(6),&hooks); return leave_frame(); }
int glue_C16CD8(void) { link_frame(2); open_input_device_timer(A(6),&hooks); return leave_frame(); }
int glue_C16B8C(void) { open_input_device_request(&hooks); return glue_return(); }
int glue_C17104(void) { link_frame(0); set_input_device_bounds(A(6),&hooks); return leave_frame(); }
int glue_C1712C(void) { link_frame(2); initialise_input_device_counters(A(6),&hooks); return leave_frame(); }
