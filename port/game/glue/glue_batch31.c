/* Glue for update_condition_a/b $C09A78/$C09A98 and drop_lost_selection
 * $C12242. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "player_input.h"

void condition_registers(void);      /* glue_batch28.c */
int view_key_would_queue(uint8_t raw);           /* glue_batch29.c */
void view_key_leftovers(uint8_t raw, int queued);

/* The callers read X, D6 and A1 as the condition scan leaves them. */
int glue_C09A78(void) {
    A(0) = CONDITIONS_A;
    condition_registers();
    update_condition_a();
    return glue_return();
}

int glue_C09A98(void) {
    A(0) = CONDITIONS_B;
    condition_registers();
    update_condition_b();
    return glue_return();
}

/* $C12242: every register is live after it. */
int glue_C12242(void) {
    gaddr record;
    int queued;
    if (!rd_u16(TARGET_RECORD)) return glue_return();
    D(0) = (uint32_t)(int32_t)rd_s16(TARGET_RECORD) << 9;
    D(1) = 9;
    record = CONTROL_RECORDS + D(0);
    A(0) = record;
    SET_W(D(0), rd_u16(record));
    if (rd_u16(record) & 0x40) return glue_return();
    D(0) = rd_u8(CONTEXT_SELECT);
    if (D(0)) {
        drop_lost_selection();
        return glue_return();
    }
    queued = view_key_would_queue(0);
    drop_lost_selection();
    D(0) = 0;
    view_key_leftovers(0, queued);
    return glue_return();
}
