# Native primary and secondary record placement

`port/native_record_action_placement.c/.h` implements complete `$C2374C` and
`$C2377E`, including their shared copy, stores and launch-placement tail.
The native control-record scheduler invokes these owners directly for all
primary, secondary and paired placement routes. Periodic, dispatch and finish
are its three remaining explicit child boundaries.

The authority is the sealed original instruction graph. The readable
`port/game/selected_fire.c` and `port/game/flight_record_actions.c` corroborate
it, but the native owner rereads the live companion after the destination
copy and kind assignment. This matters when both identities refer to the same
record: assigning kind 0/1 changes the subsequent launch-table selection.
The 41-long prefix copy preserves the destination tail and writes through the
shared aircraft, geometry and root-level owners. Store consumption, mode
counters, warning/event publication, secondary mask gates and redraw fields
retain original widths and ordering. All three transformed deltas are computed
before position publication, with wrapped long accumulation and arithmetic
shift. The full carried launch-axis value reaches the later view owner.

Both entries assign destination kind 0 or 1 before the shared class-$30 tests.
No intervening instruction or child changes it. The audit seals the assignments,
intervening source graph and both tests before excluding `C2390A-C2391A` and
`C239E2-C23A1E`. These exclusions apply only to these complete entries;
manoeuvre/release can reach those arms and still need their native owners.
There are no reachable child contracts in this placement proof.

The descriptor bank is shared with native root placement. The source's first
pointer becomes the identified record-stream procedure tag; four data pointers
are resolved original-hunk references. The tag does not claim completion of the
record-stream renderer. The original-disk loader imports Hunk-16 rows 0/$14,
resolving six non-null references into Hunk 51. Launch tables begin at Hunk 17
+$DB2/$DD6. Their 78 unchanged table/adjacent bytes match the sealed source.
The first following instruction operand is relocated: larger store indices
require explicit `after` field owners for the adjacent numeric words. The loader
does not expose unrelocated pointer offsets as those words. Missing owners fail
with preceding stores retained. Runtime imports use the original disk, never
captured RAM.

Bootstrap shares the actual descriptor bank, warning/event owners, fire state,
view redraw and aircraft stores redraws. Placement's viewed-record comparison
reads the existing logical word pair, whose owner is the live aircraft identity;
it does not introduce a second cached viewed offset.

`python tools/recomp/check_native_record_action_placement.py` passes 16,384
complete calls (8,192 per entry) at all 172/172 original boundaries. All
Chip/Slow RAM matches except the CPU ABI stack `C7FD00..C7FF00`. Typed aircraft,
position and matrix owners, scene descriptors and carried axis are checked
independently. Fixtures include all sixteen callers, signed secondary limits,
mask/divisor gates, depleted/maximum stores, warning transitions, wrapped
arithmetic, table alternatives and 3,341 aliased launches. The original-disk
asset check verifies the unchanged bytes and resolved references separately.
See `analysis/figures/native_record_action_placement_checkpoint.json`.

MSVC Release game and affected contracts, strict GNU compilation, fourteen
affected CTests and the 503-file native build guard pass. Composed contracts
exercise ordered primary depletion and actual secondary/paired placement.
The historical bootstrap oracle retains its three whole-child contracts;
16,384 calls cover all 291/291 boundaries and verify parent sequencing and
the current shared bindings, while these
placement comparisons prove the new complete lower owners. Native main,
remaining startup/frame children and production adjacent data bindings remain
open.
