#ifndef FA18_MAP_PACKET_DIRECTORY_H
#define FA18_MAP_PACKET_DIRECTORY_H

#include <stdint.h>

typedef enum {
    FA18_MAP_PACKET_DIRECTORY_NORMAL = 0,
    FA18_MAP_PACKET_DIRECTORY_WIDE = 1
} FA18MapPacketDirectoryLayout;

typedef struct {
    FA18MapPacketDirectoryLayout layout;
    uint16_t frame_flag;
    uint32_t record_base_address;
    uint16_t coordinate_bin_shift;
    int16_t row_max;
    int16_t column_max;
} FA18MapPacketDirectoryState;

/* `$C2AB34-$C2AB7B`: initialize the normal or wide map packet-directory
 * fields before their shared coordinate/bin setup at `$C2AB7C`. */
int fa18_initialize_map_packet_directory(FA18MapPacketDirectoryLayout layout,
                                         FA18MapPacketDirectoryState *state);

#endif
