#include "glue_renderer_step_math.h"
#include "glue_command_selection.h"
#include "globals.h"
#include <stdlib.h>

uint32_t glue_command_action_pc(enum CommandAction action) {
    switch(action) {
    case COMMAND_PENDING_EMPTY: return 0xc1ad70;
    case COMMAND_COUNTER_WAIT: return 0xc1ad72;
    case COMMAND_INVALID_WORD: return 0xc1ac18;
    case COMMAND_QUEUE_ONLY: return 0xc1c23c;
    case COMMAND_FINISH_EVENT: return 0xc1c2b6;
    case COMMAND_RESET_CONTEXT: return 0xc1c2b8;
    case COMMAND_SPACE: return 0xc1b21c;
    case COMMAND_SPACE_RELEASE: return 0xc1b22c;
    case COMMAND_EJECT: return 0xc1b126;
    case COMMAND_NEXT_TARGET: return 0xc1b1a4;
    case COMMAND_WEAPON_MODE: return 0xc1bb7a;
    case COMMAND_RADAR_RANGE: return 0xc1b1c8;
    case COMMAND_INFO_PAGE: return 0xc1b236;
    case COMMAND_HUD: return 0xc1b264;
    case COMMAND_GEAR: return 0xc1bc12;
    case COMMAND_HOOK: return 0xc1b616;
    case COMMAND_FLARE: return 0xc1c0e0;
    case COMMAND_CHAFF: return 0xc1c172;
    case COMMAND_ECM: return 0xc1c1f2;
    case COMMAND_TARGET: return 0xc1c052;
    case COMMAND_MAP: return 0xc1bf8c;
    case COMMAND_SIGN_INPUT: return 0xc1c224;
    case COMMAND_CONTEXT_SPECIAL: return 0xc1b664;
    case COMMAND_CONTEXT_REQUEST: return 0xc1c06e;
    case COMMAND_CONTEXT_REFRESH: return 0xc1b66c;
    case COMMAND_CONTEXT_CALCULATION: return 0xc1b6c2;
    case COMMAND_Y_DOWN: return 0xc1b4f4;
    case COMMAND_Y_UP: return 0xc1b4fc;
    case COMMAND_Y_RELEASE: return 0xc1b504;
    case COMMAND_X_RIGHT: return 0xc1b540;
    case COMMAND_X_LEFT: return 0xc1b548;
    case COMMAND_X_RELEASE: return 0xc1b550;
    case COMMAND_TRIM_A: return 0xc1b58e;
    case COMMAND_TRIM_B: return 0xc1b594;
    case COMMAND_TRIM_RELEASE: return 0xc1b59a;
    case COMMAND_THROTTLE_UP: return 0xc1b5b8;
    case COMMAND_THROTTLE_DOWN: return 0xc1b5bc;
    case COMMAND_THROTTLE_RELEASE: return 0xc1b5c0;
    case COMMAND_THROTTLE_MODE: return 0xc1b5dc;
    case COMMAND_CONTEXT_VIEW_DECREMENT: return 0xc1b7f0;
    case COMMAND_CONTEXT_VIEW_INCREMENT: return 0xc1b890;
    case COMMAND_CONTEXT_VIEW_ALTERNATE: return 0xc1b7f8;
    case COMMAND_VIEW_ZERO: return 0xc1b8f8;
    case COMMAND_VIEW_ONE: return 0xc1b914;
    case COMMAND_VIEW_INCREMENT: return 0xc1b960;
    case COMMAND_VIEW_DECREMENT: return 0xc1b930;
    case COMMAND_VIEW_TWELVE: return 0xc1b98a;
    case COMMAND_VIEW_THIRTEEN: return 0xc1b9ac;
    case COMMAND_VIEW_THREE: return 0xc1b79a;
    case COMMAND_VIEW_NINE: return 0xc1b7c2;
    case COMMAND_VIEW_TOGGLE: return 0xc1b780;
    case COMMAND_VIEW_EIGHT: return 0xc1b7b6;
    case COMMAND_FIRE_REQUEST: return 0xc1bb66;
    case COMMAND_ZOOM_OUT: return 0xc1bae2;
    case COMMAND_ZOOM_IN: return 0xc1bb02;
    case COMMAND_LOW_INDEX: return 0xc1bc72;
    case COMMAND_FUNCTION_LEVEL: return 0xc1bc50;
    case COMMAND_INDEXED: return 0xc1bc78;
    }
    abort();
}
static void outputs(void *context,enum CommandSelectionPhase phase,
                    uint32_t value,uint32_t limit) {
    uint32_t temporary;
    (void)context;
    switch(phase) {
    case COMMAND_READ_EVENT:
        m68ki_push_32(A(6)); A(6)=A(7); D(0)=value; flags_logic_l(value);
        A(7)=A(6); A(6)=m68ki_pull_32(); break;
    case COMMAND_TEST_COUNTER: case COMMAND_TEST_BYTE: flags_logic_b(value); break;
    case COMMAND_INCREMENT_COUNTER:
        temporary=value; renderer_add_byte(&temporary,1); break;
    case COMMAND_CLEAR_RELEASE: FLAG_Z=D(0)&0x80u; D(0)&=~0x80u; break;
    case COMMAND_READ_ORIGIN: SET_B(D(5),value); flags_logic_b(value); break;
    case COMMAND_READ_MODIFIER: SET_B(D(6),value); flags_logic_b(value); break;
    case COMMAND_READ_DETAIL: SET_B(D(3),value); flags_logic_b(value); break;
    case COMMAND_READ_RECORDER: SET_B(D(1),value); flags_logic_b(value); break;
    case COMMAND_COMPARE_DETAIL: case COMMAND_COMPARE_RECORDER:
    case COMMAND_COMPARE_MODE: case COMMAND_COMPARE_GATE: case COMMAND_COMPARE_KEY:
        step_compare_byte((uint8_t)limit,(uint8_t)value); break;
    case COMMAND_READ_BLOCK: SET_B(D(4),value); flags_logic_b(value); break;
    case COMMAND_SET_INDEX: D(4)=value; flags_logic_l(value); break;
    case COMMAND_SET_LATCH: flags_logic_b(value); break;
    case COMMAND_CLEAR_CONTEXT: case COMMAND_CLEAR_LATCH: flags_logic_b(0); break;
    case COMMAND_READ_WORD_A: SET_W(D(0),value); flags_logic_w(value); break;
    case COMMAND_MASK_WORD_A: SET_W(D(0),value); flags_logic_w(value); break;
    case COMMAND_PENDING_BEGIN:
        D(0)=0; SET_B(D(5),value); D(6)=0; flags_logic_l(0); A(0)=RECORD_WORD_A; break;
    case COMMAND_SELECT_WORD_B: A(0)=RECORD_WORD_B; break;
    case COMMAND_TEST_WORD: flags_logic_w(value); break;
    case COMMAND_CLEAR_WORD_BIT: FLAG_Z=value&(1u<<limit); break;
    }
}
static const CommandSelectionHooks hooks={outputs,NULL};
const CommandSelectionHooks *glue_command_selection_hooks(void) { return &hooks; }
CommandRequest glue_select_keyboard_command(void) {
    CommandRequest request=select_keyboard_command(rd_u32(A(7)+4),&hooks);
    REG_PC=glue_command_action_pc(request.action); return request;
}
CommandRequest glue_select_pending_command(void) {
    CommandRequest request=select_pending_command(&hooks);
    REG_PC=glue_command_action_pc(request.action); return request;
}
