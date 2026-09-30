/* Register flow through the post-input context command ($C10C68). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "stages.h"

void post_input_heading_registers(int apply); /* glue_target_heading.c */

int glue_C10C68(void) {
    int16_t countdown = rd_s16(POST_INPUT_COUNTDOWN);
    uint8_t mode = rd_u8(MODE_SELECT);

    queue_post_input_context_command();
    SET_W(D(0), (uint16_t)countdown);
    if (countdown >= 0) return glue_return();

    SET_B(D(0), mode);
    if ((int8_t)mode >= 3 && (int8_t)mode <= 8)
        post_input_heading_registers(0);
    if (mode != 2) SET_B(D(0), rd_u8(CONTROL_RECORDS + 4));
    A(0) = 0xC10CFEu;
    return glue_return();
}
