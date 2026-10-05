# Native startup ranges and reusable typed field bytes

`port/startup_ranges.c/.h` implements the complete `$C090C2` clear and
`$C090F2` enable leaves. The clear writes 53 bytes at the actual command
queue's neighboring offsets `$2F..$63`, then clears the supplied 52-word
startup view. Enable writes twelve 1 bytes at offsets `$23..$2E`. The
exclusive ends are original `$C457C5`, `$C45928` and `$C45790`, respectively.
These ranges and values are F/A-18 rules, not generic component defaults.

The startup owner references the live queue slots, so a later auxiliary-byte
binding immediately affects the clear. No slots or scalar values are copied
around execution. All 104 word-byte references are required, validated on
binding and supplied by the caller; no shadow word span is allocated or
initialized. Binding does not change the owners' values. The native proof
uses the existing spawn gate, command word, cockpit bytes, message state and
redraw word/long owners, with original imports for the other words.

Complete native startup record/geometry identity is still pending. In
particular, the cleared viewed-record offset needs its actual semantic native
record binding when the bootstrap graph is composed. The standalone proof
imports that word explicitly; it does not claim that a duplicated viewed
pointer was updated. Neither these two leaves nor their presence in the
native library complete `$C08F26` or the running game.

`port/field_bytes.h` extracts the mechanism already used by signed-index
queue publication. Its `PortFieldByte` references a byte within an ordinary
8/16/32-bit integer value, with signed and unsigned word/long owners. It
preserves other bits and reconstructs signed values without out-of-range
unsigned-to-signed conversions. Access does not depend on host byte order.
Exactly one live typed owner is required, with a valid byte shift. Missing
owners fail explicitly; ordered fills retain preceding writes on failure.
There is no host observer between the two bytes used to implement a word
clear. This provides the original final game-state result without creating
a CPU instruction boundary or scheduler.

The header has no F/A-18 types, original addresses, RAM allocation, address
lookup, CPU registers or external library dependency. It has two concrete
production callers: `command_queue` and `startup_ranges`. The independent
`port_field_bytes` CMake interface target and standalone contract make it
available to future ports. Compile any caller including `field_bytes.h`;
there is no object file to link. Original data interpretation and ownership
remain the game's adapter responsibility.

Run `python tools/recomp/check_native_startup_ranges.py`. There are 4,096
complete invocations of each leaf, covering **21/21** original boundaries
and matching every Chip/Slow RAM byte with **no exclusions or child
contracts**. Randomized neighboring fields and end sentinels verify both
clear extents. The actual original instructions execute fully from sealed
source; original-address packing exists only in validation.

The standalone field contract enumerates all 65,536 word bit patterns for
both byte positions, checks signed/unsigned long boundary patterns and all
replacement byte values, and covers aliases, invalid shifts/owner counts
and partial ordered writes. The startup contract binds later owners, uses
actual native command state, and publishes through the same cleared field.
The queue contract also checks explicit failure on a missing destination,
preserving preceding stores and leaving its output unset.

Extraction regression retains 73,728 complete queue calls, all 28 original
boundaries, all 138 raw destinations and all 256 translated destinations.
The preceding renderer/display proof retains 16,384 calls and all 62
boundaries. GNU strict standalone/integration builds, CPU/bus/machine/guest/
host symbol checks, the native MSVC game and affected tests pass. Eight
focused CTests pass, and the unchanged native guard passes 470 files.
`analysis/figures/native_startup_ranges_checkpoint.json` records the source
seals, implementation hashes and ownership limits; the queue/display
checkpoints include the shared header and have been refreshed.

Next bind complete control-record/workspace owners and actual remaining
startup children before composing `$C08F26`. The separate tenth renderer
buffer producer, scene initializer, original loading/checksum production,
installed-stage scheduling and sample output remain open. Native main still
does not call the new graph; the playable reference remains emulated.
