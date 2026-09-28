#ifndef FA18_TERRAIN_SELECTOR_ORIGIN_H
#define FA18_TERRAIN_SELECTOR_ORIGIN_H

#include <stddef.h>
#include <stdint.h>

enum { FA18_TERRAIN_SELECTOR_ORIGIN_DIRECT_RECORD_BYTES = 0x50 };

typedef void (*FA18TerrainSelectorOriginMatrixPrepare)(void *context);

/* Caller-owned mutable inputs to `$C29042`.  This is deliberately only the
 * gate-mode-nonzero/detail-mode-zero direct-record lane at `$C2906E-$C291C8`:
 * the matrix-table lane and `$C291D4` continuation stay explicit. */
typedef struct {
    uint8_t origin_enable;
    uint8_t gate_b;
    uint8_t gate_a;
    uint8_t gate_mode;
    uint8_t detail_mode;
    const uint8_t *active_record;
    size_t active_record_size;
    int32_t origin[3];
    FA18TerrainSelectorOriginMatrixPrepare prepare_matrix;
    void *prepare_context;
} FA18TerrainSelectorOriginDirectState;

typedef enum {
    FA18_TERRAIN_SELECTOR_ORIGIN_DIRECT_PUBLISHED,
    FA18_TERRAIN_SELECTOR_ORIGIN_GATE_EXIT,
    FA18_TERRAIN_SELECTOR_ORIGIN_UNPORTED_MATRIX_ROUTE
} FA18TerrainSelectorOriginResult;

/* `$C29042-$C291D3`: invoke the `$C2DAF2` caller boundary, apply the source
 * gates, and publish the direct record triple.  `result` distinguishes the
 * source's no-publish gate exit from its unported matrix/continuation route. */
int fa18_publish_terrain_selector_origin_direct(
    FA18TerrainSelectorOriginDirectState *state,
    FA18TerrainSelectorOriginResult *result);

/* Adapter for `$C1C6BC`'s `$C29042` callback.  It succeeds only when the
 * direct lane publishes a new triple; other source routes remain explicit. */
int fa18_publish_terrain_selector_origin_direct_callback(void *context,
                                                          int32_t origin[3]);

#endif
