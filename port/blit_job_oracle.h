#ifndef FA18_BLIT_JOB_ORACLE_H
#define FA18_BLIT_JOB_ORACLE_H

#include "blit_job.h"

/* Captured register packets are test-oracle inputs only; they are intentionally
 * absent from the native runtime source closure. */
int fa18_build_run060_frame7991_area_fill(FA18AreaFillPacket *packet);
int fa18_build_run060_frame7991_final_fill(FA18AreaFillPacket *packet);
int fa18_build_run075_frame559_blit_packets(FA18DisplayBlitPacket packets[3]);

#endif
