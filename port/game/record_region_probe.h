#ifndef FA18_GAME_RECORD_REGION_PROBE_H
#define FA18_GAME_RECORD_REGION_PROBE_H

#include "memory.h"

typedef enum {
    REGION_SEGMENT_NORMALIZED,
    REGION_SEGMENT_PROJECTED,
    REGION_SEGMENT_BOUNDED
} RegionSegmentStage;

/* Mathematical observations for the optional translated-caller bridge.
 * No CPU state is accepted or returned by the region probe. */
typedef struct {
    RegionSegmentStage stage;
    uint32_t magnitude;
    int16_t scale;
    uint32_t upper_y;
    uint32_t delta_y;
    int crossed;
    int endpoint_excluded;
} RegionSegmentProbe;

typedef struct {
    uint32_t origin[2];
    uint32_t adjustment[2];
    uint32_t offset[2];
    int16_t adjustment_index;
} RegionShapePosition;

typedef struct {
    uint32_t relative_x, relative_y;
    uint32_t x_product, cross_product;
} RegionShapeEdge;

typedef struct {
    void *context;
    void (*directory)(void *context, gaddr record, uint32_t x, uint32_t y,
                      uint16_t index, int16_t offset);
    void (*remaining)(void *context, int16_t count);
    void (*group)(void *context, gaddr first_vertex, uint16_t crossings);
    void (*segment)(void *context, const RegionSegmentProbe *probe);
    void (*shape)(void *context, uint16_t offset, const int16_t fields[5]);
    void (*position)(void *context, const RegionShapePosition *position);
    void (*distance)(void *context, uint32_t magnitude);
    void (*stream)(void *context, int16_t selector, gaddr stream);
    void (*polygon)(void *context, gaddr polygon);
    void (*edge)(void *context, const RegionShapeEdge *edge);
} RecordRegionProbeHooks;

/* $C2B05A: update the selected record's +4 bits 1 and 2 from directory
 * crossing parity and placed four-edge polygons. */
void probe_record_regions(const RecordRegionProbeHooks *hooks);

#endif
