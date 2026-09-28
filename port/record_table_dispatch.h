#ifndef FA18_RECORD_TABLE_DISPATCH_H
#define FA18_RECORD_TABLE_DISPATCH_H

#include <stddef.h>
#include <stdint.h>

typedef int (*FA18RecordTableDispatchHandler)(void *context, uint16_t selector,
                                              int16_t *source_status);
typedef int (*FA18RecordDispatchErrorHandler)(void *context, int16_t control_word);

typedef struct {
    const uint8_t *stream_bytes;
    size_t stream_size;
    uint32_t stream_base;
    uint32_t a2_cursor;
    uint16_t record_count;
    uint16_t record_status;
    FA18RecordTableDispatchHandler dispatch_handler;
    FA18RecordDispatchErrorHandler error_handler;
    void *context;
} FA18RecordTableDispatchInput;

typedef enum {
    FA18_RECORD_TABLE_DISPATCH_NEXT_CONTROL_EXTERNAL,
    FA18_RECORD_TABLE_DISPATCH_CONTINUE_EXTERNAL,
    FA18_RECORD_TABLE_DISPATCH_RESTART_EXTERNAL,
    FA18_RECORD_TABLE_DISPATCH_CONTROL_EXTERNAL
} FA18RecordTableDispatchRoute;

typedef struct {
    uint32_t next_a2_cursor;
    uint16_t record_count;
    uint16_t record_status;
    int16_t control_word;
    uint16_t selector;
} FA18RecordTableDispatchResult;

/* `$C1F910-$C1F94D`: dispatch one A2 control/selector word. The source
 * table's resolved target and enclosing walker branches remain caller-owned. */
int fa18_dispatch_record_table_entry(const FA18RecordTableDispatchInput *input,
                                     FA18RecordTableDispatchResult *result,
                                     FA18RecordTableDispatchRoute *route);

#endif
