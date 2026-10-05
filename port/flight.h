#ifndef FA18_FLIGHT_H
#define FA18_FLIGHT_H

#include <stdint.h>
#include <stddef.h>
#include "field_bytes.h"

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
    uint8_t first_derived;
    uint8_t second_derived;
} FA18Joy0DerivedInput;

typedef struct {
    int32_t first;
    int32_t second;
    int32_t third;
} FA18FlightMotionTerms;

typedef struct {
    int16_t d0;
    int16_t d1;
    int16_t d2;
    int16_t d3;
    int16_t d4;
    int16_t d5;
} FA18FlightTrigState;

typedef struct {
    const uint8_t *bytes;
    size_t byte_count;
} FA18FlightTrigTable;

/* Ordinary fixed-point sine/cosine values, in original three-angle order. */
typedef struct {
    int16_t sine[3], cosine[3];
} FA18TrigTerms;

/* Live original asset data, with the quarter-table position inside it.
 * Reads outside that asset require explicitly bound adjacent field owners.
 * No default trigonometric values or angle normalization are supplied. */
typedef struct {
    const uint8_t *bytes;
    size_t byte_count, quarter_offset;
    const PortFieldByte *before, *after;
    size_t before_count, after_count;
} FA18FlightTrigData;

/* Complete word-angle lookup used by $C2E5F6/$C2E6DA, including signed
 * displacement and doubled-word wrap. Returns -1 for missing source data. */
int fa18_flight_lookup_trig_data(const FA18FlightTrigData *data,uint16_t angle,
                                 int16_t *sine,int16_t *cosine);
int fa18_compose_attitude_terms(const FA18TrigTerms *terms,int16_t output[3][3]);

/* `$C1B410`: decode the packed control byte at the observed `$65` boundary. */
int fa18_flight_update_control_lanes(FA18FlightControlLanes *lanes,
                                     uint8_t packed_control);

/* `$C1B516/$C1B586`: replace one two-bit command field when the observed
 * input gate is active. The field masks and command values are source-level
 * control codes; their physical axis names are intentionally not assumed. */
int fa18_flight_publish_control_field(uint8_t *packed_control,
                                      uint8_t field_mask,
                                      uint8_t command,
                                      int enabled);

/* `$C16F1C`: reproduce the two independent JOY0DAT derived tests. */
int fa18_flight_decode_joy0dat(uint16_t joy0dat,
                               FA18Joy0DerivedInput *derived);

/* Proven run060 frontend direction identifiers to packed command fields. */
int fa18_flight_apply_joystick_direction(uint8_t direction,
                                         int pressed,
                                         uint8_t *packed_control);

/* `$C14B16-$C14B7D`: sign extend three prepared words, scale by four, and
 * retain their negated longword terms for the active record. */
int fa18_flight_scale_motion_words(int16_t first, int16_t second,
                                   int16_t third, FA18FlightMotionTerms *terms);

/* `$C15138`: adjust a signed word pair and return the updated first word. */
int fa18_flight_adjust_signed_word_pair(int16_t first, int16_t second,
                                        int16_t *adjusted_first);

/* Caller-shaped `$C14B26` preparation: adjust the first pair, then scale the
 * resulting first word and the two independent prepared words. */
int fa18_flight_prepare_scaled_motion(int16_t first_word,
                                      int16_t adjustment_word,
                                      int16_t second_word,
                                      int16_t third_word,
                                      FA18FlightMotionTerms *terms);

/* `$C2E514-$C2E5AB`: compose the nine post lookup fixed point words. */
int fa18_flight_compose_attitude_matrix(const FA18FlightTrigState *trig,
                                        int16_t output[3][3]);

int fa18_flight_lookup_sine_cosine(const FA18FlightTrigTable *table,
                                   int16_t angle, int16_t *sine,
                                   int16_t *cosine);
int fa18_flight_lookup_two_sine_cosine(const FA18FlightTrigTable *table,
                                       int16_t first_angle,
                                       int16_t second_angle,
                                       FA18FlightTrigState *trig);

/* `$C2E514` caller boundary: native angles are reduced by three bits before
 * the two pair lookups and matrix composition update pose attitude. */
int fa18_flight_update_attitude(const FA18FlightTrigTable *table,
                                int16_t first_angle, int16_t second_angle,
                                int16_t third_angle, FA18FlightPose *pose);

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
