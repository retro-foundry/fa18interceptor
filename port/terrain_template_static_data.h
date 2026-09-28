#ifndef FA18_TERRAIN_TEMPLATE_STATIC_DATA_H
#define FA18_TERRAIN_TEMPLATE_STATIC_DATA_H

#include <stddef.h>
#include <stdint.h>

#include "hunk.h"

enum {
    FA18_TERRAIN_TEMPLATE_CONTROL_HUNK = 65,
    FA18_TERRAIN_TEMPLATE_CONTROL_RUNTIME_BASE = 0x00c41130,
    FA18_TERRAIN_TEMPLATE_BAND_CONTROL_OFFSET = 0x1bc,
    FA18_TERRAIN_TEMPLATE_GROUP_HUNK = 66,
    FA18_TERRAIN_TEMPLATE_GROUP_RUNTIME_BASE = 0x00c42290,
    FA18_TERRAIN_TEMPLATE_GROUP_DIRECTORY_OFFSET = 0x100
};

/* Immutable source payload consumed by `$C1D330-$C1D51D`. Runtime bit gates,
 * special pairs, and the `$C48390` workspace are intentionally not loaded
 * here: they are mutable parent state. */
typedef struct {
    const uint8_t *band_control;
    size_t band_control_size;
    const uint8_t *group_bytes;
    size_t group_size;
    size_t group_directory_offset;
} FA18TerrainTemplateStaticData;

int fa18_load_terrain_template_static_data(const FA18Hunks *hunks,
                                           FA18TerrainTemplateStaticData *data);

#endif
