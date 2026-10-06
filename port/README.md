# Active port source ownership

Latest direction (2026-10-06): `fa18_native` reuses the game source with a native
intro/menu entry, omitting CPU, translations, glue and chipset objects. Build
with `python ../scripts/build_native.py` from this directory, or
`python scripts/build_native.py` from the repository root. See
[`native/README.md`](native/README.md). The historical restriction below is
superseded for this explicitly requested runner; the deleted implementation
remains retired.

The active game implementation is `game/`, with temporary CPU adapters in
`game/glue/`. The playable build is defined in `recomp/CMakeLists.txt` and
`../scripts/build_recomp.py`; it produces `fa18_recomp` and `fa18_romfree`.

The abandoned top-level `fa18_port` implementation, its build definition,
contract tests and disconnected native replacement modules have been removed.
Do not recreate that implementation or use historical standalone test counts
as evidence of progress in the playable runner. Previous sources remain in git
history. Some historical analysis and oracle tools reference those removed
sources and cannot be run against the current tree.

All 48 retained shared C source/header files now live in `game/`; no C sources
or headers remain at the top level of `port/`. Build and include paths use the
new locations. These retained dependencies are:

| Files | Active use |
| --- | --- |
| `game/disk.c/.h`, `game/hunk.c/.h` | Original ADF/OFS reading and Hunk parsing, used by `amiga/` loading |
| `game/map_packet_*.c/.h`, `game/map_detail_*.c/.h` | Shared map-packet core called by `game/map_packet.c` and its glue |
| `game/projection_packet.h` | Map-packet depth/result types |
| `game/command_types.h`, `game/context_command_types.h`, `game/flight_command_types.h` | Types included by the active command implementation in `game/` |

Build from the repository root:

```sh
cmake -S port/recomp -B build/recomp-cmake
cmake --build build/recomp-cmake --config Release
python scripts/build_recomp.py
python scripts/build_recomp.py --romfree
```

Removing the abandoned source does not remove the active runner's remaining
Musashi, guest-memory or chipset dependencies. Their removal must happen through
the actual game/runtime call graph.

`game/native_call_graph.json` records C-only ownership for retired CPU entry
adapters. Tooling checks known original callers and actual C call sites before
excluding these entries from deferred/unported lists. This inventory is scoped
to discovered calls; runtime edge counts in profile `_native_edges` demonstrate
exercise in the fixed suite. Neither proves whole-game CPU independence.

Headless frame diagnostics use `--rgb444 OUT.bin --index8 OUT.index8`. The
index stream contains one selected palette index per 320x256 pixel per frame,
including during black fades; it does not change guest state or the RGB stream.
`../scripts/compare_recomp_frames.py` ignores only same-index colours belonging
to the original game's 16-stage Copper fade table at C08510. It checks drawing
indices, all other colours and frame counts. The live gate, timing probe,
comparison image report and emulation meter use that policy. Python comparison
tools and their regression check require NumPy; image reports also use Pillow.

The active emulation meter uses schema 2. Its instruction total includes
`interpreted`, `generated`, `residual` and `adapter`: handwritten CPU steps
fetching original opcodes still count as emulation. Retained C scheduling
without an opcode fetch does not count. Schema-1 percentages omitted adapters
and are superseded. The canonical meter report states whether its scenario
coverage is partial or the full acceptance suite. Raw counts with failed parity
do not establish accepted independence; subsystem omission builds remain the
completion gate.
