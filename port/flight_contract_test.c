#include "flight.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
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
