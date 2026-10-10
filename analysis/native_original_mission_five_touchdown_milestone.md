# Successful independent original mission-five recording — 2026-10-10

The original now completes formation and combat, arrests on its carrier, earns
mission-five grade one and returns to the menu with three actual completions.
All **75,573 observations**, final RAM/register bytes and consumed keys reproduce
exactly in a separate unmodified original run without the validation controller.
The complete earned **47,814-observation** qualification/patrol/escort prefix
also reproduces exactly. Native whole-flight comparison remains separate.

The new explicit `--touchdown-approach` option changes validation keyboard
choices. Once the aircraft reaches the existing standoff region, the controller
requests its observed carrier takeoff height instead of the preceding higher
standoff height. It waits until the aircraft reaches that height, then retains
the same height goal during final. The lower goal remains selected if the
aircraft circles outside the initial standoff region. This avoids the preceding
routes' [high wire crossings](native_original_mission_five_wire_milestone.md).

The controller reads the actual live carrier's C26EBE arrestor geometry and
heading. Its computed target is `(1153000, 119.03125, 1070584)`; the aircraft
touchdown height 119.03125 comes from this flight's takeoff, while the wire
geometry is at 112. Those coordinates are observed outputs, not constants
supplied to gameplay. The original still owns motion, contact, arrest and grade.
No RAM, clock, RNG, eligibility or saved progress is seeded.

Mission-five flight begins at observation **61,797**. All **13,777** observations
through landing, earned result and menu return are retained. These are pre-input
observations, not a newly established executed-body count. The first **70,967**
complete observations remain byte-identical to the preceding curved-final route;
the first difference is 70,968 / PAL 170,604, after unchanged formation, combat
and early return. Independent replay preserves every new recorded key edge in
order and the original game's actual clocks.

| Actual original observation | Result |
| --- | --- |
| 61,862 / PAL 100,335 | Airborne |
| 61,946 / PAL 100,865 | Gear raised after takeoff |
| 68,125 / PAL 149,001 | Gear lowered while airborne before landing |
| 74,401 / PAL 192,847 | Height gate reached at 119.015625, below observed takeoff height 119.03125 |
| 74,635 / PAL 194,837 | Deck touchdown at height 119.03125, contact `8082` |
| 74,641 / PAL 194,903 | Arrest set, contact `C082` |
| 74,693 / PAL 195,307 | Arrested stop, contact `C482`, speed zero |
| PAL 195,375 | Original awards grade one and completion count three, gear down and speed zero |
| 75,573 / PAL 196,009 | Main menu, mode zero and player phase zero |

Wire Z is crossed between observations 74,639 and 74,640 with aircraft height
119.03125 on both sides. The arrested stop is at
`(1152991.53125, 119.03125, 1071085.640625)`. The failed-water diagnostic preserves
the original's subsequent 68-PAL-frame result sequence. Final RAM independently
confirms qualification one, grades `[1, 1, 1]`, completion count three and menu
state; the controller's success message alone is insufficient evidence.

The input option is restricted to mode-five wire input and cannot be combined
with the earlier temporary `--level-final` override. Four CLI/reuse guards and
five external-driver guards reject invalid or mismatched configurations; the
retained preceding report is unchanged. Original compile/link, complete
independent replay, source-prefix checks, Python compilation and clean diffs pass.

The exercised path is `check_original_mission_recording.py -> external
fa18_original_mission_pilot -> mission_pilot_tick -> original keyboard IRQ ->
original game`. `MissionPilot` is validation-only. This batch removes a reference
input limitation; it removes no playable runtime dependency. Native game code,
preallocation and original randomization are unchanged. The playable Release
executable remains SHA-256
`148d58e7d8821b1221ecdf42f1904398d52e32b401480252a78ed39d49b1dac6`.

The [checkpoint](figures/native_original_mission_five_touchdown_checkpoint.json)
binds actual outcomes, landing observations, guards, original media, archived
source/executable bytes and complete replay fingerprints. The recording is in
`build/native-flight/original-mission-five-touchdown-approach`. Trace SHA-256 is
`b7582fd9ccd6975a0ea26c079b4d23ad50de70e8de02617f8f654de7e35c175b`;
final RAM/register SHA-256 is
`40203abe0371c844ae96c7a56e7495cd693ccc9cd37e6543b844c212196bb54c`.
Passing raw temporary captures are removed. Previous failed recordings remain
retained and rejected; the pruner reports a 3.79 GiB build cache within its
4 GiB budget. Protected scripts/allowlist and user settings remain unchanged.

```powershell
python tools/native/check_original_mission_recording.py --mode 5 --source-prefix build/native-flight/original-escort-wire-target --repeat-steering --wait-for-approach-height wire --touchdown-approach --out build/native-flight/original-mission-five-touchdown-approach
```

Next: establish this recording's actual JSR/LINK update identities, then compare
the complete mission with the independently started native game and its
ordinarily earned pilot. Initial clock variation must remain genuine; no fitted
clock, world offset or comparison mask is introduced. Further complete flights
and original sound timing remain open. Campaign continuity remains waived and
named-state cleanup remains outside this goal.
