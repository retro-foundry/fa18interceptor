#ifndef FA18_GAME_TEMPLATE_PLACEMENTS_H
#define FA18_GAME_TEMPLATE_PLACEMENTS_H

#include "control_records.h"

/* Synchronous semantic observations for the temporary CPU adapter. Domain
 * work executes once; the observer owns all register-width/CCR effects. */
enum TemplatePlacementPhase {
    TEMPLATE_HELPERS, TEMPLATE_MARKERS, TEMPLATE_CURSOR,
    TEMPLATE_BAND_TYPE, TEMPLATE_EXPAND_BEGIN, TEMPLATE_EXPAND_END,
    TEMPLATE_BAND_NEXT, TEMPLATE_BUILD_BEGIN, TEMPLATE_BUILD_TYPE,
    TEMPLATE_ORIGIN, TEMPLATE_COUNT, TEMPLATE_CELL, TEMPLATE_SHIFT,
    TEMPLATE_PLACEMENT, TEMPLATE_CONTEXT, TEMPLATE_CELL_END,
    TEMPLATE_BUILD_NEXT, TEMPLATE_BUILD_END, TEMPLATE_REVERSE_BEGIN,
    TEMPLATE_REVERSE_RECORD, TEMPLATE_DESCRIPTOR_FLAGS, TEMPLATE_DESCRIPTOR_WORD,
    TEMPLATE_DESCRIPTOR_OFFSET, TEMPLATE_PACKET_BEGIN, TEMPLATE_PACKET_SECTION,
    TEMPLATE_PACKET_LINK, TEMPLATE_PACKET_MISSING, TEMPLATE_PACKET_ACCEPT,
    TEMPLATE_REVERSE_VISIT, TEMPLATE_CONTROL_END, TEMPLATE_FINISH
};
typedef struct {
    enum TemplatePlacementPhase phase;
    gaddr cursor, output, cell, group, table, other;
    int32_t x, y, z;
    uint32_t value;
    int16_t word, extra;
    uint8_t type, count, ordinal, flags, index, cycle;
    const FilingState *filing;
} TemplatePlacementEvent;
typedef struct {
    void (*observe)(void *context, const TemplatePlacementEvent *event);
    void *context;
} TemplatePlacementObserver;

/* Complete $C1D10C: select static template bands, expand their workspace,
 * build the chosen placement cache, and publish linked descriptor packets
 * and the active control-record list. */
void refresh_template_placements(void);
void refresh_template_placements_observed(const TemplatePlacementObserver *observer);

#endif
