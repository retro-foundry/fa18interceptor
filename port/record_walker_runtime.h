#ifndef FA18_RECORD_WALKER_RUNTIME_H
#define FA18_RECORD_WALKER_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#include "record_walker_prefix.h"

/* Handler for one `$C1F942` table target.  `record_cursor` is A2 after the
 * selector word, so a source target can consume its own following record. */
typedef int (*FA18RecordWalkerDispatchHandler)(void *context, uint16_t selector,
                                               uint32_t record_cursor,
                                               int16_t *source_status);
typedef int (*FA18RecordWalkerErrorHandler)(void *context, int16_t control_word);

typedef struct {
    const uint8_t *stream_bytes;
    size_t stream_size;
    /* Original address represented by stream_bytes.  A1/A2 and branch words
     * remain source addresses throughout this direct translation. */
    uint32_t stream_base;
    uint32_t initial_cursor; /* A5 at `$C1F708`. */
    uint32_t control_base;   /* `-$2c(a6)`. */
    const uint8_t *vertex_table; /* `$C48390` for the ordinary triple branch. */
    size_t vertex_table_size;
    uint32_t step_budget;
    FA18RecordWalkerTripleHandler triple_handler;
    FA18RecordWalkerHexHandler hex_handler;
    FA18RecordWalkerOtherHandler other_handler;
    FA18RecordWalkerDispatchHandler dispatch_handler;
    FA18RecordWalkerErrorHandler error_handler;
    void *context;
} FA18RecordWalkerRuntimeInput;

typedef enum {
    FA18_RECORD_WALKER_RUNTIME_RETURN_ZERO,
    FA18_RECORD_WALKER_RUNTIME_POST_STREAM_EXTERNAL,
    FA18_RECORD_WALKER_RUNTIME_CONTROL_EXTERNAL,
    FA18_RECORD_WALKER_RUNTIME_EXTENDED_CONTROL_EXTERNAL
} FA18RecordWalkerRuntimeRoute;

typedef struct {
    uint32_t cursor; /* A5 at the selected source boundary. */
    uint32_t a1_cursor;
    uint32_t a2_cursor;
    uint16_t a1_count;
    uint16_t a2_count;
    uint16_t a1_flag;
    uint16_t a2_flag;
    uint16_t record_count;
    uint16_t record_status;
    uint32_t handled_controls;
    uint32_t dispatched_selectors;
} FA18RecordWalkerRuntimeResult;

/* Direct bounded composition of `$C1F6F8-$C1F966`: ordinary walker records,
 * negative control selection (`$C1F7A0`), and negative A2 selector dispatch
 * (`$C1F910`).  `$C1F844`, `$C1F8EC`, and `$C1F94E` stay explicit because
 * their source-owned state/branches have not yet been ported. */
int fa18_run_record_walker_runtime(const FA18RecordWalkerRuntimeInput *input,
                                   FA18RecordWalkerRuntimeResult *result,
                                   FA18RecordWalkerRuntimeRoute *route);

#endif
