# Complete native escort bodies - 2026-10-10

All **6,954 actual native escort bodies**, updates **40,395 through 47,348**,
now execute original instructions with **zero gameplay differences and zero
display-byte differences** under the existing frame-body contract. This includes
combat, carrier contact, arrest, the earned result and postflight updates.
The independently started game's state and accumulated drawing history remain
separate acceptance checks.

The broader sweep exposed **18 mismatching bodies** in the preceding runner.
Every visible page still matched; the difference was the alternate placement
cache's first result word at `C4F6DE`. At update 44,005, native retained `00C2`
while original instructions returned `0001`. Other affected updates returned
zero, which the original placement owner normalizes to `FFFF`.

The original writer trace establishes the complete producer/consumer chain:
normalization and map clipping leave `00C2` in the later model's stack word;
ground rendering clears it at **C09812** and accumulates successful command
results at **C09850**. Aircraft expiry takes **C1F074 -> C1F87A -> C1F8DE**
before normal model initialization, returning that preceding word. **C1CFBA**
publishes it to the placement cache. Native ground rendering calculated its
return but omitted these two accumulator stores.

`port/game/native/model.c` now performs the source clear and command OR stores.
Its existing scene-placement wrapper publishes the actual resulting value to
the renderer's retained state. The connected path is `port/native/main.c ->
native_frontend_tick -> native_flight_tick -> native_scene_draw ->
native_scene_placement -> ground_draw`, followed by the aircraft-expiry draw.
This fixes missing producer state in the existing playable renderer; it adds
no replacement drawing, clock adjustment or comparison exclusion.

The model oracle additionally compares the ground accumulator's publication.
A captured original ground-entry state supplies its actual `00C2` caller word
only to that external component test, using the existing local-frame ABI
mapping. The fixed implementation passes; the same gate rejects the preceding
implementation despite identical ordinary return, vertices, records and pages.
That component input never initializes the playable game.

The new native game ordinarily enlists, earns its qualification and preceding
mission, completes escort with gear managed, catches the wire, earns its second
completion and returns to the menu. An independent playable-runner replay
matches all **27,176 trace rows**, complete final RAM and the saved pilot.
All controls, runtime counters, trace bytes, final RAM and pilot bytes also
preserve the preceding successful native recording exactly. Final frame is
**75,601**, actual replay update **47,349**, with zero resets or pending input.

The full body capture verifies every input entry against that retained trace,
then supplies only the external CPU oracle with each actual before/after state
and the existing PAL timer contract. Original **C0EFEA-C0F3C0** executes on each
body. Existing exclusions remain explicit: native-local ABI scratch,
asynchronous voice records/slots, synchronous-raster busy words and fake
oracle stack. RGB4 publication remains a host boundary. This proves original
rules across the entire native route; it does not equate the two independent
games' accumulated caches or different constructor-clock worlds.

Validation passes six affected Release CTests, Debug expiry plus artifact
cleanup, the strengthened original model comparisons, twelve recording and
identity rejection guards, seven delta-decoder tests and all 6,954 original
body comparisons. The legacy original-input mode still requires strict
gameplay parity and rejects the earlier failed escort comparison.

The capture contains **20,862 complete MiB snapshots** in **44,237,284 delta
bytes**, within the unchanged 512 MiB limit. Gameplay reports zero project heap
violations, SDL pool requests and pool failures. Passing raw captures are gone;
verified identical compressed full traces/RAM share storage. All 18 preceding
failures, their snapshots, writer trace and compressed preceding executable
remain retained. The 4 GiB pruner remains enabled.

The [checkpoint](figures/native_escort_complete_bodies_checkpoint.json) binds
complete body identities, logs, source inputs, rejected preceding results,
regression guards and retained artifact hashes. The canonical Release runner
is `148d58e7d8821b1221ecdf42f1904398d52e32b401480252a78ed39d49b1dac6`.
Broader independent full-flight comparisons and original sound timing remain
open. Campaign continuity stays waived; named-state cleanup remains separate.

```powershell
python tools/native/check_recorded_patrol_input.py --test build/native-cmake/native/Release/fa18_native_recorded_patrol_input_test.exe --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-escort-wire-target --source-updates build/native-flight/original-escort-wire-updates --level-final --out build/native-flight/recorded-escort-ground-carry
python tools/native/check_mission_frame_delta.py --runner build/native-cmake/native/Release/fa18_native.exe --reference build/native-flight/recorded-escort-ground-carry --source-evidence build/native-flight/original-escort-wire-target --source-updates build/native-flight/original-escort-wire-updates --native-input-reference --out build/native-flight/escort-ground-carry-complete-body-delta
python tools/native/test_frame_delta.py
```
