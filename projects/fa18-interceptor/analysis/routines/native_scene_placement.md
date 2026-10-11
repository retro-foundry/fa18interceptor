# Native scene recorder reset and root placement

`port/native_scene_placement.c/.h` implements complete `$C09266-$C095BE`
against the shared native scene records. The behavioral authority is the
decoded routine and its readable reconstruction in `port/game/scene_setup.c`;
the older detached positive/negative packets are not composed into this path.

The owner resets the recorder and playback cursors and counts, runs the actual
player prepare/reset children, selects the original scene pointer group, derives
the root kind, clears the source flag fields in order and dispatches the signed
16-byte pose entry. Positive entries use the original signed byte/word grid
tables and preserve MOVEM.W sign extension, SWAP, intermediate truncation and
32-bit wrapping. Negative entries retain the source's moved record pointer on
a rejected selection, resolve the indexed fifth scene pointer, call the native
local-to-world child and publish the selected angles through the complete native
record-orientation owner.

`fa18_load_native_scene_placement_assets` binds the Hunk 67 pose table, Hunk 8
grid tables, Hunk 63 trig data and the two Hunk 16 startup pointer groups. It
uses relocation metadata for pointers and does not expose guest addresses to
the routine. Runtime scene pointer groups remain mutable caller-owned semantic
objects because later game code rewrites them. A null original pointer remains
null; a nonzero unrelocated or out-of-range pointer fails explicitly.

`PortFieldWindow` is the reusable part: it presents an immutable byte object at
a signed logical origin and requires explicit adjacent field owners for reads
outside that object. It carries no F/A-18 addresses, CPU state, memory bus or
fallback data. The placement data model, record stride, flags and Hunk offsets
remain game-specific adapters.

The focused contract covers both pose signs, recorder reset, shared player and
context owners, fixed-point outputs, descriptor height, local-to-world and
orientation publication. The native bootstrap contract confirms `$C08F26`
calls placement directly before the real gate and the two remaining update
boundaries. These contracts are implementation checks; the prior sealed
bootstrap differential checkpoint treated placement as a child contract and
therefore does not prove this replacement. A full original-instruction
differential run remains required.

The native main still does not invoke this startup graph. Original asset/startup
assembly, lower update children and scheduling remain required for
the connected playable path.
