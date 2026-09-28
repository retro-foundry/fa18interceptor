#include "interrupts.h"

#include "globals.h"

gaddr count_interrupt(gaddr data) {
    wr_u16(data + SERVER_COUNT, (uint16_t)(rd_u16(data + SERVER_COUNT) + 1));
    return data;
}
