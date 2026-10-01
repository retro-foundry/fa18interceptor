#ifndef FA18_GAME_GRID_PROJECTION_PACKET_H
#define FA18_GAME_GRID_PROJECTION_PACKET_H
#include "memory.h"

/* $C279D0: three source-selected grid tables, a sparse matrix projection,
 * and triangle or single/pair-pixel submissions. Observers carry only
 * mathematical intermediates; CPU register replay belongs to glue. */
typedef struct {
    gaddr records;
    int16_t count, bounds_limit, shift, component_x, component_y, scaled_input;
    uint32_t base_products[3];
    int16_t base[3];
} GridProjectionSetup;

typedef struct {
    gaddr next_record;
    int16_t source_x, source_y, source_kind;
    int16_t x, y, bounds_index, bound, kind, shifted_x, shifted_y;
    int admitted;
} GridProjectionRecord;

typedef struct {
    gaddr next_pair, output;
    uint32_t vertical_first_product, vertical, depth_first_product, depth;
    int16_t horizontal_component;
    uint32_t screen_x, screen_y;
    unsigned comparisons;
    int accepted, triangle;
} GridProjectionPoint;

typedef struct {
    void *context;
    void (*gate)(void *context, uint8_t mode, int32_t threshold, int32_t depth);
    void (*setup)(void *context, const GridProjectionSetup *setup);
    void (*record)(void *context, const GridProjectionSetup *setup,
                   const GridProjectionRecord *record);
    void (*point)(void *context, const GridProjectionSetup *setup,
                  const GridProjectionRecord *record, const GridProjectionPoint *point);
    void (*triangle)(void *context);
    void (*pixel)(void *context, int16_t x, int16_t y, int adjacent);
} GridProjectionHooks;

void draw_grid_projection_packet(gaddr frame, const GridProjectionHooks *hooks);
#endif
