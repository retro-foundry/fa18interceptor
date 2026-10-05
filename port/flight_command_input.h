#ifndef FA18_FLIGHT_COMMAND_INPUT_H
#define FA18_FLIGHT_COMMAND_INPUT_H

#include "command_input.h"
#include "flight_command_types.h"

/* Command-owned values of an aircraft record. Other flight/scene fields
 * belong to their respective native subsystems. Viewed records are resolved
 * to an ordinary pointer by the owner, including after a child changes view. */
typedef struct {
    uint16_t flags, secondary_flags;
    uint8_t equipment_kind, weapon_radar, stick;
} FA18FlightCommandRecord;

typedef struct {
    FA18CommandInput *commands;
    FA18FlightCommandRecord *player, *viewed, *target;
    FA18FlightCommandRecord *spawn_slots[3];
    uint16_t emitted_requests, command_word, info_page, spawn_gate;
    uint32_t gear_gate;
    uint8_t redraw_e, redraw_b, redraw_c, redraw_d, scale_redraws;
    uint8_t info_request, info_redraws, hud_mode, trim_input;
    uint8_t next_target, script_count, weapon_pause;
    uint8_t weapon_mode_redraws, weapon_redraws, shoot_cue, gear_message;
    uint8_t flare_count, chaff_count, flare_timer, chaff_timer, mission_flags;
    uint8_t ecm_enabled, sequence_phase, eject_flag;
    uint8_t pause, context_started, stick_y, stick_x, space_command_latch;
} FA18FlightCommandState;

typedef struct {
    uint32_t event;
    /* Valid for flare/chaff sound children: the word restored after the
     * sound call, inherited on the empty-count path. */
    int16_t restore_event_word;
    /* Original $C17F8C stack arguments, nearest argument first. */
    int32_t arguments[2];
} FA18FlightCommandChildInput;

typedef struct {
    /* Supply the actual native child owner. It may change state/view pointers
     * before the parent continues. Return 0 on an unimplemented/failed child.
     * The result is explicit game event data, not a CPU register file. */
    int (*consume)(void *context, FA18FlightCommandState *state,
                   enum FlightCommandChild child,
                   const FA18FlightCommandChildInput *input,
                   FlightCommandResult *result);
    void *context;
} FA18FlightCommandOps;

int fa18_is_flight_input_command(enum CommandAction action);

/* All 28 flight actions of $C1AC28/$C1AD74, before queue publication.
 * Requires caller-owned player/view/target/spawn records and child owners.
 * Returns 0 on invalid arguments or child failure; earlier source writes may
 * already have occurred on a child failure. Never invents a child substitute.
 * published_event is assigned only after completing the action. */
int fa18_apply_flight_input_command(FA18FlightCommandState *state,
                                    const CommandRequest *request,
                                    int16_t carried_event_word,
                                    const FA18FlightCommandOps *ops,
                                    uint32_t *published_event);

/* Actual ordinary-state leaves: direction commands ($C1B50C/$C1B558 family),
 * throttle reset ($C1B602) and space release ($C08394).
 * Returns 1 only for a handled child, 0 for another child or invalid pointers.
 * Message/audio and space-press children use the command_effects.c owner.
 * Eject's $C1C214 child includes queue publication, not just its toggle. */
int fa18_apply_flight_control_child(FA18FlightCommandState *state,
                                    enum FlightCommandChild child,
                                    const FA18FlightCommandChildInput *input,
                                    FlightCommandResult *result);

#endif
