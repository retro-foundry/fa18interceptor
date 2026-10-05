# Active port source ownership

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
