# Bounded tuple-list submission at `$C301F6` (Hunk 36 +`$DA6`)

Classification: **behavioural display submission**. This complete run001 packet
reduces an observed list of word pairs to bounds and, on its observed path,
submits one line to the established blitter-line emitter. The list's primitive
and object ownership remain unknown.

## Runtime packet

- Direct edge `$C2FF4E -> $C301F6 -> $C2FF56`, hit at replay frame 9.
- 964 instructions, complete at the observed return boundary.
- P-code: `pcode/raw/run001_c301f6_render_list_stage/`, 209 observed RAM
  starts / 1,170 operations, all imported with the runtime instruction map.
- Its only observed nested target is `$C302B6 -> $C2FA7E`.

The list begins at `$C4B390`. The first word is reduced by three, and the
observed run then consumes eight `(word, word)` pairs. It maintains low/high
bounds in `D0/D2` and `D1/D3`, compares the vertical range with the word at
`$C45984`, and evaluates small absolute extents before the normal line path.
The observed path temporarily replaces `$C456E6` with `$000FFFFF` when
`$C457A2` is zero, calls the line emitter, restores the longword, and returns
`D0 = 1`.

`source_amiga/observed/submit_bounded_tuple_list.asm` is the byte-exact
206-byte observed-entry slice `$C301F6-$C302C3`. Static branches to
`$C302C4`, `$C302DA`, `$C302DE`, and `$C302EC` intentionally leave the slice.
