/* Glue for update_voices $C50158 and format_date_line $C24E2C. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "globals.h"
#include "memory.h"
#include "numbers.h"

void print_number_registers(void); /* glue_batch14.c */

/* $C50158: called by Kickstart; every register is restored, the flags are
 * those of the final CMPI.W #4 with D3 = 4. */
int glue_C50158(void) {
    update_voices();
    FLAG_N = NFLAG_CLEAR;
    FLAG_Z = ZFLAG_SET;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
    return glue_return();
}

/* $C24E2C: ends in print_number with D0 the day, A0 = DATE_LINE, D3 = $15,
 * D2 = 2; its leftovers follow. */
int glue_C24E2C(void) {
    uint32_t seconds = rd_u32(rd_u32(MODE_TABLE) + 8);
    uint32_t q = seconds / 3600;
    int16_t days = (int16_t)(q > 0xFFFF ? seconds : q);
    int16_t day = (int16_t)((days & 0x1F) + 1);

    format_date_line();
    if (day > 30) day = 30;
    /* print_number's leftovers (it prints the same digits again). */
    D(0) = (uint32_t)(int32_t)day;
    A(0) = DATE_LINE;
    D(3) = 0x15;
    D(2) = 2;
    print_number_registers();
    return glue_return();
}
