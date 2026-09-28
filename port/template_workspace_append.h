#ifndef FA18_TEMPLATE_WORKSPACE_APPEND_H
#define FA18_TEMPLATE_WORKSPACE_APPEND_H
#include <stddef.h>
#include <stdint.h>
typedef struct { uint8_t *first; size_t first_size; uint8_t *second; size_t second_size; uint8_t *out; size_t out_size; uint8_t append_enable; } FA18TemplateWorkspaceAppend;
/* `$C1D520-$C1D5D7`: append matching live-record markers. */
int fa18_append_template_workspace_matches(FA18TemplateWorkspaceAppend*,int8_t match_byte,int16_t match_a,int16_t match_b,size_t *written);
#endif
