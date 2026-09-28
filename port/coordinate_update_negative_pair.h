#ifndef FA18_COORDINATE_UPDATE_NEGATIVE_PAIR_H
#define FA18_COORDINATE_UPDATE_NEGATIVE_PAIR_H

#include "hunk.h"

#include <stddef.h>
#include <stdint.h>

/* Hunk 63 begins at the source `$C3DB00` angle table used by `$C123FA`. */
typedef struct {
    const uint8_t *bytes;
    size_t byte_count;
} FA18CoordinateAngleTable;

typedef struct {
    int32_t first_component;
    int32_t second_component;
    int32_t third_component;
    int32_t terminal_component;
} FA18CoordinateNegativePairInput;

typedef struct {
    int16_t output_x;
    int16_t output_z;
    uint8_t status_flag;
} FA18CoordinateNegativePairOutput;

/* Load the original `$C3DB00` source table from Hunk 63. */
int fa18_load_coordinate_angle_table(const FA18Hunks *hunks,
                                     FA18CoordinateAngleTable *table);

/* Exact observed `$C123FA` route `$C12402-$C12730`: normalized first and
 * second components are negative at entry, the first is dominant, the
 * low-scale branch is selected, and the terminal component is negative.
 * Other source branches remain unported and return -1. */
int fa18_update_coordinate_negative_pair(
    const FA18CoordinateAngleTable *table,
    const FA18CoordinateNegativePairInput *input,
    FA18CoordinateNegativePairOutput *output);

#endif
