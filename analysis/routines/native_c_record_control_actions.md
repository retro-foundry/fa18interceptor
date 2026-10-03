# Complete control-record actions and alert consumers

Updated 2026-10-03. C153FC/C15688/C159AE/C15AD4/C181A0 are newly registered.
C15138's older fixed-charge adapter is replaced with complete CPU/SR and source
timing. The registry has 488/624 translated entries plus 66 original source-only
entries: 554 rows and 370 timed entries (304 translated plus 66 source-only).
Game-function porting remains incomplete. C1612C's frozen graphics-wait
integration remains explicitly open; this batch replaces no Kickstart service.

The original sealed `captures/native/demo01/state.bin` is authoritative.
`analysis/data/record_control_actions_source_scope.json` audits all six complete
owners: 537 unique boundaries, zero shared boundaries and ten actual child
sites. Original JSR/BSR sites establish each callable entry, including the
earlier control/message parents and the control-input parent for C15138.

| Owner | Complete behavior |
| --- | --- |
| C153FC | Advance live control records through original collision/ground checks; update flags and expiry; wrap X/Z coordinates and cell words once at the original bounds; apply inherited velocity when flagged; retain three distinct render calls and matching-selection clearing. |
| C15688 | Select original launch coefficients/lifetimes, transform position through the original matrix, publish cell/origin coordinates, select special or mode-dependent velocity coefficients, transform velocity, clear the orientation selector and call the direction initializer. |
| C159AE | Select mode-dependent heading offset, sample indexed orientation, retain special heading flags, calculate wrapped relative coordinates, narrow their shifted words and pass the four original direction arguments. |
| C15AD4 | Retain the original vector projection child, reload its three output words, publish scaled/unscaled direction fields and optional inherited velocity contributions, and clear the record word. |
| C181A0 | Retain the null sound-request guard, actual reservation/start children, the reloaded request pointer, original sample/duration publication and request activation order. |
| C15138 | Preserve word addition, signed magnitude and arithmetic attenuation thresholds, write both original frame locals and the caller's argument slot, then return the sign-extended result with complete SR. |

Readable behavior is in `port/game/record_control_actions.c`; CPU outputs,
register saves and actual children are in its normal adapter. Matrix phases
retain original evaluation order and register halves. Domain code owns vector
stores. Original stack locals, MOVEM saves, child arguments/return PCs, signed
conditions, word/long wrapping, pointer reloads and source writes are retained.
The superseded C15138 adapter is removed from `glue_batch5.c`; its public
fixed-math function retains the same existing calculation.

147,456 complete CPU/PC/full-SR/all-RAM cases pass without exclusions: 8,192
controlled-child and 16,384 real-child cases per owner. Controlled cases cover
all 185/147/80/72/28/25 owned boundaries and compare complete CPU/SR/RAM at
every actual child entry. They exercise both selection-clear branches, all
coordinate wrap branches, launch/mode flags, signed arithmetic edges and
projection/sound pointer reloads. Test-only contracts scramble caller registers
and CCR, alter collision outputs, replace selected-record/flags pointers,
publish projection output words and change the reserved sound request pointer.
Production always retains actual original children.

Real coverage per owner is 33/185, 108/147, 70/80, 72/72, 5/28 and 25/25.
Launch and aim real fixtures retain their actual projection/direction children;
the observed raw PC counts also include nested owners, and only intersections
with each owner's source PCs are reported here. The advance fixture uses the
original expiry branch; the alert fixture uses the original null guard.
Their other paths are covered by controlled contracts, not complete real
collision/render/sound-service proofs. Kickstart and hardware timing completion
are not inferred from these structural cases.

Actual ON/shadow/sandbox dispatch passes 4,608 complete fixtures and all
non-call, OFF, selection and source-write invalidation guards. ON requires
native continuation. The real fixtures are hardware-free and both reference
modes require exact completed classifications, with no relaxed exclusions.

Five new owners are cold in all six step-disabled recording reports.
The generic checker correctly rejects C153FC for zero completed comparisons;
its assertion and rejection remain unchanged. C15138 passes 12,907 shadow /
12,926 sandbox independent readable-C matches, with 19 incomplete shadow calls,
zero sandbox incompletes and no hardware or mismatched calls. Its separate
unchanged generic checker passes. Recording
proof does not establish cold-owner execution; structural/dispatch layers do.

Local instruction/DMA proof passes 537 instructions / 17,184 cases with CPU,
SR, PC, cycle and full-RAM comparison. The independently proven union is now
18,889 / 604,448 with zero new overlap. Memory NEG.W and arithmetic direction
recipes are family-local; all seventeen older generator outputs remain
byte-identical. Shared runtime CPU/bus/math/instruction fixtures are unchanged.
No fresh combined run was made; the last fresh combined remains 16,384 /
524,288.

GNU and MSVC Release builds pass. The full 554-row gate passes 555,565 shadow /
417,331 sandbox comparisons, zero mismatches, exact RAM seals and identical
poison frames. All 36,236 isolated live frames/seals match; the group is exact
through frame 600. ALL remains 416/361, with fade deferred. Evidence hashes,
raw reports and owner coverage are in
`analysis/figures/native_record_control_actions_checkpoint.json`.

Next complete the control/flight parents C149BE/C23A7E and upgrade their
existing helpers C083E2/C25754/C24568/C2436A. The six-owner original scope has
1,416 unique / zero shared boundaries and is sealed in
`analysis/data/main_loop_flight_controls_scope_inventory.json`. The inventory
implements none of those changes. Continue game functions, including cold and
indirect owners, until only Kickstart and timing issues remain.
