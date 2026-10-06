# Current-record matrix parent timing

The active original caller is C25B66 (`advance_indexed_record_dynamics`), whose
live ON owner is `glue_C25B66_step`. At C25D9E it calls C2D408 and returns to
C25DA4. The parent is entered through the guest-PC registry; C2D408's side/depth
children now call C directly. The next dependency is the parent's fixed charge
and missing internal event boundaries, not its deleted leaf adapters.

A 600-frame isolated demo01 trace uses the existing boundary observer with
`FA18_BOUNDARY_RANGE=C25D9E-C25DA8`. Read-only tracing adds no guest accesses or
timing. No full replay was rerun. Commands, input/executable hashes, trace hashes
and all 28 call intervals are in `emulation_removal_matrix_parent_timing.json`.

| Observed boundary | OFF source | ON, only C2D408 |
| --- | ---: | ---: |
| Completed calls | 28 | 28 |
| Minimum cycles | 10,240 | 9,520 |
| Mean cycles | 12,532.57 | 9,521.57 |
| Maximum cycles | 20,622 | 9,530 |
| Calls crossing frame end | 3 | 2 |

Intervals include the original JSR and return-target bus settlement, so they are
not the child's instruction cycles alone. The native adapter charges 9,500
cycles after its atomic body. The source services events between instructions;
instruction fetches, ordered data accesses, arithmetic operands, nested callees
and DMA contention determine the actual interval. Its first observed call is
at frame 439, line 257: source returns at line 279, ON at line 278. Later
differences accumulate. The separate 800-frame frame comparison first differs
at frame 584 (488 non-fade pixels).

Keep the passing source call-result comparisons and direct C ownership. Recover
source-derived dynamic timing and original event boundaries before extending
this owner toward the frame loop. Do not substitute a capture-derived average
for 9,500 or use generated/reference execution as replacement game behavior.

This is discovery, with **zero removal delta**. The reused full-suite raw
minimum remains **38.4011%**; combined parity still fails, accepted CPU share
unavailable. Memory/chipset/boot **0%**, deletion **0/4**.
