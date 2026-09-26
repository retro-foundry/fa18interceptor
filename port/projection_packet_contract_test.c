#include "projection_packet.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    /* run060 frame 8246, `$C1C5E0` entry with root `$C46184`. */
    const FA18ProjectionRoot root = {0x11982c00, 0x00007708, 0x1059a000};
    const FA18ProjectionInput input = {0, 0x00000500, 0x00001400};
    const FA18ProjectionPacket expected = {-6188, -125, -6580, -31752};
    FA18ProjectionPacket packet;
    if (fa18_publish_projection_packet(&root, input, &packet) != 0 ||
        memcmp(&packet, &expected, sizeof packet) != 0) {
        fputs("run060 projection packet fixture failed\n", stderr);
        return 1;
    }
    if (fa18_publish_projection_packet(NULL, input, &packet) != -1) {
        fputs("projection packet argument contract failed\n", stderr);
        return 1;
    }
    puts("run060 projection packet contract passed");
    return 0;
}
