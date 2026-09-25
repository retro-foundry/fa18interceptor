# `$C1D974`: table-assisted three-component magnitude

Classification: **runtime-backed behavioral math primitive**. Callers supply
three component words in `D2/D3/D4`; the helper returns a nonnegative capped
scalar in `D1` and stores its low word at `$C45B40`.

The helper first orders `D2/D3`, then uses the word table rooted at `$C1D9D8`
with a scaled component ratio to form an intermediate planar scalar. It orders
that intermediate scalar with `D4`, repeats the ratio-table process, shifts
the product by fourteen, and caps any result above `$7FFF`. The table begins
`$4000,$4000,$4000,$4001,...`, consistent with a fixed-point ratio magnitude
lookup. This is an implementation-level magnitude contract; it does not
assign units or a game-world distance.

`$C265E8` calls it twice after deriving absolute component triples. In the
forced-slot run060 trace those calls return `$20BC` and `$4000`, which are the
two operands of the scan's signed threshold compare. The same helper is also
called by `$C2577E`, making this a higher-leverage primitive than the slot
scan alone.

Evidence: exact code `$C1D974-$C1D9D6` in the run060 slow-memory authority,
the P-code subsets `observed_call_00c1d974`, and
`build/run060_c265e8_forced_match/trace.jsonl`. The contract has not yet been
promoted to a byte-exact source slice; division-by-zero and table-bound edge
cases need a dedicated fixture before that conversion.
