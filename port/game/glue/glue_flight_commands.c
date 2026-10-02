/* Independent whole-call adaptation of the shared aircraft command actions. */
#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_flight_commands.h"
#include "globals.h"

static const struct { uint32_t entry, ret; } children[]={
    {0xc1c214,0xc1b14a}, {0xc33186,0xc1b1aa}, {0xc33186,0xc1b1ce},
    {0xc0833e,0xc1b228}, {0xc08394,0xc1b232},
    {0xc1b50c,0xc1b4f8}, {0xc1b510,0xc1b500}, {0xc1b514,0xc1b508},
    {0xc1b558,0xc1b544}, {0xc1b55c,0xc1b54c}, {0xc1b560,0xc1b554},
    {0xc1b602,0xc1b5d8}, {0xc1b602,0xc1b5e0}, {0xc33186,0xc1b5e6},
    {0xc33186,0xc1b630}, {0xc25704,0xc1b65e},
    {0xc33186,0xc1bb9c}, {0xc25704,0xc1bbb4}, {0xc33186,0xc1bbc0},
    {0xc33186,0xc1bc4c}, {0xc33186,0xc1c062},
    {0xc25704,0xc1c11c}, {0xc17f8c,0xc1c16a},
    {0xc25704,0xc1c1aa}, {0xc33186,0xc1c20e}
};
static FlightCommandResult consume(void *context,enum FlightCommandChild child) {
    FlightCommandResult result;
    (void)context;
    glue_complete_child(children[child].entry,children[child].ret);
    result.event=D(0); result.carried_event_word=(int16_t)D(4); return result;
}
static void outputs(void *context,enum FlightCommandPhase phase,uint32_t value,
                    uint32_t limit,gaddr address) {
    uint32_t temporary;
    (void)context;
    switch(phase) {
    case FLIGHT_BYTE_TEST: case FLIGHT_BYTE_STORE: flags_logic_b(value); break;
    case FLIGHT_WORD_TEST: case FLIGHT_WORD_STORE: flags_logic_w(value); break;
    case FLIGHT_REQUEST_BIT: case FLIGHT_TOGGLE_BIT: FLAG_Z=value&(1u<<limit); break;
    case FLIGHT_TEST_BIT: FLAG_Z=value; break;
    case FLIGHT_MODIFIER_TEST: flags_logic_b(D(6)); break;
    case FLIGHT_BYTE_COMPARE: step_compare_byte(limit,value); break;
    case FLIGHT_RADAR_RECORD: A(0)=address; break;
    case FLIGHT_RADAR_READ: case FLIGHT_RADAR_MASK: case FLIGHT_RADAR_SELECT:
    case FLIGHT_HUD_READ: case FLIGHT_HUD_WRAP: case FLIGHT_WEAPON_READ:
    case FLIGHT_WEAPON_MASK: case FLIGHT_WEAPON_WRAP:
        SET_B(D(4),value); flags_logic_b(value); break;
    case FLIGHT_RADAR_COMPARE: step_compare_byte(limit,D(4)); break;
    case FLIGHT_INFO_READ: SET_W(D(0),value); flags_logic_w(value); break;
    case FLIGHT_INFO_INCREMENT: step_add_word(&D(0),1); break;
    case FLIGHT_INFO_COMPARE: step_compare_word(limit,D(0)); break;
    case FLIGHT_INFO_WRAP: D(0)=1; flags_logic_l(1); break;
    case FLIGHT_INFO_FLAG: FLAG_Z=D(0)&0x8000u; D(0)|=0x8000u; break;
    case FLIGHT_HUD_INCREMENT: renderer_add_byte(&D(4),1); break;
    case FLIGHT_HUD_COMPARE: step_compare_byte(limit,D(4)); break;
    case FLIGHT_INPUT_VALUE: SET_B(D(2),value); flags_logic_b(value); break;
    case FLIGHT_INPUT_RELEASE: D(2)=value; flags_logic_l(value); break;
    case FLIGHT_INPUT_READ: case FLIGHT_INPUT_MASK: case FLIGHT_INPUT_COMBINE:
        SET_B(D(1),value); flags_logic_b(value); break;
    case FLIGHT_SOUND_SWAP: D(0)=(D(0)<<16)|(D(0)>>16); flags_logic_l(D(0)); break;
    case FLIGHT_SOUND_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case FLIGHT_SOUND_CARRY: SET_W(D(4),D(0)); flags_logic_w(D(4)); break;
    case FLIGHT_SOUND_RESTORE: SET_W(D(0),D(4)); flags_logic_w(D(0)); break;
    case FLIGHT_COUNTER_DECREMENT: temporary=value; step_subtract_byte(&temporary,1); break;
    case FLIGHT_WEAPON_DECREMENT: step_subtract_byte(&D(4),0x10); break;
    case FLIGHT_GEAR_READ: case FLIGHT_GEAR_MASK: D(4)=value; flags_logic_l(value); break;
    case FLIGHT_ECM_BEGIN: case FLIGHT_TOGGLE_ADDRESS: A(0)=address; break;
    case FLIGHT_SPAWN_SAVE:
        A(7)-=2; wr_u16(A(7),(uint16_t)D(0)); flags_logic_w(D(0)); break;
    case FLIGHT_SPAWN_ARGUMENT:
        D(0)=value; flags_logic_l(value); m68ki_push_32(value); flags_logic_l(value); break;
    case FLIGHT_SPAWN_RESTORE:
        A(7)+=8; SET_W(D(0),rd_u16(A(7))); A(7)+=2; flags_logic_w(D(0)); break;
    }
}
static const FlightCommandHooks hooks={consume,outputs,NULL};
const FlightCommandHooks *glue_flight_command_hooks(void) { return &hooks; }
uint32_t glue_execute_flight_command(const CommandRequest *request) {
    return execute_flight_command(request,(int16_t)D(4),&hooks);
}
