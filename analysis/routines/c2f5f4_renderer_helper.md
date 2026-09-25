# Renderer helper at `$C2F5F4`

Classification: **behavioral shared pixel primitive; wrapper dataflow**. The direct run001 edge `$C2F616 -> $C2F5F4 ->
$C2F618` completes in 66 instructions at replay frame 12. It has no nested
call target.

P-code: `pcode/raw/run001_c2f5f4_renderer_helper/`, 66 observed starts /
351 operations. Run060 now independently anchors the shared pixel-address
and lane-write contract; the drawn object's ownership is not assigned.

The complete `$C2F5F4-$C2F765` cluster is now byte-exact source. Its proven
contract is: select one of the fixed word-table bases, load and offset four
pointers, derive paired register values, apply two independent four-bit masks,
XOR enabled pairs through those pointers, then either return or tail-dispatch
through an indexed `A4` pointer. The destination is now established as four
planar pixel words; the depicted object and active display buffer are not.

The runtime-backed entry prefix `$C2F5F4-$C2F609` is now byte-exact source in
`source_amiga/observed/enter_renderer_table_helper.asm`. It loads the pointer
block at `$C456B6`, selects fixed tables `$C2F766/$C2F786` into `A3/A4`, and
branches to the shared body at `$C2F688`.

That shared body routes nonpositive `D1` to the byte-exact
`$C2F622-$C2F625` return in
`source_amiga/observed/reject_renderer_nonpositive_span.asm`, which sets
`D2=-1` and returns. This is a register-level contract only.

The runtime-backed `$C2F688-$C2F6D7` shared prefix is reconstructed in
`source_amiga/observed/prepare_renderer_table_offsets.asm`. After the
nonpositive-span exit, it indexes a mode-selected pointer table and a word
table, derives a scaled offset, applies it to four pointers loaded through
`A1`, then seeds `D1-D6` from the table results. The one-hot mask table,
40-byte row stride, four 8,000-byte-spaced Chip RAM lanes, and run060 lane
writes now establish a planar pixel-word address contract. See
`c2f688_planar_pixel_pipeline.md`. Displayed-buffer and depicted-object
ownership remain separate questions.

The runtime-backed `$C2F6D8-$C2F717` mask phase is reconstructed in
`source_amiga/observed/mask_renderer_register_pairs.asm`. Each clear bit in
`$C456E7` replaces the corresponding `D0-D3` word with `-1` and clears its
paired `D4-D7` word. These are the per-plane clear and set pixel masks.

The runtime-backed `$C2F718-$C2F765` output phase is reconstructed in
`source_amiga/observed/apply_renderer_output_mask.asm`. When `$C456E8` is
non-negative, each set bit in `$C456EB` XORs a paired register word through
one of the adjusted pointers (`A3,A2,A1,A0`); any such write makes `D0=-1` and
returns. With no write, or a negative enable word, it tail-jumps through `A4`.
This proves pointer writes and control flow, not a pixel/object interpretation.
