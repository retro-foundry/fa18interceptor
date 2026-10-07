#ifndef FA18_FLIGHT_COMMAND_TYPES_H
#define FA18_FLIGHT_COMMAND_TYPES_H

#include <stdint.h>

/* Action-domain outputs, distinct from the event sent to queue publication.
 * Child-owned outputs remain unresolved unless the actual owner exposes them. */
enum FlightActionOutputKind { FLIGHT_ACTION_UNRESOLVED, FLIGHT_ACTION_PRESERVE,
    FLIGHT_ACTION_HUD_MODE, FLIGHT_ACTION_GEAR_GATE, FLIGHT_ACTION_RADAR_RANGE,
    FLIGHT_ACTION_WEAPON_BLOCK, FLIGHT_ACTION_WEAPON_MODE,
    FLIGHT_ACTION_FIRE_SELECTION, FLIGHT_ACTION_COUNTERMEASURE_EVENT,
    FLIGHT_ACTION_QUEUE_INDEX };
typedef struct {
    enum FlightActionOutputKind kind;
    uint8_t hud_mode,radar_range,weapon_block,weapon_mode,fire_selection;
    uint16_t countermeasure_event;
    uint32_t gear_gate;
    int8_t queue_index;
} FlightActionOutput;

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
    FlightActionOutput output;
} FlightCommandResult;
#endif
