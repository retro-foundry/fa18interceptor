#include "flight.h"
#include "run075_trig_asset.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    FA18Joy0DerivedInput derived;
    assert(fa18_flight_decode_joy0dat(0x0000, &derived) == 0);
    assert(derived.first_derived == 0 && derived.second_derived == 0);
    assert(fa18_flight_decode_joy0dat(0x0100, &derived) == 0);
    assert(derived.first_derived == 1 && derived.second_derived == 0);
    assert(fa18_flight_decode_joy0dat(0x0202, &derived) == 0);
    assert(derived.first_derived == 1 && derived.second_derived == 0);
    assert(fa18_flight_decode_joy0dat(0x0002, &derived) == 0);
    assert(derived.first_derived == 0 && derived.second_derived == 1);
    uint8_t packed = 0x01;
    assert(fa18_flight_publish_control_field(&packed, 0x30, 0x20, 1) == 0);
    assert(packed == 0x21);
    assert(fa18_flight_publish_control_field(&packed, 0x30, 0x10, 1) == 0);
    assert(packed == 0x11);
    assert(fa18_flight_publish_control_field(&packed, 0x0c, 0x08, 1) == 0);
    assert(packed == 0x19);
    assert(fa18_flight_publish_control_field(&packed, 0x0c, 0x04, 0) == 0);
    assert(packed == 0x19);
    assert(fa18_flight_publish_control_field(&packed, 0x30, 0x08, 1) < 0);
    FA18FlightControlLanes lanes = {{0, 0, 0}};
    assert(fa18_flight_update_control_lanes(&lanes, 0x21u) == 0);
    assert(lanes.lane[0] == -1 && lanes.lane[1] == 0 && lanes.lane[2] == 0);
    lanes.lane[0] = 20;
    lanes.lane[1] = -20;
    lanes.lane[2] = 60;
    assert(fa18_flight_update_control_lanes(&lanes, 0x54u) == 0);
    assert(lanes.lane[0] == 20 && lanes.lane[1] == 1 && lanes.lane[2] == 60);
    assert(fa18_flight_update_control_lanes(&lanes, 0x08u) == 0);
    assert(lanes.lane[2] == -3);
    FA18FlightMotionTerms terms;
    assert(fa18_flight_scale_motion_words(0x0012, (int16_t)-3, 0x4000,
                                          &terms) == 0);
    assert(terms.first == -0x48 && terms.second == 12 && terms.third == -0x10000);
    int16_t adjusted = 0;
    assert(fa18_flight_adjust_signed_word_pair(10, -3, &adjusted) == 0 &&
           adjusted == 4);
    assert(fa18_flight_adjust_signed_word_pair(-7, 0, &adjusted) == 0 &&
           adjusted == -2);
    assert(fa18_flight_adjust_signed_word_pair(2, 0, &adjusted) == 0 &&
           adjusted == 2);
    assert(fa18_flight_prepare_scaled_motion(10, -3, 18, 0x4000, &terms) == 0);
    assert(terms.first == -72 && terms.second == -16 && terms.third == -0x10000);
    const FA18FlightTrigState trig = {0x1000, 0x2000, 0x3000,
                                      0x4000, 0x5000, 0x6000};
    int16_t attitude[3][3];
    assert(fa18_flight_compose_attitude_matrix(&trig, attitude) == 0);
    assert(attitude[0][0] == -0x5100 && attitude[0][1] == -0x6200 &&
           attitude[0][2] == 0x1800 && attitude[1][0] == 0x2800 &&
           attitude[1][1] == 0x3000 && attitude[1][2] == 0x1000 &&
           attitude[2][0] == -0x5c00 && attitude[2][1] == -0x2400 &&
           attitude[2][2] == 0x2000);
    const FA18FlightTrigTable trig_table = {
        fa18_run075_trig_bytes, sizeof fa18_run075_trig_bytes
    };
    int16_t sine = 0;
    int16_t cosine = 0;
    assert(fa18_flight_lookup_sine_cosine(&trig_table, 0, &sine, &cosine) == 0 &&
           sine == 0 && cosine == 0x4000);
    FA18FlightTrigState pair = {0};
    assert(fa18_flight_lookup_two_sine_cosine(&trig_table, 0, 0, &pair) == 0 &&
           pair.d0 == 0 && pair.d1 == 0x4000 && pair.d2 == 0 && pair.d3 == 0x4000);
    FA18FlightPose attitude_pose = {0};
    assert(fa18_flight_update_attitude(&trig_table, 0, 0, 0, &attitude_pose) == 0);
    assert(attitude_pose.attitude[0][0] == -0x4000 &&
           attitude_pose.attitude[1][1] == 0x4000 &&
           attitude_pose.attitude[2][2] == 0x4000 &&
           attitude_pose.attitude[0][1] == 0 && attitude_pose.attitude[1][0] == 0);

    FA18FlightPose pose = {0};
    pose.altitude = 0x72301;
    assert(fa18_flight_commit_vertical(&pose, -0x25b0) == 0);
    assert(pose.altitude == 0x6fd51);
    assert(pose.vertical_delta == -0x25b0);
    pose.lateral = 10;
    pose.forward = 20;
    assert(fa18_flight_publish_horizontal(&pose, 30, 40) == 0);
    assert(pose.lateral == 30 && pose.forward == 40);
    assert(fa18_flight_apply_motion_terms(&pose, -5, -0x20, 7) == 0);
    assert(pose.lateral == 25 && pose.forward == 47 &&
           pose.altitude == 0x6fd31 && pose.lateral_delta == -5 &&
           pose.forward_delta == 7 && pose.vertical_delta == -0x20);
    puts("flight pose contract passed");
    return 0;
}
