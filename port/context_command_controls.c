/* Actual $C091E0 local-to-world and $C0915A observer children. */
#include "context_command_input.h"

int fa18_apply_context_control_child(FA18ContextCommandState *s,
                                     enum ContextCommandChild child,
                                     const FA18ContextCommandChildInput *input,
                                     FA18ContextCommandChildResult *result) {
    unsigned row,column;
    if(!s || !s->view || !input || !result) return 0;
    if(child==CONTEXT_COMMAND_LOCAL_TO_WORLD) {
        if(!input->record) return 0;
        for(row=0;row<3;++row) {
            uint32_t sum=0;
            for(column=0;column<3;++column)
                sum+=(uint32_t)((int32_t)input->local[column]*input->record->inverse[row][column]);
            result->position[row]=(int32_t)((uint32_t)((int32_t)sum>>4)+input->record->position[row]);
        }
        result->event=(uint32_t)result->position[0]; return 1;
    }
    if(child==CONTEXT_COMMAND_SET_OBSERVER) {
        s->origin_first=(uint32_t)input->position[0];
        s->view->origin_middle=(uint32_t)input->position[1];
        s->origin_third=(uint32_t)input->position[2];
        s->negated[0]=0u-((uint32_t)input->position[0]&0x3fffffu);
        s->negated[1]=0u-(uint32_t)input->position[1];
        s->negated[2]=0u-((uint32_t)input->position[2]&0x3fffffu);
        for(row=0;row<3;++row) result->position[row]=(int32_t)s->negated[row];
        result->event=s->negated[0]; return 1;
    }
    return 0;
}
