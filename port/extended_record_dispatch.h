#ifndef FA18_EXTENDED_RECORD_DISPATCH_H
#define FA18_EXTENDED_RECORD_DISPATCH_H

#include <stddef.h>
#include <stdint.h>

typedef int (*FA18ExtendedRecordTransform)(void *context, int16_t count,
                                           int16_t descriptor_offset);
typedef int (*FA18ExtendedRecordTarget)(void *context, uint16_t selector,
                                        int16_t *source_status);

typedef struct {
    const uint8_t *stream_bytes;
    size_t stream_size;
    uint32_t stream_base;
    uint32_t a2_cursor;
    int16_t initial_word; /* D0 at `$C1F94E`. */
    uint16_t stage_shift; /* `$C45AB8`. */
    int16_t selector_limit; /* `-$28(a6)`. */
    FA18ExtendedRecordTransform transform; /* `$C1F99A`. */
    FA18ExtendedRecordTarget target; /* `$C1F98C`. */
    void *context;
    /* The source transform and following selector can have independent native
     * owners while retaining their exact `$C1F94E` order.  Null uses context. */
    void *transform_context;
    void *target_context;
} FA18ExtendedRecordDispatchInput;

typedef enum {
    FA18_EXTENDED_RECORD_DISPATCH_CONTINUE,
    FA18_EXTENDED_RECORD_DISPATCH_RESTART,
    FA18_EXTENDED_RECORD_DISPATCH_CONTROL_EXTERNAL
} FA18ExtendedRecordDispatchRoute;

typedef struct {
    uint32_t next_a2_cursor;
    uint16_t record_count;
    uint16_t record_status;
    uint16_t selector;
} FA18ExtendedRecordDispatchResult;

/* `$C1F94E-$C1F999`: positive table control, its `$C1F99A` transform call,
 * and the following selector target.  `$C1F8EC` remains an explicit route. */
int fa18_dispatch_extended_record(const FA18ExtendedRecordDispatchInput *input,
                                  FA18ExtendedRecordDispatchResult *result,
                                  FA18ExtendedRecordDispatchRoute *route);

#endif
