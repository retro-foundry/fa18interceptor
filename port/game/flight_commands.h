#ifndef FA18_FLIGHT_COMMANDS_H
#define FA18_FLIGHT_COMMANDS_H
#include "command_selection.h"

/* Shared flight actions reached by C1AC28 and C1AD74. These are internal
 * action bodies, not extra original functions or complete dispatch owners. */
enum FlightCommandChild {
    FLIGHT_EJECT_TOGGLE, FLIGHT_NEXT_TARGET, FLIGHT_RADAR_RANGE,
    FLIGHT_SPACE_PRESS, FLIGHT_SPACE_RELEASE,
    FLIGHT_Y_DOWN, FLIGHT_Y_UP, FLIGHT_Y_RELEASE,
    FLIGHT_X_RIGHT, FLIGHT_X_LEFT, FLIGHT_X_RELEASE,
    FLIGHT_THROTTLE_RELEASE, FLIGHT_THROTTLE_MODE_RELEASE,
    FLIGHT_THROTTLE_MODE, FLIGHT_HOOK, FLIGHT_HOOK_SOUND,
    FLIGHT_WEAPON_ENABLE, FLIGHT_WEAPON_SOUND, FLIGHT_WEAPON_MODE,
    FLIGHT_GEAR, FLIGHT_TARGET, FLIGHT_FLARE_SOUND, FLIGHT_FLARE_SPAWN,
    FLIGHT_CHAFF_SOUND, FLIGHT_ECM
};
enum FlightCommandPhase {
    FLIGHT_BYTE_TEST, FLIGHT_WORD_TEST, FLIGHT_BYTE_STORE, FLIGHT_WORD_STORE,
    FLIGHT_REQUEST_BIT, FLIGHT_TOGGLE_BIT, FLIGHT_TEST_BIT,
    FLIGHT_MODIFIER_TEST, FLIGHT_BYTE_COMPARE,
    FLIGHT_RADAR_RECORD, FLIGHT_RADAR_READ, FLIGHT_RADAR_MASK,
    FLIGHT_RADAR_COMPARE, FLIGHT_RADAR_SELECT,
    FLIGHT_INFO_READ, FLIGHT_INFO_INCREMENT, FLIGHT_INFO_COMPARE,
    FLIGHT_INFO_WRAP, FLIGHT_INFO_FLAG,
    FLIGHT_HUD_READ, FLIGHT_HUD_INCREMENT, FLIGHT_HUD_COMPARE, FLIGHT_HUD_WRAP,
    FLIGHT_INPUT_VALUE, FLIGHT_INPUT_RELEASE, FLIGHT_INPUT_READ,
    FLIGHT_INPUT_MASK, FLIGHT_INPUT_COMBINE,
    FLIGHT_SOUND_SWAP, FLIGHT_SOUND_WORD, FLIGHT_SOUND_CARRY,
    FLIGHT_SOUND_RESTORE, FLIGHT_COUNTER_DECREMENT,
    FLIGHT_WEAPON_READ, FLIGHT_WEAPON_MASK, FLIGHT_WEAPON_DECREMENT,
    FLIGHT_WEAPON_WRAP, FLIGHT_GEAR_READ, FLIGHT_GEAR_MASK,
    FLIGHT_ECM_BEGIN, FLIGHT_TOGGLE_ADDRESS,
    FLIGHT_SPAWN_SAVE, FLIGHT_SPAWN_ARGUMENT, FLIGHT_SPAWN_RESTORE
};
typedef struct {
    uint32_t event;
    int16_t carried_event_word;
} FlightCommandResult;
typedef struct {
    FlightCommandResult (*consume)(void *context, enum FlightCommandChild child);
    void (*observe)(void *context, enum FlightCommandPhase phase,
                    uint32_t value, uint32_t limit, gaddr address);
    void *context;
} FlightCommandHooks;
int is_flight_command(enum CommandAction action);
uint32_t execute_flight_command(const CommandRequest *request,
                               int16_t carried_event_word,
                               const FlightCommandHooks *hooks);
#endif
