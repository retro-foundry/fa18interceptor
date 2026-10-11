# Native normal mission 3 and aircraft selection

Mission-list F1 now enters normal source mode 3 without recorder playback.
Both source aircraft choices reach C10DAE flight, with 969 scene/HUD frames
by host tick 18000. Complete mission outcomes, weapons/combat sequences,
other variants and whole-game acceptance remain open.

The real caller is `native_frontend_tick` -> `native_flight_tick` ->
`run_post_input_tick` -> C0FECE. The native mode gate previously admitted 3
only with recorder mode 3 (demonstration). It now admits normal mode 3,
reusing the mode-6 briefing/context adapters. Unlike mode 6's preset entry,
normal mode 3 enters the source C10AB2/C10AE6 aircraft-choice prompt.
Actual keyboard 1/2 drives the original indexed-command gate and produces
POSTFLIGHT_FAILURE_INPUT $11/$10; game code owns the scene/aircraft setup.

Selecting either aircraft exposed renderer commands that the native model
owner lacked. Their original relocated C1FCE8 directory entries identify:

- `$118` -> C1FE68: the STREAM_MODE workspace record's +4 word selects a
  script block. Negative values leave the cursor; low bits select $86-byte
  blocks, with eight-byte subentries for values outside 12..116. Readable
  `select_workspace_script_block` retains the source word wrapping and signed
  address increments in the existing cockpit-script module.
- `$11C` -> C0CFB6: successive workspace offset/colour/radius triples draw
  scaled circles until the radius word's high bit marks the last entry.
  `draw_stream_circles` reuses the original circle projector and accumulates
  acceptance and the final-entry flag in its model frame. Native model
  composition calls these domain functions directly.

The initial C0FECE comparison also found the same saved-pointer sort choice
as mode 6. Original tracing records A4=C296EE on entering C1E328, then $96 at
C1E48C's A6-$2C test. Normal mode 3 with recorder mode 0 sorts every list
there; ordinary frame refresh and recorder demonstration keep their existing
choices. Correcting the transition removes the 22 compared RAM differences.

Shared-runtime integration starts only from the ADF and normal keys: Space
1800, digit 6 at 3000, F1 at 4500, Return at 6500 and 8000, then aircraft 1/2
at 14500. Releases follow two ticks later. Tests require recorder mode 0,
the source aircraft selector byte, C10DAE and at least 768 scene frames. They
sample stage changes and flight after 128, 512 and 768 scene frames. No
captured state feeds native gameplay.

```powershell
python tools/native/check_mode_two.py --mode 3 --out build/native-flight/mission-three/original-check
python tools/native/check_mode_two.py --mode 3 --aircraft 2 --out build/native-flight/mission-three/original-two
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mode_three_aircraft_[12]$' --output-on-failure
```

Each aircraft passes 43 actual C0F3C4/C0F5F8 input/stage intervals and 29
C0EFEA/C0F3C0 bodies with zero differences in compared original RAM/display.
Existing oracle exclusions remain unchanged: original stack, native record
locals and documented scratch/asynchronous voices/blitter polling. No HUD
mask or new rendering exclusion is introduced. This establishes sampled
component behavior and runtime integration, not full mission/sequence parity,
audio fidelity or the 20 ms performance target. No full original replay was
repeated.

The model oracle additionally passes all 65,536 selector-word values and 52
wrapped-index/boundary cases against original C1FE68's cursor/result, plus
24 complete C0CFB6 parents (one to three circles, rejection, scaling and zero
radius) against cursor/result and all non-stack RAM/display. Its older circle
test's second layout still assumed the retired ascending 256-row allocation;
it now independently asserts the actual native descending $1F40 spacing and
compares a relocated copy against the original logical plane contents.
48 circle cases, existing viewport/flag/hull tests, 25 descriptor comparisons
and complete grid/control/followup parents pass on the mission checkpoint.

Artifacts reside in `build/native-flight/mission-three/`: per-aircraft
`captures.json`, entry/body logs and before/after/source exports; the original
sort trace; `model-check.log`; native/reference build and regression logs.
The delivered `build/native/fa18_native.exe` includes this batch.

Validation also passes ten affected native integration tests, active/crash/map
source frame bodies using the current runner, frontend original pixels,
save/reload, SDL presentation and link omission, plus the reference build and
twelve host/loading tests. Normal mission mode 3 is connected; modes 4, 5, 7
and 8 remain gated, and mission outcome children remain incomplete.
