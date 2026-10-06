# Native flight controls milestone

The connected caller is `fa18_native` -> frontend tick -> native flight tick ->
source C12098 view controls -> native C1C63E composition -> C22C80 dynamics ->
C1B27E input recording, C13D84 indexed controls and C149BE root motion. These
paths execute existing C game owners instead of CPU/glue or chipset code.

`FlightHooks` now accepts explicit child input values while retaining the
reference adapter's existing observer/consumer contract. Root motion supplies
the original normalization axes and each attenuation operand. Native consumers
use source magnitude/attenuation, region probes, C16D04 time sampling and
C1803C touchdown tones. SDL/replay arrows and throttle punctuation use the
reference host's physical-key mapping and source flight command owners.

The view update allows aircraft selection to complete from C10C08 into C10DAE.
The bounded fixture chooses Free Flight, acknowledges the disk code message,
chooses location 2 / aircraft 1, then presses `=` at frame 6200, releases it at
6600, and presses/releases Up at 6800/6900. At 6500 the aircraft has positive
speed and horizontal motion, with a changed position. At 6850 the Y input is
$10 and its response has ramped to 20; at 7000 both release. This demonstrates
grounded motion and stick recording, not takeoff or sustained flight.

`check_records.py` compares all non-stack RAM after original C12098 and C1C63E
with native composition at frames 1, 4000, 5450, 6100, 6500, 6850 and 7000.
Only source stack and the explicitly identified native temporary frames are
excluded. Original C16D04 executes normally; its external C53C78 timer request
receives the same native frame clock. CPU/ROM are oracle inputs only.

Validation passes: native MSVC build, frontend/link omission and menu checks,
Free Flight setup/pause/resume, seven record/view oracle checkpoints, three
model/scene-parent checkpoints (10/34/8 reached descriptors), and the reference
MSVC build with all twelve reference CTests. GNU oracle builds also pass.

Remaining scope: cockpit/HUD, later flight/dynamics/action children, additional
keyboard actions, complete frame ordering/cadence, takeoff and recorded-run
acceptance. Missing reached children abort. Copper fade is excluded. No full
sealed replay was repeated. The estimate of 95% (previously 90%) applies only
to Free Flight startup wiring; it does not measure whole-game completion.
