# Complete workspace selector helpers ($C1EBB0, $C1EC84)

2026-10-02. These are the two complete helper routines needed by the planned
$C1E540 selector-family parent. They include their tails shared with the
existing control-record variants $C1EBC0/$C1EC96; no prefix or internal label
is registered as an extra routine. The large parents remain outstanding.

## Original contracts and readable C

`control_records.c` now provides `workspace_record` and
`add_workspace_cell_steps`, alongside the existing record field reader:

| Entry | Complete original behavior |
| --- | --- |
| C1EBB0 (11 instructions) | Mask D1.w to its high byte, shift right three, add to WORKSPACE_RECORDS ($C48184), then read +0C/+10/+0E into D2/D3/D4, sign-extending both words. Preserve D1's high half; leave A2 at the selected record and final EXT.L flags. |
| C1EC84 (23 instructions) | Select the workspace record from the high byte at (A1); subtract frame column/row words from the record's +6/+8 low bytes with signed-word wrap, convert each difference by SWAP/ASR.L #2, and add to D2/D4. Return the last displacement in D1, restore A2, and retain the final ADD.L flags. |

The table offset is 32 times the unsigned high byte. The original has no
16-record bounds check, so none is invented. `cell_step` retains the existing
word-wrap-to-fixed-point convention; unsigned output addition preserves
32-bit overflow. CPU registers, CCR, stack and bus timing remain in
`glue_workspace_records.c`, outside the readable domain operations.

## Independent proof and cold classification

The complete readable whole-call adapters match original generated calls
on 16,384 fixtures per entry, 32,768 total. The oracle executes the original
entry and dispatches its shared tail until the complete RTS; it does not call
the new timing bridge as its reference. It compares every register, full SR,
PC and all Chip/Slow RAM, excluding only the returned stack's saved A2/return
words. Fixtures cover all 256 selector high bytes, varying low bytes and
register high halves, all 32 CCR combinations, all pairs of eight signed-word
edge values, random masked cell words, and 32-bit carry/overflow operands.

```text
python tools/recomp/check_workspace_records.py
python tools/recomp/check_active_planes_step.py --group workspace_records --bus
```

The independent DMA instruction oracle matches all 34 instructions on 1,088
cases, including registers, full SR, PC, cycles and RAM. Together with earlier
independent groups, this totals 9,696 instructions and 310,272 cases. The
combined oracle was not rerun for this local group. GNU and MSVC Release
builds pass. The short isolated 500-frame probe is exact; ALL retains its
frame-416, 361-pixel difference.

These entries are cold in all three sealed recordings. The temporary
whole-call registry tool reports zero completed calls for both entries in
all six shadow/sandbox runs and then rejects the proof with "no completed
comparisons". That rejection is retained; no recorded comparison is claimed
as proof of either helper. The complete-call structural oracle above supplies
their independent readable-C evidence. Future parent activation must exercise
and verify their child contracts again.

The full 421-entry gate has zero mismatches, 703,337 completed shadow matches
and 1,110,694 sandbox matches, exact sealed final RAM and identical poison
frames. Both helpers have zero calls, zero matches and zero incomplete calls.
Their isolated live selector also leaves every one of the 36,236 RGB444
frames and sealed final RAM unchanged. This is a nonregression check for
cold entries, not a claim that the recordings execute their new code paths.
The added functions increase readable source coverage by two; they do not
increase observed call coverage. There are 190 timing-step entries.
