# Candidate reference and mode-five reset - 2026-10-08

The connected native record update now retains the original candidate scan's
reference record. A fresh normal-key mode-five diagnostic passes all 26
input/stage intervals and all 58 sampled bodies against original compared
RAM/drawing, including the previously failing reset body. The pilot still
crashes; completion count remains 3 and mode-five mission success is unaccepted.

## Source ownership and integration

C26EBE's candidate probe sets A3 at C27142; its terminal level selection sets
A3 at C27504/C2750A. Early paths preserve the incoming reference. The following
record's C241A6/C2436A sight calculation consumes that reference after the
C25B66 pose/dynamics parent returns.

`CandidateUpdateWork.reference_record` now exposes this output in
`port/game/candidate_record_update.c/.h`. The native dynamics composition
initialises it from `DynamicsState.scene`, propagates the returned reference,
and returns the final scene to the `RECORD_UPDATE_POSE` caller's record loop.
The playable path is `native_frontend_tick` -> `native_flight_tick` -> native
record updates -> dynamics -> candidate update -> following record sight tail.
This removes the caller's loss of a real candidate output.

The failed reset's original sight calculation used target slot four at
C46984 and viewer slot fourteen at C47D84. The native caller had retained the
player record at C46184. The source vector normalised to -192/-9/-108; preserving
the candidate reference restores those same results. Normalisation arithmetic,
rounding, collision, motion and timers are unchanged.

## Validation

Debug and Release playable and mission-fixture builds pass. Debug mode-three
mission success and artifact cleanup pass. Seven Release CTests pass:
mission-three success, gun kill, mode five, record expiry, frontend, artifact
policy and artifact cleanup. The Release source mission check still covers
36 input/stage intervals and 184 bodies; the gun-kill regression covers 317
intervals and 275 bodies.

The fresh mode-five comparison includes six gun-hit bodies with seven counter
increments and reset body 8,705, iteration 13,164, C11830, before/after tick
20,352, saved tick 2,810. The actual reset now matches all compared RAM/drawing.
No result config write occurs. Input bytes match the previous diagnostic's
SHA256. Existing comparison masks are unchanged. The validation pilot reads
flight state and emits ordinary keys; playable flight state is never seeded
from reference RAM.

Tracked metadata is `figures/native_candidate_reference_reset_checkpoint.json`.
Local logs are in `build/native-flight/push-reset-validation/`; passing RAM
was compared and removed immediately in temporary capture storage. The old
failed reset remains compressed in
`build/native-flight/mission-five-gun-source-check/failure/` as historical
evidence. Its old native after-state still demonstrates the former mismatch.

Build and regression commands:

```powershell
cmake --build build/native-cmake --config Debug --target fa18_native fa18_native_mission_success_test --parallel 8
cmake --build build/native-cmake --config Release --target fa18_native fa18_native_mission_success_test fa18_native_mode_two_test fa18_native_record_expiry_test --parallel 8
ctest --test-dir build/native-cmake -C Debug -R '^fa18_native_mission_three_success$' --output-on-failure
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_(mission_three_success|gun_kill|mode_five|record_expiry|frontend|artifact_policy)$' --output-on-failure
```

The optional final fixture argument `5` reproduces the failed mode-five flight.
Each exported pair was checked by `native_mode_entry_oracle` or
`native_frame_body_oracle`, built serially with `scripts/build_recomp.py`.
The fixture's failure exit remains expected because the flight does not meet
the mission-success conditions. Original instructions execute from native
before-states; this does not prove independent complete original-flight parity.

## Historical enemy-expiry investigation

The investigation recorded here is now resolved for the normal-key mode-five
combat/reset scenario. The expanded comparison passes all 115 sampled bodies,
including all 63 consecutive combat-window bodies and both enemy-expiry
accounting increments. The normalisation pointer and actual depth-sort output
now reach early model expiry through host renderer state. See
`native_mission_five_combat_reset_milestone.md` for the connected owners,
component checks, regressions and remaining full-port scope. Mode-five mission
success and independent complete original-flight parity remain open.
