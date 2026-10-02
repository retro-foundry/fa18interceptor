#ifndef FA18_GLUE_CELL_TEMPLATE_OUTPUTS_H
#define FA18_GLUE_CELL_TEMPLATE_OUTPUTS_H
#include "control_records.h"

/* Read-only CPU-output preparation shared by the cell adapter and its
 * complete selector caller. Domain expansion/filing still executes once. */
typedef struct { uint32_t d0, d1, d6, d7; } CellTemplateOutputs;
void cell_template_outputs_begin(int16_t row, int16_t column, gaddr templates,
                                 gaddr bitmap, gaddr lists, gaddr cursor,
                                 CellTemplateOutputs *outputs);
void cell_template_outputs_end(const CellTemplateOutputs *outputs,
                               const FilingState *filing, int enabled);
#endif
