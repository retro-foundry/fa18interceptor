# Original escort steering recording — 2026-10-10

An independently started original escort route now reaches the combat success
state, but its return stops beyond the carrier and earns no escort grade.
All **65,000 observations**, final RAM/register bytes and consumed keys reproduce
exactly in a second run of the unmodified original runner. This is a complete
diagnostic recording of an incomplete mission, not successful whole-flight parity.

The change is confined to the external validation input adapter:
`check_original_mission_recording.py -> fa18_original_mission_pilot.exe ->
fa18_loop_iteration -> mission_pilot_tick -> fa18_machine_key -> original
keyboard IRQ`. The adapter reads observations and sends ordinary keys. It
writes no game state, clock, random seed or physics. It is not linked into
the playable native runner; that executable remains SHA-256
`36cad74383f5beaad810378f03a973b8a93a9778b906655de112ffad39f80287`.
No gameplay dependency is removed by this evidence batch.

Optional `--repeat-steering` repeats unchanged nonzero rudder, pitch and roll
make events through the original physical keyboard queue during actual flight.
New steering choices and toggle commands are not duplicated. Without the
option, the existing input policy is unchanged. Retained recordings record
this input choice; attempting to reuse the new recording without the option
rejects before replay. Older metadata without the field retains its explicit
default of no repeated steering.

The recording first reproduces the complete **27,110-observation** earned
qualification/mission-three prefix and its consumed keys exactly. It then
enters escort normally. There are **2,433** repeated steering events. Between
observations 41,500 and 44,458, the previous route has one yaw value and the
new route has **633**. Ammunition first changes at **42,186**; source combat
success stage `C11078` first appears at **42,816 / PAL frame 68,379**.
This establishes the effect of ordinary repeated input, not a diagnosis of
the original input owner's persistence rules.

The final player position is `(1136678.703125, 2.9296875, 1085906.65625)`;
the carrier is `(1136640, 0, 1071104)`. Speed is zero, contact is `8080`,
mode is four, phase is one and completed missions remain one. The controller
never earns the second completion or returns through a successful escort menu.
Its approach turns onto final based on horizontal distance, despite returning
from high altitude. Checking the existing approach-height gate in the external
controller is the next input investigation; the game's landing rules remain
authoritative.

Validation includes the external original runner build, the complete independent
unmodified replay, exact prefix checks, Python compilation and a wrong-input-profile
rejection. Original replay executable SHA-256 is
`9da30b7cd2d0776c2b016077e683b50e9687f5ad43ba5e75c1e69384058c91e7`.
Reports and complete losslessly compressed trace/RAM remain under
`build/native-flight/original-escort-repeated-steering`; raw passing copies and
duplicate replay captures are removed by the existing bounded-storage workflow.
The preceding failed route is preserved unchanged. Hashes and the measured
outcome are retained in
[the checkpoint](figures/native_original_escort_steering_checkpoint.json).

Reproduce the new original recording and independent replay:

```powershell
python tools/native/check_original_mission_recording.py --mode 4 --source-prefix build/native-flight/original-mission-three-patrol-runway --repeat-steering --out build/native-flight/original-escort-repeated-steering
```

Successful original escort landing, independent native whole-flight comparison
and original sound onset/handoffs remain open. Campaign continuity remains waived;
named-state cleanup remains a separate task.
