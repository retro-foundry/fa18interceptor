# Shared components used by the active port

The abandoned top-level gameplay implementation has been removed. Components
listed here are retained because the current playable runner actually uses them.

| Component | Active use |
| --- | --- |
| `disk.c/.h`, `amiga/ofs.h` | Read-only OFS/ADF loading and resource lookup |
| `hunk.c/.h`, `amiga/hunk.h` | Hunk parsing and relocation metadata used by the loader |
| `amiga/rgb4.c/.h` | Host colour-map and display-list palette operations |
| `amiga/viewport_list.c/.h` | Host viewport/display-list construction and merging |
| `amiga/sha256.c/.h` | Disk/executable identification and integrity checks |
| `map_packet_*.c/.h`, `map_detail_*.c/.h` | Shared source-backed map-packet core used by `game/map_packet.c` |

Game policy stays in `game/` and Interceptor startup configuration in `romfree/`.
Compatibility services and loading are built by `amiga/CMakeLists.txt` and linked
by `recomp/CMakeLists.txt`. The active runners still require CPU/machine adapters;
these reusable helpers alone do not establish an emulation-free game.

See `README.md` in this directory for source ownership. Removed standalone
voice/field/native-owner implementations and their proofs remain in git history.
