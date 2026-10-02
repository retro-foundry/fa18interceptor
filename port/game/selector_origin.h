#ifndef FA18_GAME_SELECTOR_ORIGIN_H
#define FA18_GAME_SELECTOR_ORIGIN_H
#include "memory.h"

/* C29042's producer and its internal candidate paths. These triples are
 * selector inputs; their physical coordinate meanings remain unassigned. */
enum SelectorOriginChild {
    ORIGIN_PREPARE, ORIGIN_MATRIX_A, ORIGIN_MATRIX_B, ORIGIN_REGENERATE,
    ORIGIN_FALLBACK, ORIGIN_NORMALIZE
};
enum SelectorOriginPhase {
    ORIGIN_BYTE_TEST, ORIGIN_BYTE_COMPARE, ORIGIN_BYTE_PUBLISH,
    ORIGIN_WORD_PUBLISH, ORIGIN_LONG_COMPARE, ORIGIN_THRESHOLD,
    ORIGIN_DIRECT, ORIGIN_ANGLE, ORIGIN_MATRIX_INPUTS, ORIGIN_MATRIX_SAVE,
    ORIGIN_MATRIX_RESULT, ORIGIN_FLOOR, ORIGIN_DELTAS, ORIGIN_DISPATCH,
    ORIGIN_BLEND_SAVE, ORIGIN_BLEND_START, ORIGIN_BLEND_FIRST,
    ORIGIN_BLEND_SECOND, ORIGIN_BLEND_THIRD, ORIGIN_BLEND_LAST,
    ORIGIN_PRESET_SAVE, ORIGIN_PRESET_RESULT, ORIGIN_SMALL_SAVE,
    ORIGIN_SMALL_INPUTS, ORIGIN_SMALL_RESTORE, ORIGIN_COUNTDOWN,
    ORIGIN_STATUS, ORIGIN_REDUCE, ORIGIN_NORMALIZE_SAVE,
    ORIGIN_NORMALIZE_RESULT, ORIGIN_SMOOTH, ORIGIN_ADD, ORIGIN_COMPANION,
    ORIGIN_SCAN_START, ORIGIN_SCAN_TEST, ORIGIN_SCAN_RECORD,
    ORIGIN_SCAN_ACTIVE, ORIGIN_SCAN_SELECTED,
    ORIGIN_SCAN_FINISH, ORIGIN_SCAN_RESULT
};
typedef struct { uint32_t component[3]; } SelectorOriginTriple;
typedef struct {
    /* Magnitude tests carry value/limit/shift; matrix stages carry the
     * source record/table and byte selectors. Observe never supplies game
     * outputs: the domain owns every candidate, origin and policy write. */
    enum SelectorOriginPhase phase;
    gaddr address;
    uint32_t value, previous, parameter;
} SelectorOriginEvent;
typedef struct {
    /* MATRIX children consume three signed word inputs, using the current
     * ORIGIN_DETAIL_INDEX and prepared matrix state. NORMALIZE consumes the
     * reduced delta with the source scale 0x200 and returns its new triple.
     * PREPARE/FALLBACK have no triple input; REGENERATE reads prepared state.
     * The observer is the optional CPU proof adapter, removed in stage F. */
    SelectorOriginTriple (*consume)(void *context, enum SelectorOriginChild child,
                                   const SelectorOriginTriple *inputs);
    void (*observe)(void *context, const SelectorOriginEvent *event);
    void *context;
} SelectorOriginHooks;

void publish_selector_origin(const SelectorOriginHooks *hooks);
void select_origin_control_record(const SelectorOriginHooks *hooks);
void load_origin_candidate_preset(const SelectorOriginHooks *hooks, gaddr preset);
#endif
