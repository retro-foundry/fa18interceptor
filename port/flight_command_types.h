#ifndef FA18_FLIGHT_COMMAND_TYPES_H
#define FA18_FLIGHT_COMMAND_TYPES_H

#include <stdint.h>

/* Child identities and semantic result words shared with the reference. */
enum FlightCommandChild {
    FLIGHT_EJECT_TOGGLE, FLIGHT_NEXT_TARGET, FLIGHT_RADAR_RANGE,
    FLIGHT_SPACE_PRESS, FLIGHT_SPACE_RELEASE,
    FLIGHT_Y_DOWN, FLIGHT_Y_UP, FLIGHT_Y_RELEASE,
    FLIGHT_X_RIGHT, FLIGHT_X_LEFT, FLIGHT_X_RELEASE,
    FLIGHT_THROTTLE_RELEASE, FLIGHT_THROTTLE_MODE_RELEASE,
    FLIGHT_THROTTLE_MODE, FLIGHT_HOOK, FLIGHT_HOOK_SOUND,
    FLIGHT_WEAPON_ENABLE, FLIGHT_WEAPON_SOUND, FLIGHT_WEAPON_MODE,
    /* Historical names: *_SOUND at $C25704 post cockpit messages, and
     * FLARE_SPAWN at $C17F8C starts sound 6; it creates no entity. */
    FLIGHT_GEAR, FLIGHT_TARGET, FLIGHT_FLARE_SOUND, FLIGHT_FLARE_SPAWN,
    FLIGHT_CHAFF_SOUND, FLIGHT_ECM
};
typedef struct {
    uint32_t event;
    int16_t carried_event_word;
} FlightCommandResult;
#endif
