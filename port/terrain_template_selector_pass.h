#ifndef FA18_TERRAIN_TEMPLATE_SELECTOR_PASS_H
#define FA18_TERRAIN_TEMPLATE_SELECTOR_PASS_H
#include "terrain_template_cursor_resolver.h"
#include "terrain_template_workspace_pass.h"
typedef struct { const FA18Hunks *hunks; FA18TerrainTemplateCursorState cursor_state; uint8_t *workspace; size_t workspace_size; FA18TemplateWorkspaceAppend *append; } FA18TerrainTemplateSelectorPassInput;
typedef struct { FA18TerrainTemplateCursor cursor; FA18TerrainTemplateWorkspacePassResult workspace; } FA18TerrainTemplateSelectorPassResult;
int fa18_run_terrain_template_selector_pass(const FA18TerrainTemplateSelectorPassInput*,FA18TerrainTemplateSelectorPassResult*);
#endif
