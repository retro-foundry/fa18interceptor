# F/A-18 Interceptor: recreated C source

Recreating the source code of *F/A-18 Interceptor* (Intellisoft / Electronic
Arts, Amiga, 1988) as readable C, proven against the original.

The authority is the original disk (`FA-18 Interceptor (1988)(Electronic
Arts)[cr A-Ha].adf`) running on a pinned Engine9000 build (UAE core), with
Kickstart 1.3, A500 PAL OCS, 512 KiB Chip + 512 KiB Slow RAM.

## Where things stand

The game runs natively as C. A mechanical translation of the original 68000
code runs on a small Amiga machine model, in an SDL2 window at 50 Hz. It
plays run075 from its menu into the flight demo and accepts live keyboard and
mouse input. Hand-written C is replacing the translated routines one by one;
each replacement is proven against the original on every call.

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

Play run075 from the menu (click the window to capture the mouse, F12 releases
it):

```sh
build/recomp-cmake/Release/fa18_recomp.exe --state captures/run075/restored-state.bin \
    --rom local/system/kick13.rom --replay captures/run075/playback.e9k --window --frames 0
```

Check the work:

```sh
python scripts/recomp_parity.py --start 392 --frames 10   # frames vs the emulator
sh scripts/recomp_ports_check.sh                          # prove every recreated routine
```

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
| [CURRENT_PORT_HANDOFF.md](CURRENT_PORT_HANDOFF.md) | One-page handoff: next blockers and commands |
| [PORT.md](PORT.md) | Port architecture, stages, proof method, conventions |
| [RE_COMPLETION_PLAN.md](RE_COMPLETION_PLAN.md) | Analysis plan and how it feeds the C source |
| [GAME.md](GAME.md) | Game dossier: history, controls, landmarks, experiments |
| [AMIGA.md](AMIGA.md) | Amiga hardware and OS guide for reverse engineering |
| [NEXT_PROJECT.md](NEXT_PROJECT.md) | Playbook for the next game, end to end |

## Recording new scenarios

```powershell
python scripts/record_run.py --name run001                 # play in Engine9000
python scripts/finalize_run.py captures/run001             # seal it
python scripts/engine9000_bridge.py --restore captures/run001/initial_state.bin `
  --config captures/run001/config.uae --playback captures/run001/playback.e9k `
  --frames 300 --output build/run001_first300              # replay, snapshot, trace
```

The bridge writes `state.bin`, `chip.bin`, `slow.bin`, `screen.png` and
registers at the last frame; `--trace-frames N` adds an instruction trace and
custom-register write log. Sealed runs are never edited.

## Rules

- Never present an emulator frame as native output; emulator frames are
  comparison oracles only.
- Do not run Ghidra or its import/export scripts on this account. Use the
  existing `pcode/` exports, `scripts/disasm_game.py`, `source_amiga/` and
  emulator traces.
- `scripts/check_native_build.py`, `scripts/native_frame_count.py` and
  `port/native_data_allowlist.txt` are owned by the user. Do not edit them.
- A recreated routine is done only when `scripts/recomp_ports_check.sh`
  passes (shadow proof and poison check) and parity is unchanged.
