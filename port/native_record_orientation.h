#ifndef FA18_NATIVE_RECORD_ORIENTATION_H
#define FA18_NATIVE_RECORD_ORIENTATION_H
#include "flight.h"
#include "native_scene_records.h"

/* Complete C2D970: publish only the inverse matrix from the supplied original
 * angles. Neither stored angles nor the forward matrix change. */
int fa18_publish_native_record_inverse_with_axis(FA18NativeSceneRecord *record,
    const uint16_t angles[3],const FA18FlightTrigData *data,uint32_t *axis);

/* Complete $C2D954: publish the three original angles and both matrices into
 * the actual scene/command record. Secondary flags are preserved. The original
 * table and any read adjacent fields must be live and bound by the caller.
 * Missing data returns 0 with preceding angle/matrix stores retained. */
int fa18_publish_native_record_orientation(FA18NativeSceneRecord *record,
    const uint16_t angles[3],const FA18FlightTrigData *data);
/* Same publication, retaining the original inverse-third-angle sine as the
 * numeric axis carried into the caller's next operation. */
int fa18_publish_native_record_orientation_with_axis(FA18NativeSceneRecord *record,
    const uint16_t angles[3],const FA18FlightTrigData *data,uint32_t *axis);
/* $C2D94E adds the control-bit clear before the same publication. */
int fa18_reset_native_record_orientation(FA18NativeSceneRecord *record,
    const uint16_t angles[3],const FA18FlightTrigData *data);
#endif
