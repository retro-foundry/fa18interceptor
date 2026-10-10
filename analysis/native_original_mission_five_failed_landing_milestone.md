# Original mission-five failed landing — 2026-10-10

The original recording helper now supports mission five after a verified earned
qualification, patrol and escort prefix. The recorded route completes formation
and combat, then stops away from the carrier without earning a mission-five
grade. It remains rejected as mission-five completion.

The driver closes all **72,650 observations**, including the complete **47,814**
observation escort prefix. Mission-five flight starts at observation **61,797**;
all **10,854** observations through the failed landing are retained. These are
recorded pre-input observations, not a newly established count of executed
update bodies. Actual qualification and both preceding grades remain earned;
completions remain two, mission-five grade remains zero, and the game ends in
mode five, player phase one. Gear was raised after takeoff and lowered before
landing: observations 61,862, 61,946 and 68,125 establish airborne,
gear retracted and gear lowered while still airborne. The final observed aircraft position is
`(1153076.125, 2.9296875, 1124495.015625)`, contact `8080`, speed zero.

Every trace byte, complete final RAM/register export and consumed-key byte
matches a separate run without the controller. All recorded observations
through the landing also match the first attempt's retained partial bytes;
ending the failed input route did not change its preceding execution.
The [checkpoint](figures/native_original_mission_five_failed_landing_checkpoint.json)
binds those results to the recorded controls, original media, executables and
source files. No mission outcome is inferred from the combat-success camera.

`verified_prefix` keeps its strict mission-three default and accepts an
explicit mission-four requirement for this new path. It checks the actual
qualification, all preceding grades, completion count and menu state in the
retained RAM. Escort's parent report, complete earlier trace and consumed keys
must also reproduce exactly. Twenty altered-prefix cases are rejected across
the patrol and escort fixtures; six invalid external-driver configurations
are rejected before pilot output.

The initial attempt kept recording after the aircraft had stopped and exhausted
the default trace budget. Its **521,916,416** retained bytes have no successful
footer or final RAM and remain rejected diagnostic data. The revised test stops
at the observed grounded, zero-speed return without a new completion. Its
80,000-observation maximum fits the default 512 MiB V2 capture budget. Capture
process failures now retain available partial data and explicitly mark it
unaccepted, including failures before a final RAM export exists. An actual
driver process failure verifies that retention and remains rejected, bringing
the rejection/retention checks to 27.

The external path is `fa18_original_mission_pilot -> original IRQ keyboard
queue -> original game`. It uses the existing formation and campaign combat
input controller. The controller's input-choice clock never changes an original
game clock; no coordinates, availability, grades or saved progress are seeded.
Native gameplay, its preallocation and original randomization are unchanged.
The playable executable remains SHA-256
`148d58e7d8821b1221ecdf42f1904398d52e32b401480252a78ed39d49b1dac6`.

This batch removes a validation coverage restriction. It removes no playable
runtime dependency and establishes no new original/native whole-flight parity.
A successful original carrier landing, explicit update-call identities and the
independent native comparison remain further work. Original sound timing stays
open; campaign continuity stays waived and named-state cleanup stays outside
the goal. Passing raw temporary captures are removed and the pruner remains
enabled. Compressed reference RAM, complete controls/reports and the failed
first capture remain under `build/native-flight/`.

```powershell
python tools/native/test_original_mission_prefix.py --prefix build/native-flight/original-mission-three-patrol-runway --out build/native-flight/original-mission-five-prefix-guards/patrol.json
python tools/native/test_original_mission_prefix.py --mode 4 --prefix build/native-flight/original-escort-wire-target --out build/native-flight/original-mission-five-prefix-guards/escort.json
python tools/native/check_original_mission_recording.py --mode 5 --source-prefix build/native-flight/original-escort-wire-target --repeat-steering --out build/native-flight/original-mission-five-water-landing
```

The last command verifies the recording and preserves its failed outcome; a
zero verifier exit does not grant a mission-five grade.
