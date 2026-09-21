# Renderer helper at `$C2F5F4`

Classification: **structural**. The direct run001 edge `$C2F616 -> $C2F5F4 ->
$C2F618` completes in 66 instructions at replay frame 12. It has no nested
call target.

P-code: `pcode/raw/run001_c2f5f4_renderer_helper/`, 66 observed starts /
351 operations. The observed path is retained as arithmetic and data-flow
evidence only; its input tuple and output ownership are not yet assigned.

The runtime-backed entry prefix `$C2F5F4-$C2F609` is now byte-exact source in
`source_amiga/observed/enter_renderer_table_helper.asm`. It loads the pointer
block at `$C456B6`, selects fixed tables `$C2F766/$C2F786` into `A3/A4`, and
branches to the shared body at `$C2F688`.

That shared body routes nonpositive `D1` to the byte-exact
`$C2F622-$C2F625` return in
`source_amiga/observed/reject_renderer_nonpositive_span.asm`, which sets
`D2=-1` and returns. This is a register-level contract only.
