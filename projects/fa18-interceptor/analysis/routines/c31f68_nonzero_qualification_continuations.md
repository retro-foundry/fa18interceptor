# `$C31F68` / `$C31F72`: qualification-record continuations

Classification: **static dataflow**.

The byte-exact source is in
`source_amiga/observed/negate_qualification_record_value.asm` and
`source_amiga/observed/guard_nonzero_qualification_mode.asm`.

`$C31F68` negates the word loaded by the preceding `$C31F4C` record gate.
After the known `$C31F6A` mode test, its nonzero path at `$C31F72` requires a
nonzero byte at `$C457D9` and bit 6 at `$C458CD`; otherwise it returns at
`$C31F92`. When both guards pass, it joins the shared quotient/line-formatting
path at `$C31F94`.

The existing observed `$C31F4C` gate reaches the mode test, but these two
continuations have no independent runtime branch oracle. The fields are
therefore neutral guards, not proven qualification status, landing result, or
pass/fail conditions.
