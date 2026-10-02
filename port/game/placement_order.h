#ifndef FA18_GAME_PLACEMENT_ORDER_H
#define FA18_GAME_PLACEMENT_ORDER_H

#include "memory.h"

typedef struct { int32_t x, y, z; } PlacementOrderPoint;

/* Semantic observations used by the temporary CPU adapter. The observer
 * owns CPU effects; the domain publishes positions, normals and cursors.
 * Delivery is synchronous and the event pointer lasts only for that call.
 * A NULL observer disables observations. */
enum PlacementOrderPhase {
    ORDER_SCAN, ORDER_DESCRIPTOR, ORDER_DESCRIPTOR_WORD, ORDER_DESCRIPTOR_FLAG,
    ORDER_FIELDS, ORDER_PACKED_POSITION, ORDER_ANCHOR, ORDER_REFERENCE_POINT,
    ORDER_ENTRY, ORDER_SPECIAL_BASE, ORDER_SPECIAL_HEIGHT, ORDER_PLANE,
    ORDER_INDEXED_BEGIN, ORDER_EDGE, ORDER_EDGE_CURSOR, ORDER_INDEXED_RESTORE,
    ORDER_TRIANGLE, ORDER_TRIANGLE_SIDE, ORDER_FINISH_COUNT,
    ORDER_PARTITION_BEGIN, ORDER_PARTITION_SETUP, ORDER_COPY, ORDER_PARTITION_END
};
typedef struct {
    enum PlacementOrderPhase phase;
    gaddr entry, descriptor, cursor, record, point;
    PlacementOrderPoint coordinate;
    int32_t normal[3];
    uint32_t value;
    int16_t index;
    uint16_t word;
    uint8_t attribute;
} PlacementOrderEvent;
typedef struct {
    void (*observe)(void *context, const PlacementOrderEvent *event);
    void *context;
} PlacementOrderObserver;

/* $C1E540-$C1EBAE: classify the selected placement-cache suffix against its
 * last eligible reference, then partition using the renderer workspace.
 * Includes both plane lists and indexed polygon/triangle lists. */
void order_placement_cache(void);
void order_placement_cache_observed(const PlacementOrderObserver *observer);

#endif
