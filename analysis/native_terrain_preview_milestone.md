# Native terrain preview milestone ? 2026-10-06

The playable `fa18_native` setup now renders the source horizon and terrain
through location and aircraft selection. It retains the earlier C10C08 setup
and C10DAE pause/resume endpoints. This is connected preview work, not active
flight or full recorded-frame acceptance.

Runtime path: `port/native/main.c` -> `native_frontend_tick` ->
`native_flight_tick` -> `native_scene_project` / `native_scene_draw`. The scene
slice follows C0EFD4's record, matrix/projection, context, slide/list reset,
active planes and map ordering. It reuses C2D99C matrix dispatch, C1C54E
projection, C254E8 octant, C122A2 attitude, C2FD8C horizon submission, C2AA9C
normal/wide terrain selection, and C246A0 clipping. Existing source packet
selection, coordinate transforms, clipping, colour/plane selection, thin
polygon handling, inclusive fill and compositing remain authoritative.

Dependency removed from this native render path: chipset blitter submission
and machine-owned map data access. `port/game/native/raster.c` writes the
ordinary host plane buffers directly. `FA18_NATIVE` branches in the shared
render owners select those direct operations; the reference runners retain
their existing MMIO/blitter path. The source packet tables come from the ADF's
loaded executable data, not a capture. A separate host-owned mask occupies
0x30000, and map scratch uses 0x4000. No ROM, CPU/glue/opcode dispatch, chipset
register state, Copper or event scheduler links into `fa18_native`.

Validation:

- `python tools/native/check_raster.py`: at native location frame 5300 and
  selected-aircraft frame 6100, 160 deterministic polygon cases and complete
  horizon/map submissions produce identical low-buffer/plane bytes to the
  original opcode routines. Cases exercise small/flat polygons, edges in both
  directions, last-row clipping, colour/complement, disabled planes and masked
  copying. The validation executable compiles the exact native raster branches
  with a test memory backend and executes original opcodes independently.
  CPU/ROM/chipset are validation dependencies only. Comparison excludes source
  stack locals, busy-poll counters and Copper palette/fade effects. This is
  raster component equivalence for these inputs, not full frame timing/state.
- Both runner checkpoints draw nonblank terrain and differ after selection;
  scene/terrain counters establish execution in the actual runner.
- `check_flight_start.py`: all seven prior startup/location/aircraft/pause/resume
  checkpoints still pass. `check_frontend.py`: settled intro/menu pixels,
  callsign/save/reload, SDL presentation and native link omission pass.
- `check_records.py --reference build/native-flight/source.ram`: all five
  C1C63E comparisons still match original RAM outside source stack scratch.
- Native and reference MSVC builds pass; GNU builds the raster/record oracles;
  twelve reference CTests pass. No full sealed replay or recording mutation.

The masked-copy check exposed C3040C's retained 40-byte C modulo: its source
read rows advance by rectangle width plus 40, while A/B/D advance by 40.
The native copy preserves this source behavior. The checker also excludes the
RTS preceding C2FF48 from its original entry and advances the oracle's cycle
counter through busy polling; it makes no hardware-timing claim.

Functional estimate: approximately **80% of Free Flight startup wiring**, up
from 70%, because the previously blank setup viewport now renders its source
terrain. This is a rough scoped estimate, not an instruction count or whole-game
percentage. Aircraft/scene objects, cockpit/HUD, full input/view/timer ordering
and active flight are not complete. In particular, adding C12098 view-control
updates reaches further DY_RECORD_CONTROLS/SELECTOR/ROOT_FLIGHT children; that
input/control slice remains unconnected until its complete source composition
is implemented. The renderer does not silently bypass those children.
