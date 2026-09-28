#ifndef FA18_RECORD_WALKER_PREFIX_H
#define FA18_RECORD_WALKER_PREFIX_H

#include <stddef.h>
#include <stdint.h>

typedef struct { int16_t value[3]; } FA18RecordWalkerTriple;

typedef int (*FA18RecordWalkerTripleHandler)(void *context, int16_t selector,
                                             const FA18RecordWalkerTriple triples[3],
                                             int *source_status);
typedef int (*FA18RecordWalkerHexHandler)(void *context, const int16_t words[6],
                                          int *source_status);
typedef int (*FA18RecordWalkerOtherHandler)(void *context, int16_t first_word,
                                            int16_t flags, int *source_status);

typedef struct {
    const uint8_t *control_stream;
    size_t control_stream_size;
    uint32_t control_base;
    uint32_t initial_cursor;
    const uint8_t *vertex_table;
    size_t vertex_table_size;
    uint32_t step_budget;
    FA18RecordWalkerTripleHandler triple_handler;
    FA18RecordWalkerHexHandler hex_handler;
    FA18RecordWalkerOtherHandler other_handler;
    void *context;
} FA18RecordWalkerPrefixInput;

typedef enum {
    FA18_RECORD_WALKER_RETURN_ZERO,
    FA18_RECORD_WALKER_NEGATIVE_CONTROL_EXTERNAL
} FA18RecordWalkerPrefixRoute;

typedef struct {
    uint32_t cursor;
    uint32_t handled_controls;
    uint32_t line_emitter_mask;
    uint32_t next_line_emitter_mask;
} FA18RecordWalkerPrefixResult;

/* `$C1F6F8-$C1F79F`: initialise the source masks and execute the bounded
 * ordinary control branch. Negative non-`$FFFF` controls transfer to the
 * separately ported `$C1F7A0` selector; the caller must compose that route. */
int fa18_run_record_walker_prefix(const FA18RecordWalkerPrefixInput *input,
                                  FA18RecordWalkerPrefixResult *result,
                                  FA18RecordWalkerPrefixRoute *route);

#endif
