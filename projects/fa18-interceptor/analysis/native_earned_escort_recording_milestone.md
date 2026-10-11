# Independently recorded earned escort prefix ? 2026-10-10

The original now replays qualification and the complete first mission to earn
its pilot progress normally, then enters escort/internal mode four using
ordinary keys. All **44,458 observations**, final RAM/register bytes and
consumed input reproduce exactly in the unmodified original runner without
the automated pilot. Every observation and consumed key through the existing
27,110-boundary successful prefix also matches its earlier sealed recording.

This escort route reaches the original's **FE mission-failure outcome**, at
boundary 44,458/PAL 75,046. Completion count remains one; no escort grade is
earned. This is a validated failed original route, not successful escort
completion or evidence of a native fault. The escort flight begins at 40,643,
leaving 3,816 observations through the failure boundary. Original/native
comparison at actual JSR/LINK identities remains the next check.

`tools/native/original_mission_pilot_loop.c` replaces only the comparison
runner's physical input adapter. `native_frontend_event` there feeds the
original IRQ keyboard queue; `MissionPilot` reads original storage and chooses
normal controls. Neither tool writes pilot/gameplay state or game clocks.
The playable path remains `port/native/main.c -> native_frontend_tick ->
native_flight_tick` in `port/game/native/`. Native execution is unchanged.
The input controller raises gear after takeoff, observed at controller tick
40,596. Its nominal timing is input choice timing, not source clock adjustment.

The prefix verifier binds media, input, complete trace, consumed input, RAM,
actual qualification, earned grade and original replay fingerprints. Eight
counterfeit-prefix controls reject changed recordings, an unverified/different
replay, a truncated input, missing qualification/grade and a false menu.
The generalized comparison still produces exactly its previous complete
mission-three assessment for all 4,967 observations, including the NPC,
control and page mutation rejections. No earlier acceptance is weakened.

The first diagnostic continued after the original FE result and reached its
300-second process timeout. Its partial trace, keys, consumed input and log
are retained under `build/native-flight/original-escort-earned-prefix`, with
`failure.json`; partial data has no accepted footer/final RAM. The corrected
diagnostic stops at the first actual failure and retains complete output under
`build/native-flight/original-escort-failure-boundary`. The unmodified replay
matches all three execution/input fingerprints. Source RAM is reference
storage only and is never supplied to native gameplay.

Passing raw captures and duplicate replay output use temporary storage.
Reusable trace/RAM evidence remains losslessly compressed, and the standard
4 GiB build pruner remains enabled. The committed
[checkpoint](figures/native_earned_escort_recording_checkpoint.json) binds
reports, artifacts, executable identities and the negative controls.

Successful independent escort coverage, its full drawing assessment, broader
mission coverage and original/native sound timing remain open. Uninterrupted
campaign completion is waived; general named-state cleanup stays outside the
goal.
