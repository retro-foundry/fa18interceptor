#ifndef FA18_TWO_ANGLE_MATRIX_H
#define FA18_TWO_ANGLE_MATRIX_H

#include "flight.h"
#include "hunk.h"

#include <stdint.h>

/* `$C2E38E-$C2E3DD`: construct the two-angle matrix written to `$C45BD8`.
 * The sine table is the original Hunk 63 data at `$C3E5E8`; no captured
 * emulator state is used. */
int fa18_load_two_angle_trig_table(const FA18Hunks *hunks,
                                   FA18FlightTrigTable *table);
int fa18_build_two_angle_matrix(const FA18FlightTrigTable *table,
                                int16_t first_angle, int16_t second_angle,
                                int16_t output[3][3]);

/* `$C2E346-$C2E36F`: construct the one-angle matrix written to `$C45BFC`. */
int fa18_build_single_angle_matrix(const FA18FlightTrigTable *table,
                                   int16_t angle, int16_t output[3][3]);

#endif
