# Native shared scene records and player setup

`port/native_scene_records.c/.h` supplies sixteen ordinary control-record
owners and sixteen workspace records. Their aircraft and geometry arrays
are contiguous and directly usable by native flight/context commands.
`port/scene_player_setup.c/.h` implements complete `$C0840E`, `$C09620` and
`$C095C0`, including the actual reset child called by preparation. It also
returns the complete `$C0910C` start-position tuple as ordinary values.
The existing complete native `$C0915A` observer child is reused.

Each record has one live data view of its first 164 bytes. Aircraft flags,
equipment/radar/stick, position, heading and all inverse-matrix elements
refer directly to the native command/context objects. Root byte `$2B`
references the indexed command owner's actual control-record level; the
other records own their level bytes. Other newly established setup fields
are named by their offsets where their full meanings remain unproven.
There are no CPU registers, original executable pointers or bus accesses.

Import requires the original sixteen 512-byte records and sixteen 32-byte
workspace records; no initial data is synthesized. Mapped bytes are removed
from the unported data storage after import, preventing a duplicate packed
owner from becoming stale. Bounded reads construct the data representation
from live fields, and bounded writes update those same typed owners. The
remaining positions retain supplied original data until their actual native
subsystems are ported. These views describe a game's record format, not an
address space, instruction runtime or emulator memory image.

The actual `$C08F76..$C08FAA` block clears exactly 41 longwords from each of
sixteen control records, preserving `$A4..$1FF`, then clears every byte of
all sixteen workspace records. This is a complete clear block inside the
bootstrap; it does **not** implement the complete `$C08F26` parent.
The same record clear is used for slots 1 through 3 by `$C0840E`.
Both routes immediately change the aircraft/geometry fields used by commands.

Mission reset writes root long `$72` to `$61A800`, byte `$5F` to `$24` and
word `$60` to `$1F4`. These are retained as numeric fields, with no invented
pointer meaning. It sets the real shared weapon redraw counters to 3,
chaff/flare counts to `$10`, clears the mission flags in original A/C/B
order and the aircraft spawn gate, then clears the three additional record
prefixes. Preparation clears root byte `$21` bit zero, sets the real radar
byte to `$0D`, calls that actual reset, then initializes flags `$11C8`, word
`$7E` to `$1400`, the seven actual player globals, limit/ready/root latches
and selection. A nonzero phase becomes 4; zero remains zero.

Transient reset clears only the source words/longs, clears the same aircraft
stick byte used by input, sets the indexed root level to 9 and clears flags
bit 15. It resets warning/event values, the command effect owner's actual
message code, the shown word and the marker. All globals without existing
owners remain required caller-owned references. The queue binder attaches
phase and selection-active at offsets `$37` and `$107`; phase can directly
be the native tick's field. Missing owners return an explicit error while
preserving preceding stores, without fabricated state or child substitutes.

Run `python tools/recomp/check_native_scene_player.py`. Six sets of 4,096
comparisons cover **98/98** reachable instruction boundaries: five complete
entries, including the existing observer, and the explicitly bounded
bootstrap clear block. There are **no child contracts**. The real original
reset executes fully inside the original preparation call.

Every Chip/Slow RAM byte matches. Only the original CPU ABI stack
`$C7FD00..$C7FF00` is excluded for `$C0840E`/`$C09620`, whose MOVEM and
nested call write saved CPU frames. No RAM is excluded for `$C095C0`,
the tuple/observer entries or the record-clear block. Explicit tuple results
and independently mapped aircraft/geometry owners also match. The fixtures
vary every record byte and required global, cover both phase branches,
check original record/workspace imports and preserve all record tails.
Original-address packing and CPU state are validation-only.

The native integration contract imports all sixteen records, checks complete
roundtrips and live command/packet field writes, then prepares and resets
through the real children. Cleared inverse matrices and positions are read
by the existing actual local-to-world child. It initializes the observer
through the original tuple, binds the native tick phase and changes that
same phase through a real signed-index command publication. It also checks
all prefix/tail/workspace clear extents and preceding writes on owner failure.
GNU strict compilation and CPU/bus/machine/guest/host symbol inspection pass.
Native MSVC game/test builds, five focused CTests and the unchanged native
474-file guard pass. The checkpoint under `analysis/figures` records source
seals, implementation hashes, comparison scope and ownership limits.

Complete `$C08F26` composition remains pending. Use this bank's actual
arrays and fields when binding the remaining placement/gate/update/context
children; do not synchronize detached legacy record copies. Viewed/selected
record identity must be bound through its native owner when the startup word
clear is composed. Original record-data loading, unported record consumers,
the tenth renderer buffer producer, scene initialization, checksum production,
sample output and complete scheduling also remain required. Native main
does not call the new graph; the playable reference remains emulated.
