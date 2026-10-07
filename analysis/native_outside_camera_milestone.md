# Native outside camera and coarse angle limits

2026-10-07. The active build remains `fa18_native`, using shared runtime
objects from `port/native/CMakeLists.txt`. Full gameplay, typed state, audio
fidelity and performance acceptance remain unfinished.

## Demo outside view

The user reported apparent clipping through the aircraft just after demo
takeoff, without supplying flight or camera controls. The actual caller is
native frontend tick -> native flight body -> native scene sequence ->
scene placements -> C1ED4C aircraft descriptor -> `record_vertices`.

The first independent drawing failure was source update 2312 / native 2275,
game tick 133. The preceding native body (update 2274, tick 132) differed from
complete original C0EFEA/C0F3C0 execution by 5,449 display bytes. A complete
C1ED4C comparison identified the aircraft descriptor at C34A58: its record
cache matched, but 134 transformed-vertex bytes differed.

C1F106 advances A1 over an unrotated model vertex. C1F158 calls C1F2EE, which
reloads that vertex at A1-6, offsets it by the source camera anchor and
projects through CAMERA_MATRIX. The native attached-camera branch instead
passed the rotated record-cache vertex, applying the aircraft rotation twice.
It now passes the original model vertex to the existing `view_transform`
owner. The world-space branch continues consuming the rotated cache.

The roll itself also occurs in the original demo playback. No motion rule,
recorded input, camera placement, view selector or clipping limit was changed.

Independent source and native runs compare 223 consecutive pre-input
boundaries: source 2179..2401 / native 2142..2364. All 223 now match both
complete 320x200 four-plane drawing pages, named player motion/pose/matrices,
phase/input fields, observer position, camera/view matrices, view pan/rotate,
attitude and side. Previously only 153 drawing boundaries matched; all camera
and player pose comparisons already matched. This is a complete checked
takeoff segment, not a complete demo or whole-game acceptance claim.

A focused original prefix of 6,000 PAL frames supplied the earlier takeoff
evidence missing from the previously retained tick-222..349 window. No sealed
recording was modified. Artifacts, before/after visualizations and logs remain
under `build/native-flight/demo-takeoff-review/`.

```powershell
python tools/native/check_gameplay_window.py --runner build/native-cmake/native/Release/fa18_native.exe --source-prefix build/native-flight/demo-takeoff-review/source --source-first 2179 --native-first 2142 --count 223 --out build/native-flight/demo-takeoff-review/accepted
python tools/native/check_frame_body.py --runner build/native-cmake/native/Release/fa18_native.exe --case outside-takeoff --case outside-roll --case outside-bank
```

The three new assembled-body cases compare original instructions at actual
native takeoff/roll/bank boundaries. All ten cases in the frame-body suite
pass, including cockpit, map, crash and existing demo selection/readout cases.
All display bytes are compared. Existing scratch, asynchronous voice and
busy exclusions remain unchanged. Complete descriptor/grid/followup parents
also pass on the formerly failing native checkpoint.

The independent gameplay comparator now checks the named camera state as well
as the previous page and player scope. Its mutation checks reject altered
observer position and camera/view matrices; no pixel mask was introduced.
The later seconds-driven HUD page difference remains a separate open elapsed-
time assessment documented in `native_gameplay_acceptance.md`.

## Coarse matrix limit and extended missions

Sampling active postflight bodies in modes five through eight exposed another
connected defect at mode-seven stage C11788. Native record dynamics call the
shared `build_transform_product` / `extract_transform_angles` owners. Original
C2E042/C2E048 subtract and compare the coarse table index as signed longs.
Native narrowed it to a signed word before clamping. The original index FE28
is positive 65,064 and clamps to F8; the premature word conversion reads a
negative table offset. The source returns pitch 1C70; the old native result
was B408. The shared calculation now clamps before narrowing.

The record oracle passes 576 complete C2DEE0 product/extraction cases,
including 64 additional coefficient/clamp/sign-boundary cases, comparing
returned angles, retained divisor and non-stack RAM. Its 128 signed-attitude,
256 settling, 256 publication and actual C12098/C1C63E checkpoint cases pass,
including the formerly failing mode-seven checkpoint.

| Normal input scenario | Input/stage intervals | Sampled bodies | Scene/HUD frames |
| --- | ---: | ---: | ---: |
| Mode five | 58 | 99 | 8,934 |
| Mode six | 59 | 215 | 5,765 |
| Mode seven | 60 | 161 | 14,647 |
| Mode eight | 55 | 251 | 14,859 |
| Total | 232 | 726 | 44,205 |

All compared original gameplay RAM and display bytes match. Original guidance
continuations execute once in sampled mode seven and three times in mode
eight. The mode-eight positive-return assertion remains enforced. The sampler
now covers active postflight bodies beyond C10DAE. Mode five leaves the player
stationary under these controls while aircraft 4/6/8 move; its probe requires
observed non-player aircraft motion. It does not leave C10DAE for a separate
formation stage, correcting the earlier investigation hypothesis.

These are connected comparisons starting original instructions from native
boundaries, not independent full mission successes. The new runtime gates
are `fa18_native_combat_5` and `fa18_native_combat_7`. All eleven selected
matrix/flight/postflight/renderer gates pass. Artifacts are retained in
`build/native-flight/combat-probe/clamp-5` through `clamp-8`.

No captured state, copied geometry, fitted timer or additional exclusion
supplies playable behavior. The complete-port goal remains active.

## Final validation and build

The complete native Release build and both reference builds pass. After the
camera change, four affected renderer/scene/expiry/HUD gates pass again;
the full frame-body suite, seven actual record checkpoints and the final
frontend/link omission check pass. Settled intro/menu drawing, pilot save/
reload and SDL presentation remain validated. The native link still omits
CPU, translations, glue, bus and chipset. The public executable is refreshed.
Protected scripts/allowlist, the ADF, sealed recordings and `.vscode/` remain
unchanged. The Visual Studio ADF argument/working-directory correction is
local build configuration, not gameplay code.

Public executable SHA256:
`49AE239A506775728ADA2FAC73325933F397F2A374E776D2958DC6BB5636B19A`.
