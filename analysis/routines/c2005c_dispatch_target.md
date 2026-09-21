# Table-dispatch target at `$C2005C` (Hunk 13 +`$374`)

Classification: **structural**. This is a complete, bounded target of the
`$C1F942` indirect record dispatcher. The table/data meanings are unassigned.

## Runtime packet

- No-input `start_demo` replay from `captures/baseline_menu/state.bin`.
- Breakpoint `$C2005C`, frame 601; return boundary `$C1F944`.
- 76 instructions; terminates at the requested return boundary.
- Ghidra P-code: `pcode/raw/no_key_c2005c_dispatch_target/`, 76 observed RAM
  starts, 566 P-code operations; every start maps to a verified Hunk.

## Observed contract

The packet clears `$C4BF90`, then uses three signed word offsets consumed from
`(A2)+` to copy three longword-plus-word groups from the table rooted at
`$C48390` into the area following `$C4BF90`. It combines the third word of
each copied group into `D6` using `AND`.

For this attract invocation it then consumes another word, increments local
`-$32(A6)`, calls `$C1FB82`, and returns `$FFFF` in `D0` at `$C200F2`. The
dispatcher therefore takes its observed negative-status branch back to
`$C1F7FA`. This establishes only the data movement and return status for this
one recorded path; it does not establish record, table, or output ownership.
