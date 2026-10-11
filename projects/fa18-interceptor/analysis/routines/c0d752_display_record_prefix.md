# `$C0D752-$C0D7DF`: display-record preparation prefix

Authority: the return-bounded attract cockpit trace at
`pcode/raw/attract_cockpit_c2fede/`. `$C2FEDE` calls `$C0D752`, which returns
to `$C2FEF2`; this prefix executes before its `$C2E758` child.

`$C0D74A` is a separate observed entry that loads `$C45BEA` into `A2` and
branches into the common prefix at `$C0D758`; `$C0D752` instead loads
`$C45BD8`. The alternate selection's meaning remains unassigned.

The prefix takes four consecutive two-word inputs from `$C0D720`, combines each
with a shifted part of `$C45A66` and three-word groups at `$C45BD8`, and writes
three rounded shifted products into four records beginning at `$C4B390` with a
`$1A`-byte stride. It then clears four longwords at `$C4E854` and calls
`$C2E758`.

This establishes arithmetic/dataflow only. The source values, matrix role,
output records, scratch block, and display semantics remain unassigned.

Its `$C2E758` child now has a separately captured, byte-exact initialization
prefix in `source_amiga/observed/initialize_display_record_iteration.asm`.
That prefix establishes an eight-entry iteration over these prepared records;
it does not assign display semantics.

The complete `$C0D752-$C0DA9F` caller range is now reconstructed as adjacent
byte-exact slices: this preparation prefix; the `$C0D7E0` workspace-pair
selector; six selected record-emission branches; one fallback branch; and the
`$C0DA70` success/rejection returns. A verifier coverage check claims all 846
bytes in that range with no gaps. The split source files preserve readable
branch-level contracts without promoting the record data to a visual or
gameplay interpretation.

The run075 return-bounded capture at `build/run075_prepared_c0d752/` resolves
the count unambiguously: the `move.w #4` / decrement / `BGT` loop makes four,
not five, candidate records. It enters at `$C0D752`, invokes `$C2E758`, and
reaches `$C0D7E0` in 737 instructions. `port/display_record_candidates.{c,h}`
ports that four-record matrix prefix and its source rounding; the contract uses
the captured `$C0D720`, `$C45A66`, and `$C45BD8` inputs and `$C4B390` output.
The later iterator and selector are still separate, unscheduled native work.
