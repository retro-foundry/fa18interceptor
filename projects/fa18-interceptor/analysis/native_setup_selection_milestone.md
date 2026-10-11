# Native Free Flight selection milestone, 2026-10-06

The native runner now progresses past the original disk notice into location
and aircraft selection. The prior C1072E guard suppressed the stage that
consumes transient message state 1, leaving it permanently behind the message
printer. Removing that guard and calling the source stage resolves the stall.
The banner is original message $47, **CRACKED BY A-HA**, not a crash report.

The connected path is `port/native/main.c` -> `native_frontend_event/tick` ->
`native_menu_key`, `native_flight_tick`, and `native_setup_stage`. Original
command selection, indexed actions and C1C23C queue publication feed C32CEE's
mode-3 typed input and `check_typed_code`. The supplied disk has an empty
expected code; Return succeeds through the source check, with no native bypass.
C1072E/C1075A/C1078A now reach C10970/C109AC/C10A24. Location selection opens
the aircraft gate through C10AB2/C10AE6. Choosing an aircraft calls C10B1E ->
C10B90 -> reset_scene_recorder/native_control_records_update, then C09192's
root local-to-world preset and C10C08. Original table/constructor state gives
aircraft input and root kind $11. No captured aircraft record is installed.

Repeated C1C63E updates now call source camera-origin normalization (scale
$200), local-to-world transforms with the original matrices, and the original
fallback start-position producer. P uses the source context request and
C11A26/C11A50 pause sequence, returning through C10DAE. C16D04 timer requests
receive seconds/microseconds from the native 50 Hz frame clock; native absolute
time and update cadence do not establish original timing parity.

The native target links the source owners and ordinary data storage directly;
no CPU, ROM, translated instructions, glue or chipset objects are added.
The dependency removed in this batch is the CPU child-call composition for
these selection/camera stages. Typed-state migration and the rest of the
update sequence remain separate work.

Validation:

- `tools/native/check_flight_start.py` exercises menu -> Free Flight -> Return
  -> location 2 -> aircraft 1 -> P pause/resume, checking source stage, gates,
  pose/camera/template banks, aircraft/root kind, timer and ongoing updates.
- `tools/native/check_records.py --reference build/native-flight/source.ram`
  compares the native C1C63E composition with original opcodes at native boot,
  code input, moving-camera and settled aircraft-selection checkpoints plus
  the retained original frame-3035 checkpoint. All five match RAM outside
  original stack scratch C7F000..C7FFFF. This is component evidence, not full
  replay or complete startup-state equivalence.
- Existing intro/menu settled pixels, callsign/save/reload, SDL presentation
  and native link omission checks pass. Native and reference MSVC targets
  build; twelve reference CTests pass. GNU compiles the native record oracle.
- No sealed recordings changed or full replays repeated. Copper fade remains
  excluded from frame comparisons.

Runtime scope: the setup text and keyboard state transitions run, with real
aircraft/root and camera updates. The location/aircraft world preview is still
absent. C10C08 (C10DAE after pause/resume) is not active flight. Remaining work
is source input/view/timer ordering, native world/cockpit drawing, active flight
control/dynamics children, other modes, outcomes/audio, and timing parity.
Estimated Free Flight startup wiring is about 70%, up from 60%; this is a rough
functional estimate, excluding whole-game completeness and gameplay acceptance.
