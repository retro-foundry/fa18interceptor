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

Only dependencies of the active runners remain as top-level C sources/headers:

| Files | Active use |
| --- | --- |
| `disk.c/.h`, `hunk.c/.h` | Original ADF/OFS reading and Hunk parsing, used by `amiga/` loading |
| `map_packet_*.c/.h`, `map_detail_*.c/.h` | Shared map-packet core called by `game/map_packet.c` and its glue |
| `projection_packet.h` | Map-packet depth/result types |
| `command_types.h`, `context_command_types.h`, `flight_command_types.h` | Types included by the active command implementation in `game/` |

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
