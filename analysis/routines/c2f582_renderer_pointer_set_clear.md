# `$C2F582` renderer pointer-set clear

Classification: **static-only dataflow**. The parent frame-update tail calls
`$C2F582` when its frame counter modulo `$20` equals `$08`, but no available
P-code export executes this helper.

`source_amiga/observed/clear_renderer_pointer_sets.asm` is byte-exact for the
complete `$C2F582-$C2F5BF` span (62 bytes). The primary entry selects the
pointer quartet at `$C4566E`, clears it through the shared `$C2F596` body, then
selects the quartet at `$C4567E` and falls through to clear that too.

The shared body loads four longword pointers from `A1`, zeroes `D0-D7` and
`A4-A5`, then writes that 40-byte register block at each loaded pointer. This
establishes buffer-clearing dataflow only; the contents and graphics ownership
of those buffers remain unassigned until runtime evidence reaches the helper.
