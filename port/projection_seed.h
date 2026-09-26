#ifndef FA18_PROJECTION_SEED_H
#define FA18_PROJECTION_SEED_H

#include <stdint.h>

typedef struct { int16_t value[3][3]; } FA18Fixed14Matrix;
typedef struct { int32_t x, y, z; } FA18ProjectionBase;
typedef struct { int32_t x, y, z; } FA18ProjectionSeedResult;

/* `$C1C54E-$C1C5DF`: select the record-type seed, apply its 2.14 matrix,
 * and add the result to the named projection base. */
int fa18_select_projection_seed(uint8_t record_type,
                                const FA18Fixed14Matrix *matrix,
                                FA18ProjectionBase base,
                                FA18ProjectionSeedResult *result);

#endif
