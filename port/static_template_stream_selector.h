#ifndef FA18_STATIC_TEMPLATE_STREAM_SELECTOR_H
#define FA18_STATIC_TEMPLATE_STREAM_SELECTOR_H

#include <stddef.h>
#include <stdint.h>

#include "template_workspace_append.h"

typedef enum {
    FA18_STATIC_TEMPLATE_STREAM_REJECTED_GROUP,
    FA18_STATIC_TEMPLATE_STREAM_REJECTED_BIT,
    FA18_STATIC_TEMPLATE_STREAM_REJECTED_ROW,
    FA18_STATIC_TEMPLATE_STREAM_EXPANDED
} FA18StaticTemplateStreamRoute;

/* Caller-owned immutable data and one `$600` workspace band at the
 * `$C1D3F4-$C1D51D` boundary. Directory entries are signed word offsets
 * relative to `directory_offset`; selected group records and streams live in
 * the same immutable image. */
typedef struct {
    const uint8_t *immutable_bytes;
    size_t immutable_size;
    size_t directory_offset;
    const uint8_t *bitset_bytes;
    size_t bitset_size;
    const uint8_t *special_pairs;
    size_t special_pair_size;
    uint8_t *workspace;
    size_t workspace_size;
    FA18TemplateWorkspaceAppend *append;
} FA18StaticTemplateStreamInput;

typedef struct {
    uint16_t status_word;
    uint16_t selected_row_index;
    uint16_t expanded_item_count;
    uint16_t append_marker_count;
} FA18StaticTemplateStreamResult;

/* `$C1D3F4-$C1D51D`: select a static stream by group and row key, then copy
 * its bounded items into the supplied mutable workspace band. The append
 * helper is invoked at the original block-change and stream-end boundaries.
 * Invalid source layout and source's non-returning error paths return -1. */
int fa18_select_static_template_stream(
    const FA18StaticTemplateStreamInput *input, int16_t group_selector,
    int16_t row_key, FA18StaticTemplateStreamResult *result,
    FA18StaticTemplateStreamRoute *route);

#endif
