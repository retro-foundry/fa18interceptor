# ROM-free foundations checkpoint

This checkpoint advances the full ROM-removal objective. It does not deliver
`fa18_romfree` or a clean game launch. The Amiga SDK is reference material only.

## Implemented scope

`port/amiga` is an independently buildable host C11 library for explicit guest
memory, OFS resources and metadata, Hunk loading/relocation, service registration,
reference dependency inventory and fatal ROM-access auditing. It has no SDK,
CPU interpreter, ROM, savestate, SDL or game-asset dependency. Placement, CPU and
bus adapters, original ABI profiles and game assets belong to the embedding port.

The existing seven service families in `port/os` consume explicit bus phases
without fetching instruction or operand bytes from ROM. New FindTask (FC1DB0)
and FindName (FC1696) implementations preserve list traversal, name comparison,
interrupt nesting, guest register state and nested library-vector calls. The
dispatcher runs before interpreter opcode fetching. Known addresses identify
ABI contracts; they are not permission to read ROM data.

The reference runtime still executes other Kickstart services, including deeper
blitter helpers. Runtime guards therefore cannot yet enforce whole-game zero
ROM access. Captured RAM is oracle evidence only, never initialization input.

## Reproducible validation

- `cmake --build build/amiga-gnu` and GNU CTest: four portable contracts pass.
- `cmake --build build/amiga-msvc --config Release` and Release CTest: the same
  four contracts pass, including enabled assertions in Release.
- `python tools/amiga/check_ofs_reader.py`: all 22 game-owned ADF resources match
  independent extraction hashes and original header metadata.
- `python tools/amiga/check_hunk_loader.py`: 185 original ADF hunks and 8,441
  relocations match the true cold-entry segment layout. There are no unresolved
  segments or changed nonrelocated payload bytes in that cold layout.
- `python tools/amiga/check_service_phases.py`: 145 PCs /74,240 CPU and display
  DMA fixtures match registers, full SR, all RAM, ordered accesses and cycles.
  Candidate ROM/expansion-ROM buffers are cleared and their access guard active.
- `python tools/amiga/check_exec_task_lookup.py`: 9,216 complete FindTask/
  FindName calls match original instructions, register/SR/RAM state, ordered
  accesses and timing with ROM removed on the candidate side.
- GNU and MSVC Release reference builds pass. The fresh full 614-row gate passes
  571,427 shadow /458,087 sandbox comparisons, all three seals and poison frames.
- `python tools/amiga/check_service_recordings.py`: C service implementations
  versus original ROM service implementations match all 36,236 frames across
  the three sealed recordings, every RGB444 byte, final full RAM, CPU cycles,
  final PC and blitter counters. Both use the same game and machine settings.

Full-recording output hashes:

| Recording | Frames | RGB444 SHA-256 | Final RAM SHA-256 |
| --- | ---: | --- | --- |
| demo01 | 20,833 | 729f2ac3adae40e10aa4b87a31a40faa9c11836de16b9c3d1610e49215e37d07 | d28576a4f5718f2c190896529236be38c503e53e3c7446c5af733ee7cacec103 |
| qual_carrier_success | 12,353 | 8a8978bd6943d4cc8af26d8b4f7fda48dbf291cd3f1beca514c6f2317bfc0e0b | b68cb41fce666cc678ffd5e548b83f770d8b6d6eadcebfcace2c5c5a23d66a30 |
| qual_fail_crashes | 3,050 | 9c64d09e6b5155733ab59ec9398174c2511aa747263637757be12f727a95884d | 73918588343cd74e80f316168866e4ea17a3edc245fa0cd5617f0e63bcb03f69 |

The Engine9000 true power-on entry is C0DEB0 at host frame 7,070, with no restored
state. Its clock count is in OCS colour clocks, not CPU cycles. Committed
metadata is in `analysis/data/romfree_cold_entry.json` and
`analysis/data/romfree_hunk_layout.json`. Binary evidence remains ignored.

## Remaining acceptance work

Complete Exec memory, lists, signals, interrupt/exception handling; graphics
LoadView and deeper helpers; required input/gameport/timer/potgo/audio paths;
guest DOS operations and writable persistence; evidence-backed clean OS/device
initialization; original startup and exit; the separate ROM-free executable.
Full ordered interrupt/audio timelines and clean-launch recordings for all
reachable game modes remain unproved. The original broad acceptance scope in
the handoff remains active.
