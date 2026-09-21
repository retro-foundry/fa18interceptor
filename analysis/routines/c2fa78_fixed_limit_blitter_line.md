# `$C2FA78` fixed-limit blitter-line entry

Classification: **runtime-observed structural/dataflow**. The run024
frame-23000 continuation executes this complete six-byte alternate entry.

`source_amiga/observed/prepare_fixed_limit_blitter_line.asm` is byte-exact for
`$C2FA78-$C2FA7D`. It loads `A2` with `$00C7` and joins the normal
`$C2FA84` setup path of `prepare_blitter_line_parameters.asm`, bypassing that
routine's load of the variable limit at `$C45984`.

The caller and the meaning of the fixed limit remain unassigned.
