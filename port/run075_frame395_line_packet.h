#ifndef FA18_RUN075_FRAME395_LINE_PACKET_H
#define FA18_RUN075_FRAME395_LINE_PACKET_H

#include "line.h"

/* First `$C2FB7A` packet captured while constructing run075 frame 395. */
static const FA18LinePacket fa18_run075_frame395_line_packet = {
    .bltcon1 = 85,
    .bltbmod = 0xfff8,
    .bltamod = 0,
    .bltsize = 0,
    .destination_offset = 94u * FA18_WIDTH + 21u,
    .active_plane_mask = 0x0f
};

#endif
