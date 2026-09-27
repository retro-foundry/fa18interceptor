#ifndef FA18_PROJECTION_GRID_H
#define FA18_PROJECTION_GRID_H

#include <stdint.h>

#include "hunk.h"

enum {
    FA18_C279_PROJECTION_GRID_HUNK = 25,
    FA18_C279_PROJECTION_GRID_OFFSET = 0x0754,
    FA18_C279_PROJECTION_GRID_RECORD_BYTES = 6
};

typedef struct {
    uint16_t record_count;
    uint16_t bounds_limit;
    const uint8_t *records;
} FA18ProjectionGrid;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t kind;
} FA18ProjectionGridRecord;

typedef struct {
    uint16_t record_count;
    uint16_t bounds_limit;
    int16_t grid_x;
    int16_t grid_y;
    int16_t scaled_input;
    uint8_t coordinate_shift;
} FA18ProjectionGridSetup;

/* `$C27A36-$C27ADA`: bind the `$C28124` table from original Hunk 25 data. */
int fa18_load_projection_grid(const FA18Hunks *hunks, FA18ProjectionGrid *grid);

/* `$C27B20`: decode one three-word input record. */
int fa18_projection_grid_record(const FA18ProjectionGrid *grid, uint16_t index,
                                FA18ProjectionGridRecord *record);

/* `$C27A36-$C27ADA` observed normal route. Returns one for the separate
 * original `< -$80` branch, which has not been reconstructed. */
int fa18_prepare_projection_grid(const FA18ProjectionGrid *grid,
                                 int16_t projection_input,
                                 int16_t component_x, int16_t component_y,
                                 FA18ProjectionGridSetup *setup);

#endif
