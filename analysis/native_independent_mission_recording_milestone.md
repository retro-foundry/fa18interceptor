# Independent mission-three recording — 2026-10-09

An independently started native flight now matches the original through a
mission-three takeoff and its first crash/reset boundary. All 326 boundaries
match all 5,216 complete aircraft record cores, camera, controls, selected
record, target and callback state. Release and Debug produce identical complete
traces, final RAM, counters, saved pilots and comparison reports. This is a
failed route; it does not qualify a successful mission-three whole flight.

## Original recording authority

`original_mission_pilot_loop.c` is an external validation controller replacing
the reference runner's loop-input adapter. The original runs from the sealed
qualification menu state with Kickstart and `--ports off`. The sealed
qualification keys remain unchanged. After qualification, ordinary Shift+Escape
returns to the menu; ordinary menu keys select mission three and location two.
The shared validation `MissionPilot` reads original RAM and sends physical keys
through `fa18_machine_key`, the existing keyboard IRQ delivery API. It never
writes game RAM, seeds an objective or changes original clocks.

The controller chooses flight inputs using four nominal host ticks per physics
update, because the original renderer advances PAL time differently. This is
input-generation policy, not a game timing change. `controller-choices.e9k` is
a diagnostic log mixing controller ticks and menu PAL ticks; do not replay it
as a host-frame fixture. `input.fa18in` records the actual physical IRQ edges
at original loop/PAL boundaries. `consumed.fa18in` records the actual game
consumption of those edges and supplies the independent native replay.

The generated input reproduces **all 22,467 complete trace boundaries**, every
consumed key and the complete final RAM in an unmodified original runner without
the validation controller. The original replay exits successfully; the input
generator reports failure because its route crashed. Truncated earlier route
experiments were not accepted as recordings.

Mission three initializes at loop 22,142, C10D8A, tick one. Both games reach
C11788 at loop 22,467, with player XYZ **1,078,497 / 2 / 1,099,871** at that
pre-input boundary. The source pilot was airborne and raised its gear. It
crashed before a valid landing and never lowered the gear or earned a grade.
The final export follows the crash callback and is a distinct observation.

## Match the earned pilot context

The first native probe used the bundled ADF pilot, whose scene level was two
and whose prior grades differed. Original C08EB8 loads the saved scene level
into C458A7; scene-dispatch admission therefore differs. Four NPC slots already
differed at tick one. That probe is rejected for whole-state parity and does not
establish a native NPC fault.

The accepted native comparison first enlists a pilot using the existing ordinary
`region-pilot-enlist.e9k` keys, then cold-loads its actual 78-byte save. Both
pilots qualify through the original consumed qualification controls and enter
mission three at scene level zero, with zero mission grades and completions.
No original RAM initializes native gameplay. All sixteen record cores then
match at every boundary. Complete pilot records differ at offsets 5, 11 and
30–32; naming/date/history bytes are retained, not overwritten or claimed equal.

The tested runtime is the existing `fa18_native` Release/Debug build. Its actual
entry → frontend tick → flight tick → C0EFD4 trace path is exercised with disk
assets and keys; both runs report no CPU or chipset emulation. These new files
are validation tools, remove no game dependency and change no playable code.
Plain Escape retains original restart behavior by the user's direction.

## Strict drawing and limits

259 of 326 complete drawing boundaries match all eight page hashes. The 67
failures span loops 22,286–22,400, beginning with planes 4, 6 and 7 at tick 145.
All drawing and clock/HUD differences remain explicit in the report. Equivalent
cadence assessment of these particular failures is still open; matching flight
cores alone does not accept the pictures. NPC-core, control-byte and page-hash
mutations are each rejected. Reassessment validates the retained evidence hashes.

This proves the full failed flight through its first crash/reset callback,
not a mission victory, landing, result-menu return, every arena byte or all six
independent successful mission flights. Prior repeated individual mission
success remains separate evidence. The user waived an uninterrupted campaign;
that requirement remains dropped. Recorded sound/filtering, visible performance
and named-state cleanup remain open.

## Reproduction

```powershell
python tools/native/check_original_mission_recording.py --out build/native-flight/original-mission-three-closed
python tools/native/check_recorded_original_mission_trace.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-closed --out build/native-flight/original-mission-three-release
python tools/native/check_recorded_original_mission_trace.py --runner build/native-cmake/native/Debug/fa18_native.exe --source-evidence build/native-flight/original-mission-three-closed --out build/native-flight/original-mission-three-debug
```

The original checker supports `--reuse-driver` to independently verify an
already closed recording. The native checker supports `--assess-existing` for
hash-checked reassessment. Passing raw replay RAM uses temporary storage;
reusable original RAM and unresolved native drawing evidence are compressed.
The default 512 MiB capture limit and build pruner remain enabled. No game
source changes or unrelated CTest reruns were needed. See
`figures/native_independent_mission_recording_checkpoint.json` for fingerprints.
