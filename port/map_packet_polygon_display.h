#ifndef FA18_MAP_PACKET_POLYGON_DISPLAY_H
#define FA18_MAP_PACKET_POLYGON_DISPLAY_H

#include "map_packet_transform.h"
#include "polygon_display_pipeline.h"

typedef struct {
    const FA18ProjectionPairSubmission *submission;
    FA18PolygonDisplayPipelineResult *result;
} FA18MapPacketPolygonDisplay;

/* `$C2AFE2 -> $C246A0`: adapt the just-produced map triples to the common
 * clip/project/list-submit pipeline. The shift is the `$C4BF90` workspace
 * header, deliberately distinct from the already-applied detail shift. */
int fa18_display_map_packet_polygon(
    void *context, const FA18MapPacketProjectionRecord *records,
    uint16_t record_count, uint16_t coordinate_shift);

#endif
