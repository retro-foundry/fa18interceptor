#ifndef FA18_ACTIVE_RECORD_POSE_COMMIT_H
#define FA18_ACTIVE_RECORD_POSE_COMMIT_H

#include <stdint.h>

/* Direct mutable fields of the selected `$C46184` record touched by the
 * bounded `$C14D0C` and `$C25E6E` writer leaves. */
typedef struct {
    int32_t component_x;
    int32_t component_y;
    int32_t component_z;
    uint8_t class_byte_62;
    uint16_t word_4c;
} FA18ActiveRecordPoseFields;

typedef enum {
    FA18_ACTIVE_RECORD_POSE_C14D68_CONTINUATION,
    FA18_ACTIVE_RECORD_POSE_C14D94_CONTINUATION,
    FA18_ACTIVE_RECORD_POSE_C14DE0_CONTINUATION,
    FA18_ACTIVE_RECORD_POSE_C1505E_CONTINUATION
} FA18ActiveRecordVerticalRoute;

/* `$C14D0C-$C14D93`: commit `record +$18 -= local_delta`, clear bit two of
 * the caller-owned status byte, and expose the source's remaining branch
 * destination. The downstream bodies remain outside this bounded leaf. */
int fa18_commit_active_record_vertical_component(
    FA18ActiveRecordPoseFields *record, int32_t local_delta,
    uint8_t *status_byte, uint8_t secondary_status_byte,
    uint16_t secondary_control_word, FA18ActiveRecordVerticalRoute *route);

typedef enum {
    FA18_ACTIVE_RECORD_POSE_C25E86_CONTINUATION,
    FA18_ACTIVE_RECORD_POSE_CLASS15_CONTINUATION
} FA18ActiveRecordHorizontalRoute;

/* `$C25E6E-$C25E7D`: publish the clamped caller-produced D2/D4 pair into
 * record `+$14/+$1C`; later class-specific work remains an explicit route. */
int fa18_commit_active_record_horizontal_pair(
    FA18ActiveRecordPoseFields *record, int32_t d2, int32_t d4,
    FA18ActiveRecordHorizontalRoute *route);

#endif
