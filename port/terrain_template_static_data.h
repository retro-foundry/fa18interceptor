#ifndef FA18_TERRAIN_TEMPLATE_STATIC_DATA_H
#define FA18_TERRAIN_TEMPLATE_STATIC_DATA_H

#include <stddef.h>
#include <stdint.h>

#include "hunk.h"

enum {
    FA18_TERRAIN_TEMPLATE_CONTROL_HUNK = 65,
    FA18_TERRAIN_TEMPLATE_CONTROL_RUNTIME_BASE = 0x00c41130,
    FA18_TERRAIN_TEMPLATE_CONTROL_TRANSLATE_OFFSET = 0xc0,
    FA18_TERRAIN_TEMPLATE_CONTROL_TRANSLATE_COUNT = 21,
    FA18_TERRAIN_TEMPLATE_BAND_CONTROL_OFFSET = 0x1bc,
    FA18_TERRAIN_TEMPLATE_GROUP_HUNK = 66,
    FA18_TERRAIN_TEMPLATE_GROUP_RUNTIME_BASE = 0x00c42290,
    FA18_TERRAIN_TEMPLATE_GROUP_DIRECTORY_OFFSET = 0x100,
    FA18_TERRAIN_TEMPLATE_HELPER_HUNK = 8,
    FA18_TERRAIN_TEMPLATE_HELPER_RUNTIME_BASE = 0x00c1c2c8,
    FA18_TERRAIN_TEMPLATE_DELTA_PAIR_OFFSET = 0x149c,
    FA18_TERRAIN_TEMPLATE_DELTA_PAIR_COUNT = 21,
    FA18_TERRAIN_TEMPLATE_SPECIAL_PAIR_OFFSET = 0x15ee,
    FA18_TERRAIN_TEMPLATE_SPECIAL_PAIR_COUNT = 16
};

/* Immutable source payload consumed by `$C1D330-$C1D51D`. Runtime bit gates
 * and the `$C48390` workspace remain caller-owned mutable state. */
typedef struct {
    const uint8_t *band_control;
    size_t band_control_size;
    const int8_t *control_translate;
    size_t control_translate_count;
    const uint8_t *group_bytes;
    size_t group_size;
    size_t group_directory_offset;
    const int8_t *delta_pairs;
    size_t delta_pair_count;
    const uint8_t *special_pairs;
    size_t special_pair_count;
} FA18TerrainTemplateStaticData;

int fa18_load_terrain_template_static_data(const FA18Hunks *hunks,
                                           FA18TerrainTemplateStaticData *data);

#endif
