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

/* Semantic form of the three bounded control bytes at record offsets
 * `$28-$2A`; axis physical meanings remain intentionally unassigned. */
typedef struct {
    int8_t lane[3];
} FA18FlightControlLanes;

typedef struct {
    int32_t first;
    int32_t second;
    int32_t third;
} FA18FlightMotionTerms;

/* `$C1B410`: decode the packed control byte at the observed `$65` boundary. */
int fa18_flight_update_control_lanes(FA18FlightControlLanes *lanes,
                                     uint8_t packed_control);

/* `$C14B16-$C14B7D`: sign extend three prepared words, scale by four, and
 * retain their negated longword terms for the active record. */
int fa18_flight_scale_motion_words(int16_t first, int16_t second,
                                   int16_t third, FA18FlightMotionTerms *terms);

/* `$C15138`: adjust a signed word pair and return the updated first word. */
int fa18_flight_adjust_signed_word_pair(int16_t first, int16_t second,
                                        int16_t *adjusted_first);

/* `$C14D32`: the signed vertical delta is committed to the active pose. */
int fa18_flight_commit_vertical(FA18FlightPose *pose, int32_t delta);

/* `$C25E6E/$C25E72`: publish the two horizontal components produced by the
 * indexed update stage. */
int fa18_flight_publish_horizontal(FA18FlightPose *pose,
                                   int32_t lateral, int32_t forward);

/* Apply the three already computed local motion terms to the active pose.
 * This models the observed `$14/$18/$1C` commit boundary. */
int fa18_flight_apply_motion_terms(FA18FlightPose *pose,
                                   int32_t lateral_delta,
                                   int32_t vertical_delta,
                                   int32_t forward_delta);

#endif
