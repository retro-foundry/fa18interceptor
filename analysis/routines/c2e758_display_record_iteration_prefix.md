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

Both helpers use the shared `$C2EC36-$C2EC67` tail, reconstructed in
`source_amiga/observed/classify_adjusted_display_pair_bounds.asm`. It snapshots
`D0-D2` to `$C45AC6`, accepts only a non-negative `D2` bound with both signed
components within that bound, restores the saved registers, and returns status
zero for acceptance or one for rejection.

The static prefix ends immediately after the direct helper call so that the
unexecuted selector tail remains outside this bounded reconstruction.
