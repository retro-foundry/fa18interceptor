#include "blit_job_oracle.h"

#include <string.h>

int fa18_build_run060_frame7991_area_fill(FA18AreaFillPacket *packet) {
    if (!packet) return -1;
    *packet = (FA18AreaFillPacket){
        0x0d0c, 0x0002, 0x00ff, 0x00ff,
        0x0028, 0x0001, 0x0001, 0x0000,
        0x000076ee, 0x000076ee, 0x00010026, 0x000076ee,
        20, 52
    };
    return 0;
}

int fa18_build_run060_frame7991_final_fill(FA18AreaFillPacket *packet) {
    if (!packet) return -1;
    *packet = (FA18AreaFillPacket){
        0x0dfc, 0x0002, 0x00ff, 0x00ff,
        0x0028, 0x0001, 0x0001, 0x0001,
        0x000076ee, 0x00014266, 0x00000037, 0x00014266,
        20, 52
    };
    return 0;
}

int fa18_build_run075_frame559_blit_packets(FA18DisplayBlitPacket packets[3]) {
    static const FA18DisplayBlitPacket recovered[3] = {
        {0x8aea, 0x0053, 0xffff, 0xffff, 0x0028, 0x0004, 0xfdc8, 0x0028,
         0xffff, 0x8000, 0x1e02, 0x6e71, 0x6e71},
        {0xface, 0x0043, 0xffff, 0xffff, 0x0028, 0x0000, 0xfea4, 0x0028,
         0xffff, 0x8000, 0x0d42, 0x6eef, 0x6eef},
        {0x0b4a, 0x0043, 0xffff, 0xffff, 0x0028, 0x0000, 0xfebc, 0x0028,
         0xffff, 0x8000, 0x0dc2, 0x6e58, 0x6e58}
    };
    if (!packets) return -1;
    memcpy(packets, recovered, sizeof recovered);
    return 0;
}
