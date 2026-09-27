#ifndef FA18_MAP_PACKET_RELATIVE_OFFSET_H
#define FA18_MAP_PACKET_RELATIVE_OFFSET_H

#include <stddef.h>
#include <stdint.h>

typedef int (*FA18MapPacketControlPairResolver)(void *context, uint8_t mode,
                                                 int8_t pair[2]);
typedef int (*FA18MapPacketAddressResolver)(void *context, uint32_t address,
                                            const uint8_t **data, size_t *size);

typedef struct {
    uint8_t mode;
    int16_t row_min;
    int16_t row_max;
    int16_t column_min;
    int16_t column_max;
    uint8_t wide_layout;
    uint8_t allow_negative_packet;
    uint32_t record_base_address;
    const uint8_t *record_directory;
    size_t record_directory_size;
    FA18MapPacketControlPairResolver resolve_control_pair;
    FA18MapPacketAddressResolver resolve_packet;
    void *context;
} FA18MapPacketRelativeOffsetInput;

typedef enum {
    FA18_MAP_PACKET_RELATIVE_OFFSET_READY = 0,
    FA18_MAP_PACKET_RELATIVE_OFFSET_OUT_OF_BOUNDS = 1,
    FA18_MAP_PACKET_RELATIVE_OFFSET_RETRY = 2,
    FA18_MAP_PACKET_RELATIVE_OFFSET_INVALID = 3
} FA18MapPacketRelativeOffsetRoute;

typedef struct {
    int16_t row;
    int16_t column;
    uint32_t packet_address;
    const uint8_t *packet;
    size_t packet_size;
    uint16_t error_code;
} FA18MapPacketRelativeOffsetResult;

/* `$C2AD80-$C2AE1A`: obtain the signed row/column pair for one control mode,
 * bounds-check it, read the positive relative packet offset from the selected
 * normal/wide directory, and reject a disabled negative packet header. */
int fa18_select_map_packet_relative_offset(
    const FA18MapPacketRelativeOffsetInput *input,
    FA18MapPacketRelativeOffsetResult *result,
    FA18MapPacketRelativeOffsetRoute *route);

#endif
