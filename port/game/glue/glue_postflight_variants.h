#ifndef FA18_GLUE_POSTFLIGHT_VARIANTS_H
#define FA18_GLUE_POSTFLIGHT_VARIANTS_H

#include "postflight_variants.h"

void postflight_tuple_head_registers(void);
void postflight_fixed_head_registers(void);
void postflight_tail_prefix_registers(const PostflightVariantWork *work);
void postflight_tail_select_registers(const PostflightVariantWork *work, int selected);
void postflight_tail_classify_registers(const PostflightVariantWork *work);

#endif
