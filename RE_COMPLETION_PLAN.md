# Analysis plan

Updated 2026-09-28.

## Purpose

Reverse engineering now serves the recreated C source (see [PORT.md](PORT.md)).
It answers what each routine, variable and record field *means*, so the C can
use real names and types. It no longer gates running code: the game already
runs as translated C, and each routine's behaviour is proven by the shadow
check, not by analysis.

Analysis must still be evidence-based. A name needs behavioural or scenario
evidence; until then the C uses honest structural names (`REC_FIELD_0C`) and
the memory map says what is known.

## Constraints

- No Ghidra on this account. Use the existing `pcode/raw/` exports,
  `scripts/disasm_game.py` (all CODE hunks, relocated), the generated
  translation (`port/recomp/generated/`, with the original instructions as
  comments), `source_amiga/observed/`, and emulator traces from
  `scripts/engine9000_bridge.py`.
- Sealed captures are read-only.

## Assets

| Asset | Contents |
| --- | --- |
| `analysis/memory_map.md` | Addresses of globals, records and hardware use |
| `analysis/routines/` | 425 routine reports: contract, callers, evidence |
| `source_amiga/observed/` | 1,176 byte-exact assembly slices (51,256 bytes) |
| `pcode/raw/` | Raw P-code for 122 captures (16% of CODE bytes) |
| `analysis/coverage.json` | Byte coverage: 285,976 CODE bytes, 66,192 confirmed data |
| `analysis/port_inventory.md` | Porting order, with the module and report for each routine |
| `port/recomp/generated/recomp_graph.json` | Call graph of the 540 translated routines |
| `port/recomp/generated/recomp_edges.json` | Observed call edges, including indirect calls |

## Work, in the order the port needs it

1. **Name what the next porting wave uses.** For the routines ranked by
   `tools/recomp/port_candidates.py`, list the globals and record fields they
   read and write, and establish their meaning from callers and traces. Add
   each to `analysis/memory_map.md` and `port/game/globals.h` together.
2. **Control records.** The sixteen 512-byte records at `$C46184` are the
   game's objects. Build their field map (offset, width, writers, readers,
   observed values) from the translated code, and turn it into a C struct.
3. **Renderer pipeline.** The record walker (`$C1FB82` and its callers),
   transforms, projection and polygon submission. These are the next large
   porting targets; the existing reports (`c1fb82_*`, `c1f4ac_*`,
   `c24cfe_*`, `c2ff48_*`) are the starting point.
4. **OS use.** Inventory every Kickstart entry the game calls (the
   interpreter's executed ROM PCs plus the library vector offsets in the
   code), with arguments and results, as the specification for replacing
   them in C (PORT.md stage E).
5. **Timing.** Measure the real per-frame CPU stall caused by bitplane,
   Copper and blitter DMA (UAE `custom.c` and `blitter.c` slot allocation)
   as the specification for bus contention in the machine layer.
6. **Scenario coverage.** Record sealed runs for the modes run075 does not
   reach: free flight, training, missions, combat, landing, crash, map, pilot
   log. Each one becomes a parity and shadow-proof scenario.
7. **Code/data classification.** The translator's discovery and the
   interpreter's fallback log show what executes. Use them to settle the
   55,280 unclassified CODE-hunk bytes.

## Deliverables

- A complete memory map and control-record struct, used directly by
  `port/game/`.
- Routine reports for every recreated routine whose meaning is not obvious
  from its C.
- A specification of the Kickstart calls the game makes.
- Recordings for every game mode.

Byte-exact assembly reconstruction (`scripts/verify_reconstructions.py`) and
P-code coverage are no longer goals in themselves; keep the existing products
as reference material.
