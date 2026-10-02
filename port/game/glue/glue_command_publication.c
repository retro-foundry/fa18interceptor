#include "glue_renderer_step_math.h"
#include "glue_command_publication.h"
#include "globals.h"

static void outputs(void *context,enum CommandPublicationPhase phase,
                    uint32_t value,gaddr address) {
    uint32_t temporary;
    (void)context; (void)address;
    switch(phase) {
    case COMMAND_PUBLICATION_TAKEN: flags_logic_b(value); break;
    case COMMAND_PUBLICATION_RELEASE: FLAG_Z=value; break;
    case COMMAND_PUBLICATION_CLAIM: flags_logic_b(1); break;
    case COMMAND_PUBLICATION_COUNT: step_compare_byte(10,value); break;
    case COMMAND_PUBLICATION_WRITE_INDEX:
        SET_B(D(4),value); flags_logic_b(value); step_compare_byte(10,value); break;
    case COMMAND_PUBLICATION_RESET_INDEX: D(4)=0; flags_logic_l(0); break;
    case COMMAND_PUBLICATION_RAW:
        SET_W(D(4),(int16_t)(int8_t)D(4)); A(3)=KEY_RAW; flags_logic_b(value); break;
    case COMMAND_PUBLICATION_TRANSLATED:
        SET_W(D(0),D(0)&0xffu); A(3)=KEY_TABLE; SET_B(D(0),value); flags_logic_b(value); break;
    case COMMAND_PUBLICATION_ADVANCE_INDEX:
        renderer_add_byte(&D(4),1); flags_logic_b(D(4)); break;
    case COMMAND_PUBLICATION_ADVANCE_COUNT:
        temporary=value; renderer_add_byte(&temporary,1); break;
    case COMMAND_PUBLICATION_TRANSLATED_INDEX:
        A(3)=KEY_TRANSLATED; SET_B(D(4),value);
        SET_W(D(4),(int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case COMMAND_PUBLICATION_TRANSLATED_WRITE: flags_logic_b(value); break;
    case COMMAND_PUBLICATION_CLEAR: flags_logic_b(0); break;
    }
}
/* Full shared exit body; it is not registered as an extra original function. */
static const CommandPublicationHooks hooks={outputs,NULL};
const CommandPublicationHooks *glue_command_publication_hooks(void) { return &hooks; }
int glue_publish_command_event(void) {
    publish_command_event((uint8_t)D(0),&hooks); return glue_return();
}
