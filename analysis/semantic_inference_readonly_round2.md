# High-leverage read-only P-code inference, round 2

Five contracts were promoted in this pass. The selection favored routines
that clarify several callers or a shared state transition, not the shortest
available P-code packets. The strict packet screen found nine exports with
no memory stores on their captured paths; several were already explained
(notably the paired and single-angle trig lookups), and several were trivial
early exits. Larger P-code exports were then screened by observed function
membership. That grouping identifies candidates, **not** whole-function
purity: unobserved branches and tail-dispatched handlers remain separate.

| Priority | Address | Promoted meaning | Downstream explanation | Evidence boundary |
| ---: | --- | --- | --- | --- |
| 1 | [`$C2F688`](routines/c2f688_planar_pixel_pipeline.md) | Behavioral four-plane pixel address/mask preparation | Shared by primary, alternate, adjusted, and adjacent-pixel wrappers plus 16 mode handlers | Read-only **prefix**; complete pipeline writes Chip RAM. Checked in deterministic run060. |
| 2 | [`$C27456`](routines/c27456_candidate_plane_side_test.md) | Behavioral indexed-triple plane-side scan | Explains the geometry decision within `$C26EBE`'s candidate-record path | Three observed scores and loop/exit; negative-first-long route unobserved. |
| 3 | [`$C231A2`](routines/c231a2_paired_record_guard.md) | Behavioral paired-bank route selector | Shared by bank-9, bank-11, bank-13 update callers | Later focused pass resolved the zero/nonzero result and caller continuation. |
| 4 | [`$C265E8`](routines/c265e8_flagged_slot_scan.md) | Behavioral 20-slot flagged threshold decision | Feeds the parent flight-update true/false branch; table stride shared with `$C151CE` | Later focused pass resolved the match comparison and true caller route. |
| 5 | [`$C1FEF2`](routines/c1fef2_record_stream_stride_skip.md) | Behavioral counted `$34`-byte cursor skip | Names one indirect `$C1F942` record-dispatch operation | Complete return and 15-step cursor delta captured in attract replay. |

This is five new or materially sharpened behavioral contracts, **not** five
new source slices or five fully pure functions. Byte coverage and the older
aggregate `analysis/semantics.json` are not silently incremented here;
the latter needs its separate generated-coverage reconciliation. Follow-on
focused passes resolved the `$C27456` candidate handoff, `$C231A2`'s nonzero
continuation, and `$C265E8`'s match route. The next high-value work should
prioritize producers/consumers of the now-proven component-magnitude primitive
`$C1D974`, plus scenario-backed state ownership for the remaining record-bank
contracts, before stronger gameplay names are justified.
