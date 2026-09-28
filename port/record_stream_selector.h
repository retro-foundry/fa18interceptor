#ifndef FA18_RECORD_STREAM_SELECTOR_H
#define FA18_RECORD_STREAM_SELECTOR_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    const uint8_t *stream_bytes;
    size_t stream_size;
    uint32_t stream_base;
    uint32_t control_cursor;
    int16_t control_word;
} FA18RecordStreamSelectorInput;

typedef enum {
    FA18_RECORD_STREAM_SELECTOR_RETURN_ZERO,
    FA18_RECORD_STREAM_SELECTOR_DISPATCH,
    FA18_RECORD_STREAM_SELECTOR_POST_STREAM_EXTERNAL
} FA18RecordStreamSelectorRoute;

typedef struct {
    uint32_t next_control_cursor;
    uint32_t a1_cursor;
    uint32_t a2_address;
    uint16_t a1_count;
    uint16_t a2_count;
    uint16_t a1_flag;
    uint16_t a2_flag;
} FA18RecordStreamSelectorResult;

/* `$C1F7A0-$C1F837`: select the source A1/A2 stream pair for the enclosing
 * walker. `stream_base` maps `stream_bytes` to its original address range;
 * direct pointer operands remain original addresses in the result. */
int fa18_select_record_streams(const FA18RecordStreamSelectorInput *input,
                               FA18RecordStreamSelectorResult *result,
                               FA18RecordStreamSelectorRoute *route);

#endif
