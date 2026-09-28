#ifndef FA18_ACTIVE_PLANE_PACKET_H
#define FA18_ACTIVE_PLANE_PACKET_H
#include "blit_job.h"
typedef struct { uint32_t plane_base[4]; uint16_t size_input; uint8_t plane3_select; } FA18ActivePlanePacketInput;
int fa18_build_active_plane_packet(const FA18ActivePlanePacketInput*,FA18BlitOperation[4]);
int fa18_execute_active_plane_packet(const FA18ActivePlanePacketInput*,uint8_t*,size_t);
#endif
