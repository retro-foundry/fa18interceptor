# `$C2F688`: four-plane pixel address and mask preparation

Classification: **behavioral, run060-backed renderer primitive**. The
`$C2F688-$C2F6D7` prefix is memory-read-only, but the complete pipeline is
**not**: its optional XOR phase and table-selected handlers write Chip RAM.
The four lanes are 16-bit planar pixel words. Which buffer is being displayed
at a particular frame is a separate Copper/pointer question. Ghidra's
`observed_call_00c2f688` group has 45 distinct P-code starts across the
existing exports, but its function boundary excludes tail-dispatched handler
writes; a no-`STORE` query on that group alone is not a purity proof.

## Exact preparation contract

The primary wrapper `$C2F5F4` supplies `A1=$C456B6`'s pointer block,
`A3=$C2F766` (16 one-hot word masks `$8000,$4000,...,$0001`), and
`A4=$C2F786` (16 handler pointers). `$C2F626` and `$C2F63A` select an
alternate mask table at `$C2F7C6`; `$C2F5C0` adjusts the input pair and
rejects X outside signed `[0,$140)`. The shared body returns via `$C2F622`
with `D2=-1` when signed `D1.w <= 0`.

For a positive `D1.w`, let `x=D0.w`, `y=D1.w`, and `mode=$C45954.w & 15`:

```text
handler = long[A4 + 4*mode]
bit     = word[A3 + 2*(x & 15)]
clear   = (~bit) & $FFFF
offset  = wrap16(40*y + (sign16(x & $FFF0) >> 3))
lanes   = four longs at A1, each incremented by sign16(offset)
(D0,D1,D2,D3).w = clear; (D4,D5,D6,D7).w = bit
```

The arithmetic is word-width: the `ASL.W`/`ADD.W` sequence makes `40*y`
modulo `$10000`. For the wrappers' in-range X, `(x & $FFF0)>>3` is the
byte offset of a 16-pixel word. `$C456E7` can suppress individual lanes by
replacing their clear mask with `$FFFF` and their set mask with zero.
`$C456E8/$C456EB` optionally XOR selected lane words before the final
dispatch. The 16 `$C2F786` targets at `$C2F826-$C2F8C6` implement every
four-lane combination: a zero mode clears the selected bit in all lanes by
`AND.W clear`; a set lane uses `OR.W bit`. The alternate `$C2F7C6` table
contains mostly adjacent two-bit masks; its first two entries are both
`$C000`, so its edge rule must not be simplified to a uniform shift.

## Runtime checks and downstream meaning

The deterministic run060 frame-9548 trace
`build/run060_qualification_message_9547_trace/trace.jsonl` enters the
primary path with `(x,y)=(158,167)`. It loads mask `$0002`, computes offset
`$1A2A = 40*167 + 18`, and obtains adjusted pointers
`A3=$04F55A`, `A2=$05149A`, `A1=$0533DA`, `A0=$05531A`. The four bases
read from `$C4566E` are `$04DB30,$04FA70,$0519B0,$0538F0` in the
corresponding `A3..A0` order, each `$1F40` (8,000) bytes apart. All are in
Chip RAM. With mode zero this invocation jumps to `$C2F826` and executes
four `AND.W` pixel-word updates. The optional XOR phase is established by
byte-exact source; this run060 window does not exercise it. The mask table
and four plane bases are also present in this packet's `slow.bin` snapshot.

This promotes the shared prefix from abstract offsets to a planar pixel-word
address contract. It explains the common work of `$C2F5F4`, `$C2F626`,
`$C2F63A`, `$C2F5C0`, and `$C2F60A` and the 16 mode handlers. It does not
identify what any drawn pixel depicts, or prove that these are the Copper's
currently displayed planes. Source: `prepare_renderer_table_offsets.asm`,
`mask_renderer_register_pairs.asm`, `apply_renderer_output_mask.asm`, and
`apply_primary_renderer_lane_masks.asm` under `source_amiga/observed/`.

The run075 frame-315 alternate path provides a separate port contract:
`A3=$C2F7C6`, `A4=$C2F7E6`, mode `$B`, mask `$0018` at x=172, and handler
`$C2F9EE`. It writes the selected two-bit mask at y=99 and y=100, changing
four Chip-RAM bytes and two deplanarized pixel indices. The later run060
trace at x=100, y=125, mode `$D` independently proves a primary-table handler
that maps input index 2 to 12. `port/renderer.c` translates the complete
proved primary and alternate table behavior into indexed chunky pixels through
the named `FA18RendererState` struct and `FA18PixelTable` enum. It preserves
enabled-plane behavior and both XOR paths. The native primitive owns only its
320x200 visual buffer, so original writes outside that buffer remain outside
this port contract. See `analysis/routines/c2f688_run075_two_row_mask.md`.
