# F/A-18 Interceptor: recreated C source

Recreating the source code of *F/A-18 Interceptor* (Intellisoft / Electronic
Arts, Amiga, 1988) as readable C, proven against the original.

The authority is the original disk (`FA-18 Interceptor (1988)(Electronic
Arts)[cr A-Ha].adf`) running on a pinned Engine9000 build (UAE core), with
Kickstart 1.3, A500 PAL OCS, 512 KiB Chip + 512 KiB Slow RAM.

## Where things stand

The game runs natively as C. A mechanical translation of the original 68000
code runs on a small Amiga machine model, in an SDL2 window at 50 Hz.
Hand-written C is replacing the translated routines in source-backed batches;
425 game entries are registered. Three sealed native recordings cover the
demo, a successful carrier landing, and qualification failure.

The immediate work is the next complete readable source batch. The user has
deferred the minor one-frame Copper-fade delay; its evidence is retained in
`CURRENT_PORT_HANDOFF.md` for later parity work.
The gauge correction matches all recorded frames and sealed final RAM in
isolation; 208 registered entries now have source timing. The complete
registered demo still matches through frame 415. The targeted gauge checkpoint
is complete, as are the placement-ordering parent and two workspace selector
helpers. The complete C1D10C template-placement parent now passes independent
domain, normal CPU-adapter and source-timing checks and is registered.
The complete primary/alternate scene-placement pair also passes 8,192
structural cases and 17,047 completed recorded whole-call comparisons;
its consumers remain explicit child owners. See
[scene-placement proof](analysis/routines/native_c_scene_placements.md).
The frame-416 comparison
confirmed a fade starting and finishing two frames late. Source timing for
the scene initializer has removed one delayed frame; one remains inherited
from preceding HUD updates. See the
[visible checkpoint](analysis/routines/native_frame_416_checkpoint.md) and
[planning review](CURRENT_PORT_HANDOFF.md#planning-review-2026-10-02).

See [STATUS.md](STATUS.md) for the numbers,
[CURRENT_PORT_HANDOFF.md](CURRENT_PORT_HANDOFF.md) for the next steps, and
[PORT.md](PORT.md) for how the port works.

## Quick start

Build (Windows; CMake with MSVC, or gcc):

```sh
cmake -S port/recomp -B build/recomp-cmake
cmake --build build/recomp-cmake --config Release
sh scripts/build_recomp.sh            # headless gcc build: build/recomp/fa18_recomp.exe
```

The headless build uses Ninja to cache each source file and its header
dependencies. Structural oracles share those objects, so routine bridge edits
normally require one compile and one link.

Open the native demo start state (click the window to capture the mouse, F12
releases it):

```sh
build/recomp-cmake/Release/fa18_recomp.exe --state captures/native/demo01/state.bin \
    --rom local/system/kick13.rom --window --frames 0
```

Check the work:

```sh
sh scripts/recomp_ports_check.sh   # three sealed native recordings, shadow/sandbox/poison
sh scripts/recomp_live_check.sh    # fresh source OFF vs live ON frames and sealed final RAM
```

Set `PORTS_ONLY` to a comma-separated registered batch for its isolated live
check. The live gate checks final RAM during the existing ON frame replay
and removes temporary frame/RAM outputs afterward.

Large replay and trace artifacts are bounded automatically. Headless builds
prune old disposable files when `build/` exceeds 12 GiB, while preserving
compiler outputs and preferring the current `frames_shadow_*.bin` references.
Run `python scripts/prune_build_artifacts.py` directly to prune on demand, or
set `FA18_BUILD_MAX_GIB` to change the cache budget. Individual RGB444 outputs
are limited to 4 GiB and boundary traces to 1 GiB; set the corresponding
`FA18_RGB444_MAX_MIB` or `FA18_BOUNDARY_TRACE_MAX_MIB` value to zero only for
an intentional unlimited capture.

`local/` (ROM, extracted files, toolchain) and `captures/` (sealed
recordings) are not in git.

## Layout

| Path | Contents |
| --- | --- |
| `port/game/` | The recreated game source |
| `port/game/glue/` | Temporary adapters between translated callers and the new C |
| `port/machine/` | Amiga machine model: bus, blitter, Copper, display, CIAs, savestates, input |
| `port/recomp/` | Runtime for translated code, port dispatch and the shadow proof, `generated/` output |
| `tools/recomp/` | Translator, register liveness, porting tools |
| `tools/musashi/` | Vendored Musashi 68000 core (reference CPU and fallback) |
| `scripts/` | Emulator bridge, recording, parity, lockstep and proof scripts |
| `captures/` | Sealed recordings (read-only; not in git) |
| `analysis/` | Memory map, routine reports, inventories |
| `pcode/`, `source_amiga/` | Earlier analysis products: P-code exports, byte-exact assembly |
| `port/*.c` (top level) | The earlier bottom-up port (`fa18_port`), kept as source material |

## Documents

| File | Purpose |
| --- | --- |
| [STATUS.md](STATUS.md) | Current state and numbers |
| [CURRENT_PORT_HANDOFF.md](CURRENT_PORT_HANDOFF.md) | Current count, timing blockers, next batch and gates |
| [PORT.md](PORT.md) | Port architecture, stages, proof method, conventions |
| [RE_COMPLETION_PLAN.md](RE_COMPLETION_PLAN.md) | Analysis plan and how it feeds the C source |
| [GAME.md](GAME.md) | Game dossier: history, controls, landmarks, experiments |
| [AMIGA.md](AMIGA.md) | Amiga hardware and OS guide for reverse engineering |
| [REVERSE_ENGINEERING.md](REVERSE_ENGINEERING.md) | The reverse-engineering process, end to end, and its lessons |

## Recording new scenarios

Record with `fa18_recomp --window --record OUT.fa18in`, then seal with
`python scripts/seal_native_run.py NAME --state START.bin --input OUT.fa18in`.
The result is a read-only `captures/native/NAME/` recording. Archived
`captures/uae/` runs remain source evidence; they are not the current gate.

## Rules

- Never present an emulator frame as native output; emulator frames are
  comparison oracles only.
- Do not run Ghidra or its import/export scripts on this account. Use the
  existing `pcode/` exports, `scripts/disasm_game.py`, `source_amiga/` and
  emulator traces.
- `scripts/check_native_build.py`, `scripts/native_frame_count.py` and
  `port/native_data_allowlist.txt` are owned by the user. Do not edit them.
- A recreated routine is done only when `scripts/recomp_ports_check.sh`
  passes on all native recordings and live ON RGB frames match the native
  shadow reference for affected scenarios.
