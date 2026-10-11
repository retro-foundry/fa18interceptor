# `$C2F8D0` offset renderer lane-mask targets

Classification: **static-only dataflow**. The alternate dispatch table at
`$C2F7E6`, selected by the bounded renderer entry, resolves to all 16 entries
in `$C2F8D0-$C2FA6F`; no available P-code export executes an entry.

`source_amiga/observed/apply_offset_renderer_lane_masks.asm` is byte-exact for
the complete 416-byte family. As with the primary table, each index selects
which of four lanes ORs its set mask (`D4-D7`) and which ANDs its clear mask
(`D0-D3`) through `A3-A0`. Each selected operation is applied at both the base
pointer and its `$28` offset.

This establishes paired memory-combine dataflow, not a graphics interpretation
for the pointers, offsets, or lanes.
