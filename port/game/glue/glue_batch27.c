/* Glue for update_target_point $C1C2C8 and blit_lane $C304FA. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "render_buffers.h"

/* $C1C2C8 restores D0-D6; A0 is left at the viewed record when enabled. */
int glue_C1C2C8(void) {
    int enabled = rd_u8(TARGET_ENABLED) != 0;
    update_target_point();
    if (enabled) A(0) = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    return glue_return();
}

/* $C304FA: D0.w plane offset, D3 bit 0 the minterm choice. The caller reads
 * D4 (C modulo, word), D5 (C pointer), D6, D7 and the high words of D0-D3. */
