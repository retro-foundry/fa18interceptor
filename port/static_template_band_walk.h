#ifndef FA18_STATIC_TEMPLATE_BAND_WALK_H
#define FA18_STATIC_TEMPLATE_BAND_WALK_H
#include <stddef.h>
#include <stdint.h>
typedef int (*FA18TemplateBandSelect)(void*,int16_t,int16_t,uint8_t*,size_t);
typedef struct { const uint8_t *control; size_t control_size; const int8_t *translate; size_t translate_size; const int8_t *delta_pairs; size_t delta_pair_count; int16_t row_term,group_term; uint8_t append_enable; uint8_t *workspace; size_t workspace_size; FA18TemplateBandSelect select; void *context; } FA18TemplateBandWalkInput;
typedef struct { uint16_t status; uint16_t accepted; } FA18TemplateBandWalkResult;
int fa18_walk_static_template_bands(const FA18TemplateBandWalkInput*,FA18TemplateBandWalkResult*);
#endif
