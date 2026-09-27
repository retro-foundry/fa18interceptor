#ifndef FA18_MAP_PACKET_TRANSFORM_H
#define FA18_MAP_PACKET_TRANSFORM_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int16_t value[9];
} FA18MapPacketMatrix;

typedef struct {
    uint32_t packed_seed;
    int16_t output_base[3];
    uint16_t detail_shift;
    FA18MapPacketMatrix matrix;
} FA18MapPacketTransform;

typedef struct {
    int16_t value[2];
} FA18MapPacketPair;

typedef struct {
    int16_t value[3];
} FA18MapPacketProjectionRecord;

/* `$C2AF92-$C2AFE0`: transform the source's two-word static map records into
 * three-word projection workspace records. The caller owns the subsequent
 * `$C246A0` display-stage invocation and stream continuation. */
int fa18_transform_map_packet_pairs(const FA18MapPacketTransform *transform,
                                    const FA18MapPacketPair *pairs,
                                    size_t pair_count, int16_t count,
                                    FA18MapPacketProjectionRecord *output,
                                    size_t output_count,
                                    uint16_t *transformed_count);

#endif
