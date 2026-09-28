#ifndef FA18_SCENE_PLACEMENT_BUILDER_PREFIX_H
#define FA18_SCENE_PLACEMENT_BUILDER_PREFIX_H

#include <stddef.h>
#include <stdint.h>

/* Caller-owned tables and packet fields read by `$C1DC44-$C1DD34`.  The two
 * map tables contain signed bytes; each coordinate table entry is a big-endian
 * word pair.  The mutable workspace is the `$C48390` 16-cell band. */
typedef struct {
    uint8_t type_selector;
    const int8_t *type_map;
    size_t type_map_count;
    const int8_t *type_pair_table;
    size_t type_pair_count;
    int16_t type_bias[2];
    uint8_t placement_selector;
    const int8_t *placement_map;
    size_t placement_map_count;
    const int16_t *workspace_pair_table;
    size_t workspace_pair_count;
    const int16_t *translation_pair_table;
    size_t translation_pair_count;
    uint8_t *workspace;
    size_t workspace_size;
} FA18ScenePlacementBuilderPrefixInput;

typedef struct {
    uint32_t selector_packet;
    /* `$C1DCE4-$C1DCE8`: signed placement-map byte retained in `$C459BC`
     * before the first record tail is emitted. */
    int16_t record_tail_word;
    int32_t workspace_component[2];
    int32_t translation_component[2];
    uint8_t *workspace_cursor;
    uint8_t *workspace_end;
} FA18ScenePlacementBuilderPrefix;

/* `$C1DC44-$C1DD34`: select one of the sixteen 96-byte workspace cells and
 * form the source-scaled coordinate pairs that feed `$C1DD98`. */
int fa18_prepare_scene_placement_builder_prefix(
    const FA18ScenePlacementBuilderPrefixInput *input,
    FA18ScenePlacementBuilderPrefix *output);

#endif
