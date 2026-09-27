#ifndef FA18_PROJECTION_GRID_H
#define FA18_PROJECTION_GRID_H

#include <stdint.h>

#include "hunk.h"

enum {
    FA18_C279_PROJECTION_GRID_HUNK = 25,
    FA18_C279_PROJECTION_BOUNDS_OFFSET = 0x0354,
    FA18_C279_PROJECTION_BOUNDS_BYTES = 0x0400,
    FA18_C279_PROJECTION_GRID_OFFSET = 0x0754,
    FA18_C279_PROJECTION_GRID_RECORD_BYTES = 6,
    FA18_C279_PROJECTION_PAIR_SOURCE_12_OFFSET = 0x0d0c,
    FA18_C279_PROJECTION_PAIR_SOURCE_16_OFFSET = 0x0d24,
    FA18_C279_PROJECTION_PAIR_SOURCE_20_OFFSET = 0x0d3c,
    FA18_C279_PROJECTION_PAIR_SOURCE_BYTES = 12
};

typedef struct {
    uint16_t record_count;
    int16_t bounds_limit;
    const uint8_t *records;
    const uint8_t *bounds_table;
    const uint8_t *pair_sources[3];
} FA18ProjectionGrid;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t kind;
} FA18ProjectionGridRecord;

typedef struct {
    uint16_t record_count;
    int16_t bounds_limit;
    int16_t grid_x;
    int16_t grid_y;
    int16_t scaled_input;
    uint8_t coordinate_shift;
} FA18ProjectionGridSetup;

/* Literal state writes at `$C279D0-$C27A31`. The source addresses are not
 * promoted to gameplay concepts; the state merely makes its renderer packet
 * boundaries available to the native caller. */
typedef struct {
    int16_t renderer_state_words[4];
    uint16_t renderer_selector;
    uint8_t line_emitter_mode_flag;
    int16_t negative_kind_flag;
} FA18ProjectionGridPacketState;

typedef enum {
    FA18_PROJECTION_GRID_PACKET_MODE_CONTINUATION = 0,
    FA18_PROJECTION_GRID_PACKET_DEPTH_REJECT,
    FA18_PROJECTION_GRID_PACKET_LOWER_RANGE_CONTINUATION,
    FA18_PROJECTION_GRID_PACKET_READY,
    FA18_PROJECTION_GRID_PACKET_ALTERNATE_CONTINUATION
} FA18ProjectionGridPacketRoute;

typedef struct {
    int16_t shifted_x;
    int16_t shifted_y;
    int16_t kind;
    int16_t bound;
} FA18ProjectionGridPreparedRecord;

typedef struct {
    int16_t words[9];
} FA18ProjectionPairMatrix;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t depth;
} FA18ProjectionPairBase;

typedef struct {
    int16_t x;
    int16_t y;
} FA18ProjectionPairInput;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t depth;
} FA18ProjectionPairOutput;

typedef struct {
    int16_t x;
    int16_t y;
} FA18ProjectionPairScreenPoint;

typedef struct {
    FA18ProjectionPairScreenPoint points[3];
} FA18ProjectionTriangle;

typedef struct {
    FA18ProjectionTriangle triangle;
    FA18ProjectionPairScreenPoint direct_pair;
} FA18ProjectionGridEmission;

typedef struct {
    int16_t min_x;
    int16_t max_x;
    int16_t min_y;
    int16_t max_y;
} FA18ProjectionPairBounds;

/* `$C301F6` continues from its reduced extrema through five distinct branch
 * targets. Only the `$C302B6 -> $C2FA7E` branch has a native renderer
 * connection; the other targets remain explicit so they cannot silently turn
 * into a generic line or area fill. */
typedef enum {
    FA18_PROJECTION_PAIR_BOUNDS_RETURN = 0,
    FA18_PROJECTION_PAIR_BOUNDS_C302C4_AXIS_STEP,
    FA18_PROJECTION_PAIR_BOUNDS_C2F66E_HELPER,
    FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION,
    FA18_PROJECTION_PAIR_BOUNDS_C302EC_CONTINUATION,
    FA18_PROJECTION_PAIR_BOUNDS_BLITTER_LINE_C2FA7E
} FA18ProjectionPairBoundsRoute;

/* Caller adapter for the source-proved `$C302B6 -> $C2FA7E` line boundary.
 * The bounds stage has no evidence for the surrounding line-plane state, so
 * it preserves that state as an opaque caller context. */
typedef int (*FA18ProjectionPairLineEmitter)(void *context,
                                             int16_t x0, int16_t y0,
                                             int16_t x1, int16_t y1,
                                             int16_t display_bound_y);

/* `$C2FF48` writes `$8400` to DMACON before entering `$C301F6`. The custom
 * register remains an opaque platform boundary; this callback records or
 * performs precisely that one source-proved write. */
typedef int (*FA18ProjectionPairDmaEmitter)(void *context, uint16_t value);

/* `$C3029E` exposes the temporary `$C456E6` value only to `$C2FA7E`; it is
 * restored immediately after the call. */
typedef int (*FA18ProjectionPairProtectedLineEmitter)(void *context,
                                                      int16_t x0, int16_t y0,
                                                      int16_t x1, int16_t y1,
                                                      uint32_t line_scratch);

/* `$C302C4` calls one of the shared pixel helpers.  The helper-local planar
 * state is outside the bounds continuation, so this preserves it as caller
 * owned state instead of inventing a framebuffer connection. */
typedef int (*FA18ProjectionPairAxisEmitter)(void *context,
                                             int16_t x, int16_t y);

/* `$C27C48`: after the final table record, the source clears byte `$C457A2`.
 * Its owner is outside this packet, so completion remains a required caller
 * boundary rather than a guessed renderer-state field. */
typedef int (*FA18ProjectionGridCompletionEmitter)(void *context);

typedef enum {
    FA18_PROJECTION_PAIR_AXIS_RETURN = 0,
    FA18_PROJECTION_PAIR_AXIS_C2F5F4,
    FA18_PROJECTION_PAIR_AXIS_C2F60A
} FA18ProjectionPairAxisRoute;

/* `$C305AA` only has a complete local contract through its three exits. The
 * `$C305F8` continuation consumes these literal register-derived words. */
typedef enum {
    FA18_PROJECTION_PAIR_RANGE_EQUAL_RETURN = 0,
    FA18_PROJECTION_PAIR_RANGE_C305D6_CONTINUATION,
    FA18_PROJECTION_PAIR_RANGE_C305F8_CONTINUATION
} FA18ProjectionPairRangeRoute;

typedef struct {
    int16_t d0;
    int16_t d1;
    int16_t d3;
    int16_t d4;
    int16_t d5;
    int16_t d6;
    int16_t d7;
    int16_t a1;
} FA18ProjectionPairRangeState;

/* Register image at the shared `$C305F8` entry. */
typedef struct {
    int16_t d3;
    int16_t d4;
    int16_t d5;
    int16_t d6;
    int16_t d7;
    int16_t a1;
    int16_t display_bound_y;
    uint32_t renderer_base_long;
} FA18ProjectionPairBlitterCoreInput;

typedef struct {
    int16_t d0;
    int16_t d1;
    int16_t d2;
    int16_t d3;
    int16_t display_bound_y;
    uint32_t renderer_base_long;
} FA18ProjectionPairBlitterInput;

typedef enum {
    FA18_PROJECTION_PAIR_BLITTER_RETURN = 0,
    FA18_PROJECTION_PAIR_BLITTER_C30668_SUBMIT
} FA18ProjectionPairBlitterRoute;

/* Exact `$C30668` register writes. The source does not write BLTALWM or
 * BLTBPT, so they deliberately do not appear in this boundary. */
typedef struct {
    uint16_t bltcon0;
    uint16_t bltcon1;
    uint16_t bltafwm;
    uint16_t bltadat;
    uint16_t bltbdat;
    uint16_t bltamod;
    uint16_t bltbmod;
    uint16_t bltcmod;
    uint16_t bltdmod;
    uint16_t bltapt_low;
    uint32_t bltcpt;
    uint32_t bltdpt;
    uint16_t bltsize;
} FA18ProjectionPairBlitterWrites;

/* Hardware submission remains caller-owned: `$C30668` writes only this
 * partial register set, while other custom-chip state is inherited. */
typedef int (*FA18ProjectionPairBlitterEmitter)(
    void *context, const FA18ProjectionPairBlitterWrites *writes);

/* `$C30342` reads the first two words of `$C4597C` sequentially into D1/D3. */
typedef struct {
    int16_t saved_first_word;
    int16_t saved_second_word;
    int16_t vertical_value;
    int16_t horizontal_value;
    int16_t display_bound_y;
    uint32_t renderer_base_long;
} FA18ProjectionPairFinalInput;

typedef struct {
    uint32_t offset_long;
    uint32_t lane_long;
    uint32_t lane_copy;
    uint16_t blit_size;
    uint16_t bltcon0;
    uint16_t bltcon1;
    uint32_t bltapt;
    uint32_t bltcpt;
    uint32_t bltdpt;
    uint16_t bltadat;
    uint16_t bltbdat;
    uint16_t bltcdat;
} FA18ProjectionPairFinalState;

typedef int (*FA18ProjectionPairFinalEmitter)(
    void *context, const FA18ProjectionPairFinalState *state);

/* Caller-owned state at the `$C2FF48 -> $C301F6` hardware boundaries. Every
 * callback maps to one source-proved custom-chip or line-emitter operation. */
typedef struct {
    int16_t display_bound_y;
    int16_t vertical_value;
    int16_t horizontal_value;
    uint32_t renderer_base_long;
    uint8_t mode_flag;
    uint32_t saved_line_scratch;
    FA18ProjectionPairDmaEmitter dma_emitter;
    void *dma_context;
    FA18ProjectionPairLineEmitter line_emitter;
    void *line_context;
    FA18ProjectionPairProtectedLineEmitter protected_line_emitter;
    void *protected_line_context;
    FA18ProjectionPairBlitterEmitter blitter_emitter;
    FA18ProjectionPairFinalEmitter final_emitter;
    void *blitter_context;
} FA18ProjectionPairSubmission;

/* Caller-owned renderer boundaries for the complete bounded
 * `$C27AF4-$C27D0F` table traversal.  Negative records enter the existing
 * `$C2FF48` list submission, while the two direct routes retain their distinct
 * pixel-helper callbacks. */
typedef struct {
    const FA18ProjectionPairSubmission *triangle_submission;
    FA18ProjectionPairAxisEmitter primary_emitter;
    FA18ProjectionPairAxisEmitter adjacent_emitter;
    void *emitter_context;
    FA18ProjectionGridCompletionEmitter completion_emitter;
    void *completion_context;
} FA18ProjectionGridSubmission;

typedef int (*FA18ProjectionPairRangeEmitter)(void *context,
                                              int16_t d0, int16_t d1,
                                              int16_t d2, int16_t d3,
                                              int16_t display_bound_y);

typedef struct {
    int16_t d0;
    int16_t d1;
    int16_t d2;
    int16_t d3;
    int16_t d6;
    const FA18ProjectionPairScreenPoint *pairs;
    uint16_t pair_count;
    int16_t display_bound_y;
    int16_t vertical_value;
    int16_t horizontal_value;
    uint32_t renderer_base_long;
} FA18ProjectionPairFinalizationInput;

typedef enum {
    FA18_PROJECTION_PAIR_FINALIZATION_C3029E_FALLBACK = 0,
    FA18_PROJECTION_PAIR_FINALIZATION_SUBMITTED
} FA18ProjectionPairFinalizationRoute;

enum {
    FA18_PROJECTION_GRID_SKIP = 0,
    FA18_PROJECTION_GRID_TRIANGLE = 1,
    FA18_PROJECTION_GRID_DIRECT_RENDERER_A = 2,
    FA18_PROJECTION_GRID_DIRECT_RENDERER_B = 3
};

/* `$C27A36-$C27ADA`: bind the `$C28124` table from original Hunk 25 data. */
int fa18_load_projection_grid(const FA18Hunks *hunks, FA18ProjectionGrid *grid);

/* `$C27B20`: decode one three-word input record. */
int fa18_projection_grid_record(const FA18ProjectionGrid *grid, uint16_t index,
                                FA18ProjectionGridRecord *record);

/* `$C27B94`: decode one of the three original pair blocks selected by record
 * kinds `-12`, `-16`, and `-20` through the local `$C286DC/$C286F4/$C2870C`
 * pointer slots. */
int fa18_projection_grid_pair_source(const FA18ProjectionGrid *grid, int16_t kind,
                                     FA18ProjectionPairInput pairs[3]);

/* `$C27A36-$C27ADA` observed normal route. Returns one for the separate
 * original `< -$80` branch, which has not been reconstructed. */
int fa18_prepare_projection_grid(const FA18ProjectionGrid *grid,
                                 int16_t projection_input,
                                 int16_t component_x, int16_t component_y,
                                 FA18ProjectionGridSetup *setup);

/* `$C279D0-$C27A6D`: initialize the observed renderer words, select the
 * zero-mode depth gate, and enter the bounded grid setup. The nonzero-mode,
 * below-`-$800`, and `-$800..-$201` successors have no bounded source body,
 * so they are reported as routes. */
int fa18_initialize_projection_grid_packet(
    const FA18ProjectionGrid *grid, uint8_t packet_mode,
    int32_t projection_component, int16_t projection_input,
    int16_t component_x, int16_t component_y,
    FA18ProjectionGridPacketState *state, FA18ProjectionGridSetup *setup,
    FA18ProjectionGridPacketRoute *route);

/* `$C27C48`: source completion clears the one-byte renderer mode flag. This
 * signature deliberately matches `FA18ProjectionGridCompletionEmitter`. */
int fa18_complete_projection_grid_packet(void *context);

/* `$C27B20-$C27B8E`: prepare one table record before the pair projection.
 * `negative_kind_flag` is the source's `-$22(A6)` word, retained without a
 * promoted semantic name. Returns one when prepared, zero when bounds-cull
 * skips the record, or minus one outside this bounded native model. */
int fa18_prepare_projection_grid_record(const FA18ProjectionGrid *grid,
                                        const FA18ProjectionGridSetup *setup,
                                        uint16_t record_index,
                                        int16_t negative_kind_flag,
                                        FA18ProjectionGridPreparedRecord *record);

/* `$C27B9C-$C27BF0`: project one translated pair through the live sparse
 * `$C45BD8` matrix. This excludes the following cull, perspective divide, and
 * `$C2FF48` polygon submission. */
int fa18_transform_projection_pair(const FA18ProjectionPairMatrix *matrix,
                                   const FA18ProjectionPairBase *base,
                                   const FA18ProjectionPairInput *input,
                                   FA18ProjectionPairOutput *output);

/* `$C27AF4-$C27B1E`: derive the D1/D5/D7 pair bases from the scaled grid
 * input and middle `$C45BD8` matrix column. */
int fa18_prepare_projection_pair_base(const FA18ProjectionPairMatrix *matrix,
                                      int16_t scaled_input,
                                      FA18ProjectionPairBase *base);

/* `$C27BF2-$C27C4D`: cull and perspective-project one matrix result into the
 * `$C4B392` pair-buffer coordinate system. Returns one when accepted, zero
 * when the source culls it, or minus one outside the bounded DIVS model. */
int fa18_project_projection_pair(const FA18ProjectionPairOutput *input,
                                 FA18ProjectionPairScreenPoint *point);

/* `$C27B9C-$C27C4D`: project the three-pair block submitted together at
 * `$C2FF48`. `translation` is the already shifted grid record. */
int fa18_project_projection_triangle(const FA18ProjectionPairMatrix *matrix,
                                     const FA18ProjectionPairBase *base,
                                     const FA18ProjectionGridPreparedRecord *translation,
                                     const FA18ProjectionPairInput pairs[3],
                                     FA18ProjectionTriangle *triangle);

/* `$C27B94-$C27D0F`: route one prepared record to its static three-pair
 * polygon batch or one of the two direct-pair renderer selections. */
int fa18_emit_projection_grid_record(const FA18ProjectionGrid *grid,
                                     const FA18ProjectionPairMatrix *matrix,
                                     const FA18ProjectionPairBase *base,
                                     const FA18ProjectionGridPreparedRecord *record,
                                     int16_t direct_pair_mode_limit,
                                     FA18ProjectionGridEmission *emission);

/* `$C27C62-$C27D0F`: dispatch an accepted direct pair to the original
 * `$C2F5F4` (route A) or `$C2F60A` (route B) renderer boundary. */
int fa18_submit_projection_grid_direct_pair(
    int route, FA18ProjectionPairScreenPoint point,
    FA18ProjectionPairAxisEmitter primary_emitter,
    FA18ProjectionPairAxisEmitter adjacent_emitter, void *emitter_context);

/* `$C27B94-$C27D0F`: submit only the negative-kind three-point result through
 * its observed `$C2FF48` list boundary. Direct-pair renderer selections are
 * returned to the caller unchanged. */
int fa18_submit_projection_grid_record(
    const FA18ProjectionGrid *grid, const FA18ProjectionPairMatrix *matrix,
    const FA18ProjectionPairBase *base,
    const FA18ProjectionGridPreparedRecord *record,
    int16_t direct_pair_mode_limit,
    const FA18ProjectionPairSubmission *submission,
    FA18ProjectionGridEmission *emission,
    FA18ProjectionPairBoundsRoute *bounds_route,
    FA18ProjectionPairFinalizationRoute *finalization_route);

/* `$C27AF4-$C27D0F`: derive the pair base, visit the exact prepared table
 * count, bounds-cull each record, and retain the original negative/direct
 * renderer split.  `setup` must be the matching result of the preceding
 * grid preparation. `submitted_record_count` receives records that reached a
 * renderer boundary; culls and direct-pair row-limit skips are not counted.
 * On a complete pass the required completion emitter performs the source's
 * `$C457A2` byte clear. */
int fa18_submit_projection_grid_pass(
    const FA18ProjectionGrid *grid, const FA18ProjectionGridSetup *setup,
    const FA18ProjectionPairMatrix *matrix, int16_t negative_kind_flag,
    int16_t direct_pair_mode_limit,
    const FA18ProjectionGridSubmission *submission,
    uint16_t *submitted_record_count);

/* `$C301F6-$C30258`: reduce the submitted word-pair list to signed extrema. */
int fa18_reduce_projection_pair_bounds(const FA18ProjectionPairScreenPoint *pairs,
                                       uint16_t count, FA18ProjectionPairBounds *bounds);

/* `$C301F6-$C302C3`: select the post-bounds target using the source's signed
 * word extent tests. `display_bound_y` is the observed `$C45984` word. */
int fa18_select_projection_pair_bounds_route(const FA18ProjectionPairBounds *bounds,
                                             int16_t display_bound_y,
                                             FA18ProjectionPairBoundsRoute *route);

/* Execute only the source-proved `$C302B6 -> $C2FA7E` route. Other selected
 * continuations are reported through `route` without native substitute work. */
int fa18_submit_projection_pair_bounds(const FA18ProjectionPairBounds *bounds,
                                       int16_t display_bound_y,
                                       FA18ProjectionPairLineEmitter line_emitter,
                                       void *line_context,
                                       FA18ProjectionPairBoundsRoute *route);

/* `$C2FF48-$C2FF57`: write DMACON `$8400`, reduce the `$C4B390` tuple list,
 * then enter `$C301F6`. Only its already-proved direct line route is emitted;
 * every other source continuation is returned as `route`. */
int fa18_submit_projection_pair_tuple_list(
    const FA18ProjectionPairScreenPoint *pairs, uint16_t count,
    int16_t display_bound_y, FA18ProjectionPairDmaEmitter dma_emitter,
    void *dma_context, FA18ProjectionPairLineEmitter line_emitter,
    void *line_context, FA18ProjectionPairBoundsRoute *route);

/* `$C3029E-$C302C2`: execute the protected direct-line route. */
int fa18_submit_projection_pair_protected_line(int16_t d0, int16_t d1,
                                                int16_t d2, int16_t d3,
                                                uint8_t mode_flag,
                                                uint32_t saved_line_scratch,
                                                FA18ProjectionPairProtectedLineEmitter emitter,
                                                void *emitter_context);

/* `$C302C4-$C302DD`: advance the source row and choose its shared-renderer
 * call. `value_x`, `endpoint_x`, and `remaining_distance` are D0, D2, and D6
 * at entry. The passed `advanced_y` is the incremented D1. */
int fa18_submit_projection_pair_axis_step(int16_t value_x, int16_t endpoint_x,
                                          int16_t y, int16_t remaining_distance,
                                          int16_t display_bound_y,
                                          FA18ProjectionPairAxisEmitter first_emitter,
                                          FA18ProjectionPairAxisEmitter second_emitter,
                                          void *emitter_context,
                                          int16_t *advanced_y,
                                          FA18ProjectionPairAxisRoute *route);

/* `$C305AA-$C305D5`: select the equal, unsigned-low, or unequal pair-range
 * exit. On the `$C305F8` path, `state` holds the source's prepared words. */
int fa18_prepare_projection_pair_range(int16_t d0, int16_t d1,
                                       int16_t d2, int16_t d3,
                                       int16_t display_bound_y,
                                       FA18ProjectionPairRangeState *state,
                                       FA18ProjectionPairRangeRoute *route);

/* `$C305AA-$C306B3`: join either unequal-pair exit to the shared prepared
 * blitter write set; an equal pair returns without a blitter submission. */
int fa18_submit_projection_pair_range(int16_t d0, int16_t d1,
                                      int16_t d2, int16_t d3,
                                      int16_t display_bound_y,
                                      uint32_t renderer_base_long,
                                      FA18ProjectionPairBlitterWrites *writes,
                                      FA18ProjectionPairRangeRoute *range_route,
                                      FA18ProjectionPairBlitterRoute *blitter_route);

/* `$C305D6-$C306B3`: derive the unequal-pair line state and output the exact
 * prepared-blitter writes. A zero divisor maps the source's divide exception
 * to a native failure. */
int fa18_prepare_projection_pair_blitter(const FA18ProjectionPairBlitterInput *input,
                                         FA18ProjectionPairBlitterWrites *writes,
                                         FA18ProjectionPairBlitterRoute *route);

/* `$C305F8-$C306B3`: shared suffix used by both `$C305AA` continuations. */
int fa18_prepare_projection_pair_blitter_core(
    const FA18ProjectionPairBlitterCoreInput *input,
    FA18ProjectionPairBlitterWrites *writes,
    FA18ProjectionPairBlitterRoute *route);

/* Join `$C305AA`'s high-range output directly to its `$C305F8` successor. */
int fa18_submit_projection_pair_range_blitter(
    const FA18ProjectionPairRangeState *state, int16_t display_bound_y,
    uint32_t renderer_base_long, FA18ProjectionPairBlitterWrites *writes,
    FA18ProjectionPairBlitterRoute *route);

/* `$C30342-$C3040B`: prepare the final lane workspace and exact `$09F0` job
 * after the preceding `$C305AA` pair submissions have returned. */
int fa18_finalize_projection_pair_blit(const FA18ProjectionPairFinalInput *input,
                                       FA18ProjectionPairFinalState *state);

/* `$C302E6-$C3040B`: walk adjacent list pairs plus the closing pair through
 * `$C305AA`, then prepare the final lane submission. */
int fa18_finalize_projection_pair_list(
    const FA18ProjectionPairFinalizationInput *input,
    FA18ProjectionPairRangeEmitter range_emitter, void *emitter_context,
    FA18ProjectionPairFinalState *state,
    FA18ProjectionPairFinalizationRoute *route);

/* Compose `$C301F6`'s two far exits with `$C302DE/$C302EC -> $C3040B`.
 * A non-far result is reported through `bounds_route` without work. The
 * protected `$C3029E` fallback remains explicit through `finalization_route`.
 */
int fa18_submit_projection_pair_far_list(
    const FA18ProjectionPairScreenPoint *pairs, uint16_t count,
    int16_t display_bound_y, int16_t vertical_value, int16_t horizontal_value,
    uint32_t renderer_base_long, uint8_t mode_flag,
    uint32_t saved_line_scratch,
    FA18ProjectionPairProtectedLineEmitter protected_line_emitter,
    void *protected_line_context,
    FA18ProjectionPairBlitterEmitter blitter_emitter,
    FA18ProjectionPairFinalEmitter final_emitter, void *emitter_context,
    FA18ProjectionPairBoundsRoute *bounds_route,
    FA18ProjectionPairFinalizationRoute *finalization_route);

/* `$C2FF48-$C3040B`: complete list dispatcher for the source-proved direct
 * line and far blitter paths. Axis-step and table-helper routes are reported
 * through `bounds_route` but receive no native substitute. */
int fa18_submit_projection_pair_list(
    const FA18ProjectionPairScreenPoint *pairs, uint16_t count,
    const FA18ProjectionPairSubmission *submission,
    FA18ProjectionPairBoundsRoute *bounds_route,
    FA18ProjectionPairFinalizationRoute *finalization_route);

#endif
