#ifndef FA18_TERRAIN_TEMPLATE_CURSOR_RESOLVER_H
#define FA18_TERRAIN_TEMPLATE_CURSOR_RESOLVER_H
#include <stddef.h>
#include <stdint.h>
#include "terrain_template_static_data.h"
typedef struct { uint8_t append_enable, route_flag, alternate_pack, selector_a, selector_b, map_selector; int32_t guard_long; int16_t row_a, group_a, row_b, group_b; const uint8_t *gate_a,*gate_b,*gate_c; size_t gate_a_size,gate_b_size,gate_c_size; } FA18TerrainTemplateCursorState;
typedef struct { FA18TerrainTemplateStaticData data; const uint8_t *gate; size_t gate_size; int16_t row_term,group_term; uint8_t derived_root; } FA18TerrainTemplateCursor;
int fa18_resolve_terrain_template_cursor(const FA18Hunks*,const FA18TerrainTemplateCursorState*,FA18TerrainTemplateCursor*);
#endif
