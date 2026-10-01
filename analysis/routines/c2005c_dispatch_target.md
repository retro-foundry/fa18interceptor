# Table-dispatch target at `$C2005C` (Hunk 13 +`$374`)

Classification: **registered, source-timed C bridge**. This is a complete,
bounded target of the `$C1F942` indirect record dispatcher. The table/data
meanings are unassigned.

## Runtime packet

- No-input `start_demo` replay from `captures/uae/baseline_menu/state.bin`.
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

## Source-timed proof

`glue_C2005C_step` covers the complete `$C2005C-$C200F4` parent. Its registered
`$C1FB82` face-test and `$C2469E` clipper children dispatch at their original
call boundaries, while the parent preserves variable vertex-list traversal,
stack saves, register effects, and return paths.

The direct instruction oracle matches 54 instructions over 1,728 fixtures.
The combined bridge oracle matches 1,232 instructions over 39,424 fixtures;
both compare registers, full SR, PC, cycles and RAM. Fresh isolated
`PORTS_ONLY=C2005C` output matches every RGB444 frame across the three sealed
recordings. GNU and MSVC Release builds pass. The full 414-entry gate matches
721,752 shadow calls and 1,169,653 sandbox calls with zero mismatches, sealed
RAM, and identical poison frames.

The complete registered path still first differs at demo frame 416 by 361
pixels because C212B0, C332BC, C23CA6 and C246A0 independently differ at the
same frame in isolation.
