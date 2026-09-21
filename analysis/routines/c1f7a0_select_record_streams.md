# Control-record stream selection at `$C1F7A0` (Hunk 10 +`$E68`)

Classification: **structural**. This observed control-record block selects
`A1` and `A2` streams for the enclosing record walker. It does not establish
the ownership or semantic type of either stream.

`source_amiga/observed/select_record_streams.asm` is a byte-exact 152-byte
slice `$C1F7A0-$C1F837`. It processes the negative control word in `D0`,
recognizes `$FFFF`, records bits `$2000`, `$4000`, and `$8000` in stack locals,
and uses bit `$1000` to choose between a direct longword pointer and a
base-plus-`$0FFF`-masked word offset. It then enters the established
`$C1F906` record-dispatch loop.

The slice is observed on the human-flight display path; all exits to the
enclosing walker remain named external branches.
