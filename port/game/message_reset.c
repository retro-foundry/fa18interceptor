/* C11312: shared message queue reset, independent of scene/CPU owners. */
#include "stages.h"
#include "globals.h"

void reset_message_sequence(void) {
    wr_u16(MESSAGE_QUEUE, 0);
    wr_u16(MESSAGE_QUEUE + 2, 0);
    wr_u32(MESSAGE_TIMER, 0x1B8);
    wr_u8(MESSAGE_STATE_A, 0);
    wr_u8(MESSAGE_STATE_B, 0);
    wr_u8(MESSAGE_STATE_C, 0);
    wr_u8(MESSAGE_STATE_D, 0);
}

