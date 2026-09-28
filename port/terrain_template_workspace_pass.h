#ifndef FA18_TERRAIN_TEMPLATE_WORKSPACE_PASS_H
#define FA18_TERRAIN_TEMPLATE_WORKSPACE_PASS_H

#include <stddef.h>
#include <stdint.h>

#include "static_template_band_walk.h"
#include "template_workspace_append.h"
#include "terrain_template_static_data.h"

/* `$C1D330-$C1D51D`: combine immutable template hunks with the live selector
 * terms, bit gates, append records, and `$C48390` workspace. */
typedef struct {
    const FA18TerrainTemplateStaticData *static_data;
    const uint8_t *bitset_bytes;
    size_t bitset_size;
    int16_t row_term;
    int16_t group_term;
    uint8_t append_enable;
    uint8_t *workspace;
    size_t workspace_size;
    FA18TemplateWorkspaceAppend *append;
} FA18TerrainTemplateWorkspacePassInput;

typedef struct {
    FA18TemplateBandWalkResult band_walk;
    uint16_t expanded_band_count;
} FA18TerrainTemplateWorkspacePassResult;

int fa18_build_terrain_template_workspace(
    const FA18TerrainTemplateWorkspacePassInput *input,
    FA18TerrainTemplateWorkspacePassResult *result);

#endif
