# Native gun approach diagnostics — 2026-10-08

The mode-eight gun approach passes 59 input/stage intervals and 277 sampled
frame bodies against original compared RAM and drawing. This includes 32
bodies with active gun control effects. The flight runs 14,811 scene frames
and records zero gun hits. Gun hit and shoot-down acceptance remain open.
This checkpoint adds validation tools; it changes no playable game behavior.

## Source ownership and scope

The playable caller is `native_frontend_tick` -> `native_flight_tick` ->
`native_scene_draw` -> scene child updates -> `native_control_effects` ->
`advance_main_loop_control_records` (C1518C). Its existing C15688/C159AE
launch and direction owners call C266AE collision through `RA_CHECK_RECORD`,
then the component or face motion owner. C266AE advances pilot-log +60 on its
damage path; the target's third damage hit starts destruction. The missile
owners use separate +64/+68 counters. No hit probability, damage threshold,
motion or timer is changed to make this approach succeed.

The fixture starts from the ADF, unlocks saved-pilot mission availability
through the existing preflight file fixture and uses ordinary menu, throttle,
bounded pull-up, target, weapon-selection and firing keys. It supplies no
flight state, target position, projectile, damage or outcome to the game.
Original instructions execute separately from each captured native before-state.
This proves the sampled compositions and observed misses, not an independent
complete original flight or a successful gun shoot-down. Existing comparison
exclusions are unchanged.

## Diagnostics

```powershell
python tools/native/check_mode_two.py --mode 8 --gun-approach --out build/native-flight/gun-checkpoint
```

`--gun-approach` requires 32 active-projectile samples while allowing misses;
the JSON report records a null hit body when no hit occurred. `--hit --gun`
requires an actual +60 counter advance and fails with this current input
sequence. It has no accepted hit fixture yet. Gun reports do not treat C4FDD2
as a valid first-hit target because the source need not publish it on that path.
Missile checks retain their exact single-hit counter requirement.

Optional `FA18_GUN_TRACE=1` emits read-only target positions, aircraft angles,
controls and projectile velocity/lifetime at bounded intervals. Target-local
coordinates use the full +20/+24/+28 positions and the player matrix; they
do not add coarse map cells again. Telemetry is neither game input nor oracle
input. The unsuccessful experimental feedback pilot was removed; the retained
sequence is fixed ordinary keyboard input. These tools link only into the
validation fixture.

Debug and Release fixture builds pass. Radar-kill, infrared-kill, frontend and
artifact-cleanup regression results are recorded in the compact checkpoint
`analysis/figures/native_gun_approach_checkpoint.json` and local log
`build/native-flight/gun-checkpoint-ctest.log`. Passing RAM is temporary and
deleted after comparison; one before-state is held in memory for hit detection.

Next work is a genuine normal-input gun hit and subsequent destruction, then
successful complete missions and independent full-flight comparisons. Callback
contracts, typed game state, audio fidelity and broader visible/combat
performance remain open. The complete-port goal remains active.
