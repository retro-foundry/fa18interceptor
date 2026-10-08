/* Validation-only keyboard pilot; never linked into the playable runner. */
#ifndef FA18_MISSION_PILOT_H
#define FA18_MISSION_PILOT_H
#include "native/frontend.h"
#include <stdio.h>

typedef struct {
    double home[3], forward[3], previous_x, previous_y;
    unsigned scene, phase, target, started, objective, target_press, completions, grade;
    unsigned mode, trace;
    int force_return, formation_started, formation_done;
    int complete_flight, combat_started, return_started;
    int tour_flight;
    int escort_flight, final_flight, final_sequence, rescue_flight, cruise_flight;
    unsigned rescue_drop_tick;
    double rescue_drop_yaw;
    unsigned final_breakaway_tick, final_shot_tick;
    double final_breakaway_yaw;
    unsigned return_input_phase;
    unsigned missile_target, missile_tick, weapon_press;
    int rudder, pitch, roll, throttle, fire, hook;
    FILE *keys;
} MissionPilot;
void mission_pilot_event(MissionPilot *pilot,NativeFrontend *game,int code,int down);
void mission_pilot_tick(MissionPilot *pilot,NativeFrontend *game);
#endif
