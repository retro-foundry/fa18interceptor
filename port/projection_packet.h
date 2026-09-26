#ifndef FA18_PROJECTION_PACKET_H
#define FA18_PROJECTION_PACKET_H

#include <stdint.h>

typedef struct { int32_t x, y, z; } FA18ProjectionRoot;
typedef struct { int32_t x, y, z; } FA18ProjectionInput;
typedef struct { int16_t x, y, z; int32_t depth_metric; } FA18ProjectionPacket;

/* `$C1C5E0-$C1C63D`: publish a transformed root-relative tuple. */
int fa18_publish_projection_packet(const FA18ProjectionRoot *root,
                                   FA18ProjectionInput input,
                                   FA18ProjectionPacket *packet);

#endif
