#ifndef FA18_MAP_PACKET_STATIC_DATA_H
#define FA18_MAP_PACKET_STATIC_DATA_H

#include "hunk.h"
#include "map_packet_relative_offset.h"

enum {
    FA18_MAP_PACKET_CONTROL_HUNK = 28,
    FA18_MAP_PACKET_CONTROL_RUNTIME_BASE = 0x00c29f00,
    FA18_MAP_PACKET_PACKET_HUNK = 68,
    FA18_MAP_PACKET_PACKET_RUNTIME_BASE = 0x00c42ca8
};

typedef struct {
    const uint8_t *control_bytes;
    size_t control_size;
    const uint8_t *packet_bytes;
    size_t packet_size;
} FA18MapPacketStaticData;

/* Bind the two original code-Hunk payloads that `$C2AA9C-$C2AFF9` reads as
 * data: Hunk 28 for selector/control streams and Hunk 68 for map packets. */
int fa18_load_map_packet_static_data(const FA18Hunks *hunks,
                                     FA18MapPacketStaticData *data);

int fa18_resolve_map_packet_control_pair(void *context, uint8_t mode,
                                         int8_t pair[2]);
int fa18_resolve_map_packet_control_stream(void *context, uint32_t address,
                                           const uint8_t **stream, size_t *size);
int fa18_resolve_map_packet_static_packet(void *context, uint32_t address,
                                          const uint8_t **packet, size_t *size);

#endif
