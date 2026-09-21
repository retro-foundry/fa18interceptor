# `$C2E758-$C2E7D3`: display-record iteration prefix

Authority: `build/attract_cockpit_c2e758_trace/`, captured with no future
input from the established attract cockpit state. The breakpoint at `$C2E758`
hits on frame 4, executes 579 instructions, and returns to `$C0D7E0`. The raw
P-code authority is committed at `pcode/raw/attract_cockpit_c2e758/`.

This byte-exact prefix initializes `A0=$C4B990`, `A1=$C4B390`, and an eight
entry (`D0=0..7`) loop. Each pass selects two wrapped neighbour indices based
on the parity of `D0`, scales the indices by `$10`, and derives an eight-byte
workspace slot at `$C4B990 + 8*D0`.

The observed path clears the local byte flag, loads three words from the
selected `$10`-stride record, adjusts two of them, checks the selected
neighbour record, and calls `$C2EA5A` on the passing path. The following
control-flow and the helper's display meaning remain unassigned. In
particular, this does not establish that the records represent screen objects
or pixels.

`$C2EA5A-$C2EACF` is separately reconstructed in
`source_amiga/observed/adjust_display_record_pair.asm`. It saves `D0-D6`,
loads a three-word neighbour tuple, performs signed multiply/divide adjustment
with remainder-sensitive rounding, then branches to the common status tail at
`$C2EC36` or `$C2EC58`. This establishes arithmetic and control flow only.

The second observed helper call is `$C2EAD0-$C2EB4B`, reconstructed in
`source_amiga/observed/adjust_negated_display_record_pair.asm`. It follows the
same arithmetic pattern but negates its initial inputs and final candidate;
the routine is consequently documented as a polarity variant, not as a
geometric or display operation.

`$C2EB4C-$C2EBBF`, reconstructed in
`source_amiga/observed/adjust_transposed_display_record_pair.asm`, is a
statically decoded third helper form, reached by the surrounding selector but
not entered in this trace. It uses the same signed
multiply/divide-and-rounding pattern while deriving its candidate through
`D1/D4` and returns through the shared bound classifier. Its role is likewise
kept at arithmetic/dataflow level.

The final static sibling `$C2EBC2-$C2EC35` is reconstructed in
`source_amiga/observed/adjust_negated_transposed_display_record_pair.asm`.
It negates the transposed inputs and final candidate, then falls through to the
same `$C2EC36` classifier. This helper was not entered by the focused trace.

Both helpers use the shared `$C2EC36-$C2EC67` tail, reconstructed in
`source_amiga/observed/classify_adjusted_display_pair_bounds.asm`. It snapshots
`D0-D2` to `$C45AC6`, accepts only a non-negative `D2` bound with both signed
components within that bound, restores the saved registers, and returns status
zero for acceptance or one for rejection.

When one of the selector paths reaches `$C2E9F8-$C2EA59`, reconstructed in
`source_amiga/observed/project_adjusted_display_pair.asm`, the shared triplet
at `$C45AC6` is converted to a pair using signed divide, carry-sensitive
rounding, offsets `$A0/$5A`, and clamps `$000..$13F` and `$000..$B3`.
The pair is stored through `A3`, then the enclosing eight-entry loop advances.
This proves the numerical transform and bounds, while the screen/display role
of the output remains unassigned.

The first continuation case, `$C2E7D4-$C2E7FB`, is reconstructed in
`source_amiga/observed/dispatch_first_display_record_case.asm`. It branches on
the preceding helper status, tests `$C45ACA`, conditionally calls the negated
helper `$C2EAD0`, and on its zero result increments `$C4E85A`, writes `D0` to
`$C4E862`, then enters the shared projection tail. These are dataflow facts;
the counters and slots remain unassigned.

The adjacent `$C2E7FC-$C2E833` case is reconstructed in
`source_amiga/observed/dispatch_second_display_record_case.asm`. It derives a
negated bound from `D3`, reads two words from the selected neighbour record,
and routes through `$C2EAD0` and conditionally `$C2EA5A` under the `$C45ACA`
signed gate. Its branch destinations and dataflow are exact; record meaning is
not assigned.

The `$C2E834-$C2E887` case is reconstructed in
`source_amiga/observed/dispatch_third_display_record_case.asm`. It gates on
signed `D4`, runs the two transposed helpers (`$C2EBC2` then `$C2EB4C`), and on
the passing result increments `$C4E854` and stores `D0` at `$C4E85C` before
projection. These counter/slot roles are dataflow-only.

The `$C2E888-$C2E8D9` case is reconstructed in
`source_amiga/observed/dispatch_fourth_display_record_case.asm`. It applies the
opposite signed comparison order, invokes `$C2EB4C` and conditionally
`$C2EBC2`, then increments `$C4E858` and stores the result at `$C4E860` on the
passing path. Meaning remains dataflow-only.

The static prefix ends immediately after the direct helper call so that the
unexecuted selector tail remains outside this bounded reconstruction.
