#ifndef FA18_RECORD_SCAN_SCALAR_H
#define FA18_RECORD_SCAN_SCALAR_H

#include <stdint.h>

#include "scene_component_magnitude.h"

/* `$C257EC-$C25862`: returns 1 for the source's selector-zero continuation,
 * and -1 for a caller/table fault or source DIVU-by-zero condition. */
int fa18_calculate_record_scan_scalar(const FA18SceneMagnitudeTable *table,
                                      int16_t selector, int16_t x, int16_t y,
                                      int16_t z, int16_t result[3]);

#endif
