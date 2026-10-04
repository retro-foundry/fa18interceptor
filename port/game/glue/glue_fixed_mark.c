/* Register flow for the fixed tuple multiplied by the 2.8 view matrix. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "projection.h"

void projection_mode_registers(int16_t mode, int entry);

static int32_t product(int16_t value, gaddr coefficient) {
    return (int32_t)value * rd_s16(coefficient);
}
