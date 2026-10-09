# Independent mission-three success recording — 2026-10-09

An original flight now completes mission three with ordinary controls: approach
the normally spawned aircraft, receive the objective, lower gear, land on the
runway, stop, earn grade one and return to the menu. The 27,110-boundary recording
reproduces every trace byte, consumed control and final RAM in the unmodified
original runner without the validation controller. Independently started native
Release and Debug also earn grade one and return to the menu with no reset.
Their complete traces, final RAM, counters and actual saved pilots are identical.

**Whole-flight state parity is not yet accepted.** The first 360 boundaries
match all 5,760 complete cores and camera/control/target state. At loop 22,502,
the original recording repeats the preceding tick 360, every observed field,
all sixteen cores and all eight page hashes; native advances to tick 361.
The strict checker fails and retains the entire subsequent diagnostic.

## Input generation and runtime use

The external reference controller remains ordinary physical keyboard input
through `fa18_machine_key`. Original RAM, clocks, physics, eligibility and grades
are never written by it. The qualification prefix remains the sealed input.
After its original menu return, the controller selects mission three/location
two and uses four nominal input ticks per physics update. This is test input
policy, not a new game clock. The controller log has nominal ticks; the physical
`input.fa18in` and actual `consumed.fa18in` remain the authoritative recordings.

The existing `MissionPilot` now has an opt-in `patrol_flight` validation profile.
It reuses the level-turn approach, observes the aircraft without firing, and
returns at a controlled height through turns. Its final waypoint targets the
observed runway home height beyond the starting position. Carrier-wire approach
input was unsuitable for this runway. All outputs are ordinary keys. Other
profiles retain zero for this flag; their algorithms are unchanged.

The native diagnostic opens the existing frontend and links
`fa18_native_runtime`; its entry → frontend tick → flight tick path is exercised.
It replays actual original qualification/menu consumption, then runs the same
nominal input controller. Its separate observation object never changes the
runtime clock. It records physical host ticks, renders audio as the canonical
runner does, and waits for the actual grade/save after stopping. A stopped plane
alone is not treated as an earned grade.

`check_recorded_patrol_input.py` independently enlists both native pilots through
the existing keys. The canonical executable must replay the original prefix and
new physical controls and earn exactly the diagnostic's 78-byte saved record.
Only the replay end bound is extended; original qualification/menu control rows
remain identical. Release and Debug agree on controller inputs, counters and
earned saves. This exercises the changed input path in the playable executable,
not merely a library. The executable is unchanged:
`55b2fd45a853fddf76d3aa35ad3632b45d796c06f3036c42f49ab829c59edc23`.

The default-profile mission-three source comparison still passes 36 input/stage
intervals, 184 bodies, all 96 landing bodies and the actual config write. The
earlier failed-flight trace reassessment still passes its complete core/control
gate. Python compilation and diff checks pass. No gameplay source, Escape
mapping, protected build checker or recording is modified.

## Original success and independent replay

Original mission initialization is loop 22,142, tick one. Actual original cores
show airborne at 22,289, raised gear while airborne at 22,291, lowered gear while
airborne at 23,929 and touchdown at 26,060. Grade/completion become one, and the
original returns to C0FCB4 at loop 27,110. Reference replay proves the controller
changed execution only through its recorded keys.

The independent native runs receive all 1,426 original consumed edges and a
separate intro key. Their saved context agrees with the original on qualification,
grade, completion count, level and final menu/player phase. Complete pilot records
retain differences in date/name/history bytes; they are not claimed equal.
Native reports no CPU/chipset emulation, no queued input and no reset.

Two unsuccessful input experiments remain separate evidence. Three nominal
ticks per update crashed at loop 22,673. The first patrol return reached the
objective but landed away from the runway and earned no grade through 40,000
boundaries. Both recordings reproduced exactly without their controllers. They
are not success fixtures or evidence of a native runtime defect.

## First repeated boundary and limits

The source's 22,501/22,502 observations are identical apart from their PAL
timestamps (20,052/20,053). Five original and five fresh native RAM snapshots at
22,500–22,504 independently reproduce every corresponding trace field, core and
page hash. Both retain POST_INPUT_AUX=1 and POST_INPUT_EVENT=0 at the repeat.

The next investigation is the reference entry hook in `fa18_ports_enter`
(`port/recomp/recomp_ports.c`): it counts C0EFD4 dispatches and suppresses a known
first-instruction deadline resumption. An interrupt resumption at the same entry
may need separate accounting. This is a hypothesis, not a confirmed recorder
fault or an accepted reason to discard a frame. No source/native clock, game
counter, physics state, input alignment or pixels were altered to pass the gate.

The complete fixed-offset comparison covers all 4,967 observed flight boundaries:
64,996/79,472 cores match, 360 complete-core boundaries match, 361 camera/control
boundaries match and 192 complete drawings match. Strict drawing first differs
at loop 22,286, planes 4/6/7, as in the earlier failed route. Clock/HUD and later
state differences remain explicit. The source's two final menu observations
(27,109–27,110) are outside native flight-only tracing; actual native final RAM
and menu counters are checked separately.

Successful outcomes do not prove the entire state sequence. Independent complete
flights for all six missions, drawing/cadence assessment, recorded sound/filter,
visible performance and named-state cleanup remain open. The user's waived
uninterrupted-campaign requirement stays dropped. Original Escape remains intact.

## Reproduction

```powershell
cmake --build build/native-cmake --config Release --target fa18_native_recorded_patrol_input_test
python tools/native/check_recorded_patrol_input.py --test build/native-cmake/native/Release/fa18_native_recorded_patrol_input_test.exe --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-closed --out build/native-flight/recorded-patrol-input-release
python tools/native/check_original_mission_recording.py --patrol-input --out build/native-flight/original-mission-three-patrol-runway
python tools/native/check_recorded_original_mission_trace.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --out build/native-flight/original-mission-three-success-release
```

The last command intentionally exits nonzero until whole-flight state parity is
established. Its complete compressed traces and report are retained before the
gate. Debug repeats the same checks; `--assess-existing` verifies retained hashes.
The default 512 MiB capture limit and 4 GiB pruner remain enabled. Raw passing RAM
is temporary; reusable original and unresolved native RAM remain compressed.
Fingerprints are in `figures/native_independent_mission_success_checkpoint.json`.
