# Complete selected segments, projection and view crossings

Twelve original game functions are complete in
`port/game/segment_projection.c`, with CPU effects and actual child calls in
`port/game/glue/glue_segment_projection.c`. Their complete original bytes,
incoming calls and twenty child contracts are sealed in
`analysis/data/segment_projection_source_scope.json`. The evidence checkpoint
is `analysis/figures/native_segment_projection_checkpoint.json`.

| Original entries | Complete behavior |
| --- | --- |
| C1FF9C, C1FFA4 | Copy the selected workspace pair, preserve original early exits and saved registers, and invoke the actual A4 child |
| C2ED70 | Test both original points, project and clamp both axes before mirroring, and consume the actual line child |
| C2EE4A | Full four-plane decision chain, actual crossing returns and CLIP_POINT, original point exchange, local counter, error word and line child |
| C2F0C6, C2F0F4, C2F128, C2F156 | Truncated crossings, exact saved working words and original parallel loops |
| C2EA5A, C2EAD0, C2EB4C, C2EBC2 | Rounded crossings, signed remainder adjustment and unchanged parallel-case CLIP_POINT |

All twelve upgrade existing registrations. The registry remains 527 translated
plus 73 source-only entries: 600 rows. Source-timed entries rise to 521
(448 translated plus 73 source-only). Timing-adapter presence alone does not
certify other older functions.

The scope has 663 unique / 60 shared instruction boundaries and twenty distinct
child sites. Both selected targets are proven by their original LEA instructions
and JSR (A4) at C1FFF6. Original selector-table slots C1FDA8 and C1FE10,
their actual callers and recorded indirect edges establish incoming callability.
This evidence does not claim a complete selector-table domain or add a guard.

All 393,216 whole CPU/PC/full-SR/all-RAM calls pass: 196,608 with controlled
children and 196,608 with original children. Controlled calls cover all 663
boundaries, including all 211 in C2EE4A. Every child entry compares complete
CPU/SR/RAM, and each returned child state is consumed once. Successive child
Z results vary independently to cover opposite-plane and other-axis decisions.
Original-child coverage is 610/663; every owner's missing PCs remain explicit.
Ordered Custom writes and terminal registers, effective flags, blit counters
and data latches are checked independently.

The original unrounded parallel branches C2F0DC, C2F10E, C2F13E and C2F170
remain non-returning. A separate production-domain/observer proof passes 65,536
observations of 64 original branches each, comparing all registers, PC, full SR
and all RAM. Its test observer ends observation; it never supplies a game return
or inserts a production loop limit. An extreme original clipped fixture entered
such a loop and failed the returning proof's instruction budget. That failure
is retained; completed original-child fixtures use a bounded returning domain.
Rounded zero-denominator exits retain the original untouched CLIP_POINT.

Whole original-child proofs execute actual source bytes with event service
held; devices retain their actual clock and busy reads. Each of the four
parents starts an actual initial blit on odd scenarios. Actual ON/shadow/sandbox
dispatch separately passes 36,864 calls through production generated-child
continuations. Each mode classifies 12,288 hardware-free / zero hardware-bearing
calls and validates 21,852 actual Custom packets. These classifications and
packet observations are distinct. No production glue invokes opcode handlers.

The unchanged whole-C recording checker passes every owner: 106,838 shadow /
126,128 sandbox matches, with zero mismatches. It retains original incomplete
and hardware classifications. The full 600-row gate passes 568,446 shadow /
443,870 sandbox comparisons, exact recording seals and identical poison frames.
All 36,236 isolated live RGB444 frames and final RAM seals match fresh source OFF.

Local DMA checks pass 663 instructions / 21,216 cases. The fresh combined run
passes 29,042 / 929,344; an independent union has previous 28,931 plus 663,
overlap 552. All thirty-seven older generator outputs remain byte-identical.
Shared runtime, core, bus, arithmetic and observers are unchanged. Unused
selected/projected/clipped replay helpers are removed; the remaining C1F2EE
and edge replay exports retain identical bodies. GNU and MSVC Release builds
pass; `build/` is 1.921 GiB after temporary frame cleanup.

The family matches source frames through 600. ALL remains first different at
424 / 34,144 pixels. Copper fade, Kickstart and combined timing remain deferred.
Larger corner/view owners, other older partial adapters, C2C392's computed
transfer, C1612C's graphics-wait integration and the complete original
cold/indirect/callback graph still prevent claiming game-function completion.

Reproduce from the repository root:

```text
python tools/recomp/audit_segment_projection_source.py
python tools/recomp/make_command_dispatch_step.py --family segment_projection
python tools/recomp/check_segment_projection.py --cases 16384
python tools/recomp/check_segment_projection_parallel.py --cases 16384
python tools/recomp/check_segment_projection_dispatch.py --cases 1024
python tools/recomp/check_active_planes_step.py --group segment_projection --cases 32 --bus
python tools/recomp/check_active_planes_step.py --group all --cases 32 --bus
python tools/recomp/check_whole_call_glue.py C1FF9C C1FFA4 C2ED70 C2EE4A C2F0C6 C2F0F4 C2F128 C2F156 C2EA5A C2EAD0 C2EB4C C2EBC2
python tools/recomp/verify_segment_projection_recordings.py
```
