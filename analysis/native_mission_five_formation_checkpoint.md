# Mode-five formation diagnostic checkpoint - 2026-10-08

The validation-only `5-formation` argument attempts to follow the stolen
aircraft before engaging the enemies. All input uses ordinary frontend keys;
the pilot only reads runtime state. No playable game source, physics, mission
rule or comparison mask changes in this checkpoint.

The original objective is `C0A002`, reconstructed in
`port/game/postflight_scheduler.c`: proximity to the active stolen aircraft
decrements `SCENE_DISPATCH_GATE`, with record restoration after it becomes
negative. Enemy-expiry accounting and the original landing/stopped-aircraft
admission are also required for a successful mission. This pilot has not
reached restoration or successful completion.

## Observed original behavior

With `FA18_MISSION_END_TICK=11600`, the fixture reports an incomplete mission
and returns status 1. Mode remains five, phase zero, no crash reset occurs,
completion count stays 3 and the proximity countdown stays 200. Applying
maximum throttle during the first turn produces a frozen attitude. Contact
bit two toggles around ticks 11,420 through 11,439; its meaning is not inferred
from this observation.

All 24 input/stage intervals and all 36 sampled full bodies match original
compared RAM/drawing with zero differences and no config write. Original
instructions run externally from native before-states. This establishes
agreement at the sampled boundaries, including the early frozen attitude;
independent complete original/native flight parity remains unverified.

The exact consumed keys are retained in
`tools/native/fixtures/mission-five-formation.e9k`. Metadata and report hashes
are in `analysis/figures/native_mission_five_formation_checkpoint.json`.
Local comparison logs and capture metadata remain under
`build/native-flight/mission-five-formation-stall-source/`; passing raw RAM
was temporary and removed. The optional diagnostic adds observation of contact
bit-two transitions. Default mode three and numeric mode five retain their
existing controls and capture policy.

To reproduce the diagnostic, build `fa18_native_mission_success_test` and
invoke it with the ADF, a fresh save directory, output key path, temporary
capture prefix and final argument `5-formation`. Set
`FA18_MISSION_END_TICK=11600` for this bounded checkpoint. Use the existing
`CaptureWorkspace` facility for temporary RAM and
`compare_mission_boundaries` for serial original comparisons; an incomplete
mission return is expected for this diagnostic. The normal acceptance checks
must still pass without that environment variable.

The Release fixture build passes. A fresh bounded run reproduces identical
capture metadata and consumed keys. Release CTest passes mode-three mission
success, mode-five combat/reset and artifact cleanup; the regression log is
`build/native-flight/mission-five-formation-regression.log`. Mode three still
checks its saved result and cold reload; numeric mode five still checks its
original combat and natural reset.

Next, adjust the validation pilot's transition from takeoff to formation and
compare any newly reached restoration boundary against original instructions.
Do not modify flight physics to compensate for a pilot trajectory that the
original reproduces. After restoration, enemy expiry, return, safe landing,
stopped-aircraft admission, saved result and cold reload remain to be accepted.
