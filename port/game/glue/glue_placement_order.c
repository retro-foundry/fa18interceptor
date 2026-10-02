/* $C1E540: readable placement ordering with adapter-only CPU results.
 * The semantic observer avoids repeating child reads/writes or executing
 * original instructions to reconstruct outputs. Flags are dead at C1C9AE. */
#include "glue.h"
#include "globals.h"
#include "placement_order.h"

static void placement_outputs(void *context, const PlacementOrderEvent *e) {
    (void)context;
    switch (e->phase) {
    case ORDER_SCAN: SET_W(D(5), e->index); A(1) = e->entry; break;
    case ORDER_ENTRY: A(1) = e->entry; break;
    case ORDER_DESCRIPTOR: A(2) = e->descriptor; D(7) = e->value; break;
    case ORDER_DESCRIPTOR_WORD: SET_W(D(7), e->word); break;
    case ORDER_DESCRIPTOR_FLAG:
        SET_W(D(7), e->word); SET_B(D(7), e->attribute & 0x10u); break;
    case ORDER_FIELDS: A(2) = e->record; break;
    case ORDER_PACKED_POSITION:
        D(5) = (uint32_t)(int32_t)(int16_t)D(5);
        D(6) = (uint32_t)(int32_t)(int16_t)D(6);
        D(7) = (uint32_t)(int32_t)(int16_t)D(7); break;
    case ORDER_ANCHOR:
        A(3) = (uint32_t)e->coordinate.x; A(4) = (uint32_t)e->coordinate.y;
        A(5) = (uint32_t)e->coordinate.z; D(7) = 0xC00; break;
    case ORDER_REFERENCE_POINT:
        D(5) = (uint32_t)e->coordinate.x; D(6) = (uint32_t)e->coordinate.y;
        D(7) = (uint32_t)e->coordinate.z; break;
    case ORDER_SPECIAL_BASE: A(3) = e->record; break;
    case ORDER_SPECIAL_HEIGHT:
        A(4) = e->cursor; if (e->attribute) A(2) = CONTROL_RECORDS; break;
    case ORDER_PLANE:
        A(2) = e->record; A(3) = (uint32_t)(int32_t)(int16_t)e->coordinate.x;
        A(4) = e->cursor; A(5) = (uint32_t)(int32_t)(int16_t)e->coordinate.z;
        D(5) = (uint32_t)e->normal[0]; D(6) = (uint32_t)e->normal[1];
        D(7) = (uint32_t)e->normal[2]; break;
    case ORDER_INDEXED_BEGIN: A(2) = e->record; A(4) = e->cursor; break;
    case ORDER_EDGE:
        A(4) = e->cursor; A(5) = e->point;
        D(5) = (uint32_t)e->normal[0]; D(6) = (uint32_t)e->normal[1];
        D(7) = (uint32_t)e->normal[2]; break;
    case ORDER_EDGE_CURSOR: case ORDER_INDEXED_RESTORE: A(4) = e->cursor; break;
    case ORDER_TRIANGLE:
        A(2) = e->record; A(4) = e->cursor; A(5) = e->point;
        /* C1E9CA/CC/CE retain different full-width cross-product high words. */
        D(5) = e->value; SET_W(D(5), e->normal[0]);
        D(7) = (uint32_t)e->normal[0]; SET_W(D(7), e->normal[2]);
        D(6) = (uint32_t)e->normal[2]; SET_W(D(6), e->normal[1]); break;
    case ORDER_TRIANGLE_SIDE:
        SET_W(D(5), e->normal[0]); D(7) = (uint32_t)e->normal[2]; break;
    case ORDER_FINISH_COUNT: SET_W(D(7), e->index); break;
    case ORDER_PARTITION_BEGIN: SET_W(D(6), e->index); A(1) = WORKSPACES; break;
    case ORDER_PARTITION_SETUP: A(2) = e->record; A(3) = WORKSPACES; break;
    case ORDER_COPY: D(5) = e->value; break;
    case ORDER_PARTITION_END:
        A(1) = A(3) = e->cursor; A(2) = e->record; SET_W(D(7), 0xFFFF); break;
    }
}

int glue_C1E540(void) {
    PlacementOrderObserver observer = {placement_outputs, NULL};
    D(5) = 0xFFFFFFFFu;
    order_placement_cache_observed(&observer);
    return glue_return();
}
