#ifndef FA18_PROJECTION_GRID_H
#define FA18_PROJECTION_GRID_H

#include <stdint.h>

#include "hunk.h"

enum {
    FA18_C279_PROJECTION_GRID_HUNK = 25,
    FA18_C279_PROJECTION_BOUNDS_OFFSET = 0x0354,
    FA18_C279_PROJECTION_BOUNDS_BYTES = 0x0400,
    FA18_C279_PROJECTION_GRID_OFFSET = 0x0754,
    FA18_C279_PROJECTION_GRID_RECORD_BYTES = 6
};

typedef struct {
    uint16_t record_count;
    int16_t bounds_limit;
    const uint8_t *records;
    const uint8_t *bounds_table;
} FA18ProjectionGrid;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t kind;
} FA18ProjectionGridRecord;

typedef struct {
    uint16_t record_count;
    int16_t bounds_limit;
    int16_t grid_x;
    int16_t grid_y;
    int16_t scaled_input;
    uint8_t coordinate_shift;
} FA18ProjectionGridSetup;

typedef struct {
    int16_t shifted_x;
    int16_t shifted_y;
    int16_t kind;
    int16_t bound;
} FA18ProjectionGridPreparedRecord;

typedef struct {
    int16_t words[9];
} FA18ProjectionPairMatrix;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t depth;
} FA18ProjectionPairBase;

typedef struct {
    int16_t x;
    int16_t y;
} FA18ProjectionPairInput;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t depth;
} FA18ProjectionPairOutput;

typedef struct {
    int16_t x;
    int16_t y;
} FA18ProjectionPairScreenPoint;

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

/* `$C27B20-$C27B8E`: prepare one table record before the pair projection.
 * `negative_kind_flag` is the source's `-$22(A6)` word, retained without a
 * promoted semantic name. Returns one when prepared, zero when bounds-cull
 * skips the record, or minus one outside this bounded native model. */
int fa18_prepare_projection_grid_record(const FA18ProjectionGrid *grid,
                                        const FA18ProjectionGridSetup *setup,
                                        uint16_t record_index,
                                        int16_t negative_kind_flag,
                                        FA18ProjectionGridPreparedRecord *record);

/* `$C27B9C-$C27BF0`: project one translated pair through the live sparse
 * `$C45BD8` matrix. This excludes the following cull, perspective divide, and
 * `$C2FF48` polygon submission. */
int fa18_transform_projection_pair(const FA18ProjectionPairMatrix *matrix,
                                   const FA18ProjectionPairBase *base,
                                   const FA18ProjectionPairInput *input,
                                   FA18ProjectionPairOutput *output);

/* `$C27BF2-$C27C4D`: cull and perspective-project one matrix result into the
 * `$C4B392` pair-buffer coordinate system. Returns one when accepted, zero
 * when the source culls it, or minus one outside the bounded DIVS model. */
int fa18_project_projection_pair(const FA18ProjectionPairOutput *input,
                                 FA18ProjectionPairScreenPoint *point);

#endif
