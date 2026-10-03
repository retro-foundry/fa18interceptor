#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_context_publication.h"
#include "glue_view_commands.h"
#include "glue_command_publication.h"
#include "context_publication.h"

static ContextPublicationResult consume(void *context,enum ContextPublicationChild child) {
    static const struct { uint32_t entry,ret; } calls[]={
        {0xc08324,0xc1bf16},{0xc1ba86,0xc1bf6c},{0xc25704,0xc09df0}
    };
    ContextPublicationResult result;
    (void)context;
    glue_complete_child(calls[child].entry,calls[child].ret);
    result.event=D(0); result.record_offset=(int16_t)D(1); return result;
}
static void outputs(void *context,enum ContextPublicationPhase phase,uint32_t value,
                    uint32_t limit,gaddr address) {
    (void)context;
    switch(phase) {
    case CONTEXT_PUBLISH_BYTE_TEST: case CONTEXT_PUBLISH_BYTE_STORE: flags_logic_b(value); break;
    case CONTEXT_PUBLISH_WORD_STORE: flags_logic_w(value); break;
    case CONTEXT_PUBLISH_INDEX_SCALE: renderer_asl_word(&D(1),8); step_add_word(&D(1),D(1)); break;
    case CONTEXT_PUBLISH_RECORD_BASE: A(0)=address; break;
    case CONTEXT_PUBLISH_RECORD_KIND: case CONTEXT_PUBLISH_RECORD_MASK:
        SET_B(D(4),value); flags_logic_b(value); break;
    case CONTEXT_PUBLISH_RECORD_COMPARE: step_compare_byte(limit,value); break;
    case CONTEXT_PUBLISH_BYTE_ZERO: flags_logic_b(0); break;
    case CONTEXT_PUBLISH_SELECTED_RECORD: A(1)=address; break;
    case CONTEXT_PUBLISH_BIT_TEST: case CONTEXT_PUBLISH_BIT_SET: case CONTEXT_PUBLISH_BIT_CLEAR:
        FLAG_Z=value&(1u<<limit); break;
    case CONTEXT_PUBLISH_SELECTED_READ: SET_W(D(0),value); flags_logic_w(value); break;
    case CONTEXT_PUBLISH_SELECTED_COMPARE: case CONTEXT_PUBLISH_CHOSEN_COMPARE: step_compare_word(limit,value); break;
    case CONTEXT_PUBLISH_SELECTION_TONE_ID: SET_W(D(0),value); flags_logic_w(value); break;
    case CONTEXT_PUBLISH_WARNING_MASK: flags_logic_l(value); break;
    }
}
static ContextPublicationHooks hooks(void) {
    ContextPublicationHooks result={glue_view_command_hooks(),glue_command_publication_hooks(),consume,outputs,NULL};
    return result;
}
int glue_C1B7A6(void) {
    const ContextPublicationHooks h=hooks();
    publish_context_detail_command((uint8_t)D(0),&h); return glue_return();
}
int glue_C1BEE8(void) {
    const ContextPublicationHooks h=hooks();
    publish_context_record_command(D(0),(int16_t)D(1),&h); return glue_return();
}
int glue_C1C214(void) {
    const ContextPublicationHooks h=hooks();
    publish_context_toggle_command((uint8_t)D(0),A(0),&h); return glue_return();
}
int glue_selected_record_request_body(void) {
    const ContextPublicationHooks h=hooks();
    set_selected_record_request((uint8_t)D(7),&h);
    D(4)=m68ki_pull_32(); D(7)=m68ki_pull_32(); A(1)=m68ki_pull_32();
    return glue_return();
}
int glue_C083A6(void) {
    /* Original MOVEM.L D4/D7/A1,-(SP); flags are not changed. */
    step_predecrement_long(A(1)); step_predecrement_long(D(7)); step_predecrement_long(D(4));
    SET_B(D(7),1); flags_logic_b(1); return glue_selected_record_request_body();
}
int glue_C09DD0(void) {
    const ContextPublicationHooks h=hooks();
    step_predecrement_long(D(0)); flags_logic_l(D(0));
    clear_matching_record_selection(&h);
    D(0)=m68ki_pull_32(); flags_logic_l(D(0)); return glue_return();
}
