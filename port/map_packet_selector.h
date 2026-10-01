#ifndef FA18_MAP_PACKET_SELECTOR_H
#define FA18_MAP_PACKET_SELECTOR_H

#include <stddef.h>
#include <stdint.h>

typedef int (*FA18MapPacketStreamResolver)(void *context, uint32_t reference,
                                           const uint8_t **stream, size_t *size);

typedef struct {
    const uint8_t *packet;
    size_t packet_size;
    int32_t packet_origin;
    uint8_t origin_adjusted;
    int16_t origin_matrix[3];
    int32_t detail_metric;
    uint8_t alternate_stream;
    FA18MapPacketStreamResolver resolve_stream;
    void *context;
} FA18MapPacketSelectorInput;

typedef enum {
    FA18_MAP_PACKET_READY = 0,
    FA18_MAP_PACKET_REJECTED = 1,
    FA18_MAP_PACKET_COUNT_ERROR = 2
} FA18MapPacketSelectorRoute;

typedef struct {
    const uint8_t *pair_stream;
    size_t pair_stream_size;
    int16_t pair_count;
    int16_t origin_component[3];
    uint16_t error_code;
} FA18MapPacketSelectorResult;

/* `$C2AEFC-$C2AF91`: select a static packet's inline or alternate stream,
 * derive its origin triple, and decode the count/relative-detail guard that
 * precedes `$C2AF92`. */
int fa18_select_map_packet_stream(const FA18MapPacketSelectorInput *input,
                                  FA18MapPacketSelectorResult *result,
                                  FA18MapPacketSelectorRoute *route);

#endif
