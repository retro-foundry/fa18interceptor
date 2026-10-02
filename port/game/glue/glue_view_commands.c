#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_view_commands.h"

static uint32_t consume(void *context,enum ViewCommandChild child) {
    (void)context;
    return (uint32_t)glue_complete_child(child==VIEW_COMMAND_ZOOM_MAXIMUM?0xc08324:0xc082b8,
                                       child==VIEW_COMMAND_ZOOM_MAXIMUM?0xc1b9e0:0xc1ba8c);
}
static void outputs(void *context,enum ViewCommandPhase phase,uint32_t value,
                    uint32_t limit,gaddr address) {
    uint32_t temporary;
    (void)context;
    switch(phase) {
    case VIEW_BYTE_TEST: case VIEW_BYTE_STORE: case VIEW_ORIGIN_TEST: flags_logic_b(value); break;
    case VIEW_WORD_STORE: flags_logic_w(value); break;
    case VIEW_LONG_STORE: flags_logic_l(value); break;
    case VIEW_REQUEST_BIT: FLAG_Z=value&(1u<<limit); break;
    case VIEW_ORIGIN_READ: case VIEW_RECORD_TYPE_READ: case VIEW_RECORD_TYPE_MASK:
    case VIEW_MODE_READ: case VIEW_ZOOM_FLAGS_READ: case VIEW_ZOOM_FLAGS_MASK:
        SET_B(D(4),value); flags_logic_b(value); break;
    case VIEW_DETAIL_SET: case VIEW_ORIGIN_SET: D(4)=value; flags_logic_l(value); break;
    case VIEW_ORIGIN_DECREMENT: step_subtract_byte(&D(4),1); break;
    case VIEW_ORIGIN_INCREMENT: renderer_add_byte(&D(4),1); break;
    case VIEW_ORIGIN_COMPARE: step_compare_byte(limit,D(4)); break;
    case VIEW_MIDDLE_READ: case VIEW_MIDDLE_SET: D(1)=value; flags_logic_l(value); break;
    case VIEW_MIDDLE_DECREASE: step_subtract_long(&D(1),0x02000000); break;
    case VIEW_MIDDLE_INCREASE: step_add_long(&D(1),0x02000000); break;
    case VIEW_MIDDLE_COMPARE: step_compare_long(limit,D(1)); break;
    case VIEW_MODE_ZERO: SET_B(D(7),0); flags_logic_b(0); break;
    case VIEW_MODE_SET: D(7)=value; flags_logic_l(value); break;
    case VIEW_MODE_DECREMENT: temporary=value; step_subtract_byte(&temporary,1); break;
    case VIEW_MODE_INCREMENT: temporary=value; renderer_add_byte(&temporary,1); break;
    case VIEW_BYTE_COMPARE: step_compare_byte(limit,value); break;
    case VIEW_RECORD_ADDRESS: case VIEW_SPAN_TABLE: A(0)=address; break;
    case VIEW_ROW_SET: SET_W(D(7),value); flags_logic_w(value); break;
    case VIEW_ROW_COMPARE: step_compare_word(limit,D(7)); break;
    case VIEW_SPAN_INDEX:
        SET_B(D(4),value); SET_W(D(4),(int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case VIEW_SPAN_READ:
        SET_B(D(4),value); SET_W(D(4),(int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case VIEW_SPAN_SCALE: renderer_asl_word(&D(4),4); break;
    case VIEW_ZOOM_OUT_BEGIN: D(3)=0x20; flags_logic_l(0x20); break;
    case VIEW_ZOOM_OUT_COMPARE: step_compare_word(value,D(3)); break;
    case VIEW_ZOOM_IN_COMPARE: step_compare_word(limit,value); break;
    case VIEW_ZOOM_DECREASE: temporary=value; renderer_asr_word(&temporary,1); break;
    case VIEW_ZOOM_INCREASE: temporary=value; renderer_asl_word(&temporary,1); break;
    case VIEW_ZOOM_FLAGS_CLEAR: case VIEW_ZOOM_FLAGS_SET: FLAG_Z=value&0x80u; break;
    }
}
static const ViewCommandHooks hooks={consume,outputs,NULL};
const ViewCommandHooks *glue_view_command_hooks(void) { return &hooks; }
uint32_t glue_execute_view_command(const CommandRequest *request) {
    return execute_view_command(request,&hooks);
}
