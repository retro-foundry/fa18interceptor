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

    FA18FlightPose pose = {0};
    pose.altitude = 0x72301;
    assert(fa18_flight_commit_vertical(&pose, -0x25b0) == 0);
    assert(pose.altitude == 0x6fd51);
    assert(pose.vertical_delta == -0x25b0);
    pose.lateral = 10;
    pose.forward = 20;
    assert(fa18_flight_publish_horizontal(&pose, 30, 40) == 0);
    assert(pose.lateral == 30 && pose.forward == 40);
    puts("flight pose contract passed");
    return 0;
}
