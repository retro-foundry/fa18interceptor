# Successful independent original escort recording — 2026-10-10

The original now completes escort, arrests on the carrier, earns its actual
escort grade and returns to the menu. All **47,814 observations**, complete
final RAM/register bytes and consumed keys reproduce exactly in an independent
run of the unmodified original runner, without the validation controller.
Qualification and the complete first-mission prefix remain exactly reproduced.
This supplies a successful full original escort reference; native whole-flight
comparison and original sound timing remain separate, open checks.

`--wait-for-approach-height wire` selects a validation input route through the
running original carrier's arrestor triangle. It reads the source C26EBE
carrier class, offsets **488/494/500**, model scaling and actual heading to
compute the triangle centroid. It retains the observed aircraft takeoff height
on that carrier and the existing standoff-height gate. It sends ordinary keys
through the same original keyboard IRQ. No recorded coordinate, source RAM,
clock, RNG seed or physics adjustment is supplied to gameplay.

The input path remains `check_original_mission_recording.py -> external
fa18_original_mission_pilot.exe -> fa18_loop_iteration -> mission_pilot_tick ->
fa18_machine_key`. `MissionPilot` is validation-only, never linked into the
playable runner. Original geometry, touchdown, arrest and grade owners decide
the result. Missing/ambiguous carrier geometry fails explicitly; other input
profiles retain their behavior. Source/executable identities are bound before
launch, including the included controller header.

The live computed destination is `(1136616, 119.03125, 1070584)`, replacing
the controller's takeoff-home destination `(1136684, 119.03125, 1071520)`.
These are observed outputs, not constants in the input controller. This
corrects the missed-wire route established by
[the preceding original recording](native_original_escort_standoff_milestone.md).
The first **43,853** complete observations remain identical to that route;
the first difference is 43,854, after unchanged combat success at 42,816.

| Actual original observation | Result |
| --- | --- |
| 46,879 / PAL 94,289 | Deck touchdown, gear down, hook extended, height 119.03125, contact `8082` |
| 46,885 / PAL 94,356 | Arrest flag set, contact `C082`, position `(1136610.75, 119.03125, 1070633.328125)` |
| 46,934 / PAL 94,778 | Arrested stop on deck, speed zero, contact `C482` |
| 46,943 / PAL 94,852 | Actual escort grade one, completion count two, gear down and speed zero |
| 47,814 / PAL 95,531 | Main menu, mode zero and player phase zero |

The approach-height gate admits height **818.1875** below its actual
**819.03125** target. There are **3,644** repeated held-steering make events.
The final source grade and completion count are checked in actual RAM, not
inferred from the controller's phase or success message alone.

Validation includes the external original compile/link, complete independent
unmodified replay, exact full-prefix checks, Python compilation, clean diffs
and two wrong-target guards. Both mismatched wire/standoff profiles reject
before replay. No playable code or runtime dependency changes; the native
executable remains SHA-256
`36cad74383f5beaad810378f03a973b8a93a9778b906655de112ffad39f80287`.
The unmodified original replay executable remains SHA-256
`9da30b7cd2d0776c2b016077e683b50e9687f5ad43ba5e75c1e69384058c91e7`.

Complete compressed evidence and actual input remain under
`build/native-flight/original-escort-wire-target`. Trace SHA-256 is
`d4cf4cb02b71adf742120166f5195e1765a3ed80ad73ffb3c969d66e71bba5bb`;
final RAM/register SHA-256 is
`b2f55338d766629458ee868db4518c1f391896f53a960f6f5ea9cd35df09ffdf`.
Raw passing captures and duplicate replay files are removed, and the pruner
completes. The [checkpoint](figures/native_original_escort_wire_checkpoint.json)
retains exact outcomes, source identities, guards and evidence hashes.

```powershell
python tools/native/check_original_mission_recording.py --mode 4 --source-prefix build/native-flight/original-mission-three-patrol-runway --repeat-steering --wait-for-approach-height wire --out build/native-flight/original-escort-wire-target
```

Next: establish the successful recording's actual JSR/LINK update identities,
then exercise the current native runner from its independent earned start
through the complete flight with declared input events. Preserve initial clock
variation and all genuine differences in the assessment. Broader complete
flight and sound timing acceptance remain open. Campaign continuity remains
waived; named-state cleanup remains outside this goal.
