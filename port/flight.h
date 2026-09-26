#ifndef FA18_FLIGHT_H
#define FA18_FLIGHT_H

#include <stdint.h>

/* Semantic replacement for the selected 512-byte flight record. Only fields
 * with a proven writer are exposed here; the record layout itself is not
 * recreated. */
typedef struct {
    int32_t lateral;
    int32_t altitude;
    int32_t forward;
    int32_t lateral_delta;
    int32_t vertical_delta;
    int32_t forward_delta;
    int16_t attitude[3][3];
} FA18FlightPose;

/* `$C14D32`: the signed vertical delta is committed to the active pose. */
int fa18_flight_commit_vertical(FA18FlightPose *pose, int32_t delta);

/* `$C25E6E/$C25E72`: publish the two horizontal components produced by the
 * indexed update stage. */
int fa18_flight_publish_horizontal(FA18FlightPose *pose,
                                   int32_t lateral, int32_t forward);

#endif
