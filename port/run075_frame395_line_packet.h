#ifndef FA18_RUN075_FRAME395_LINE_PACKET_H
#define FA18_RUN075_FRAME395_LINE_PACKET_H

#include "line.h"

/* First `$C2FB7A` packet captured while constructing run075 frame 395. */
static const FA18LinePacket fa18_run075_frame395_line_packet = {
    .bltcon1 = 85,
    .bltbmod = 0xfff8,
    .bltamod = 0,
    .bltsize = 0x005d,
    .destination_byte_offset = 0x0ec5,
    .active_plane_mask = 0x0f
};

/* The next three C2FB7A submissions, captured by skipping one, two, and
 * three prior hits respectively.  These are adapter inputs, not Amiga
 * memory images. */
static const FA18LinePacket fa18_run075_frame395_line_packets[] = {
    { 85, 0xfff8, 0, 0x005d, 0x0ec5, 0x0f },
    { 81, 0xfffe, 0, 0x0082, 0x0ec4, 0x0f },
    { 81, 0xffe6, 0, 0x0382, 0x0eec, 0x0f },
    { 81, 0xfffe, 0, 0x0082, 0x0ec6, 0x0f }
};

#define FA18_RUN075_FRAME395_LINE_PACKETS \
    (sizeof(fa18_run075_frame395_line_packets) / \
     sizeof(fa18_run075_frame395_line_packets[0]))

#endif
