#include "flight.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
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
