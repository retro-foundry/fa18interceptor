# Complete record roll and pitch steering owners

Six original owners are implemented in `port/game/record_steering.c`, with
CPU adapters and source-boundary continuations in
`port/game/glue/glue_record_steering*`. Four older adapters are replaced;
C2CB82 and C2CBBC are original source-only callable peers. Registration is
539 translated plus 75 source-only entries, 614 total, 543 source-timed.
These counts do not establish whole-game completion.

| Original owner | Complete game behavior |
| --- | --- |
| C2CA26 | Record flags, signed turn/rate comparisons and heading thresholds select original roll controls |
| C2CA92 | Signed requested turn selects left, neutral or right controls |
| C2CAA0 | Neutral roll, retaining the original low two control bits |
| C2CB86 | Signed pitch/rate comparison and normal control replacement |
| C2CB82 | Original minus-one D1 prefix selects pitch while retaining other control bits with mask CF |
| C2CBBC | Original backward branch into the full normal pitch helper |

The source audit seals 72 unique /34 shared instructions, actual incoming
BSR bytes (including eight cold C2CBBC callers), both source-only prefixes,
two oracle ownership arrays and fixture caller contracts. There are no child
calls in these owners. CPU register high words and every original SR flag
are retained; the readable native functions publish each original low-byte
or low-word replacement rather than rebuilding partial outputs afterward.

All 98,304 completed whole calls match every register, PC, full SR, all
Chip/Slow RAM, ordered Custom writes and terminal device state. Union coverage
is 72/72. C2CA26/C2CA92/C2CAA0 cover 48/48, 13/13 and 6/6. C2CB86 covers
19/21, C2CB82 covers 21/22, and C2CBBC covers 20/22. The missing mask arms
are unreachable from their respective entries and covered by the counterpart:
zero or minus-one MOVEQ followed by MOVE.B leaves D1's low word nonnegative
or negative for all 256 control-byte values. This selects mask 3 or CF exactly.
The skipped source bodies remain implemented and independently DMA-tested.

Actual dispatch passes 18,432 calls, 1,024 per owner in ON/shadow/sandbox.
All 6,144 calls per mode are hardware-free, with zero observed Custom writes.
The initial C2CBBC ON fixture exposed an incorrect continuation range: its
backward branch into C2CB86 escaped that range. The registration now includes
the whole shared body, and all modes pass. The shared runtime is unchanged;
the earlier failed dispatch log remains sealed.

The unchanged normal-C checker has zero mismatches and 162 completed shadow
and 162 completed sandbox comparisons across four existing owners. The two
source-only peers are uncalled in the recordings and have separate whole-call
proofs. The generic rejection `C2CB82: no completed comparisons` is retained.

The full 614-row gate passes 571,427 shadow /458,087 sandbox comparisons,
three exact RAM seals and identical poison frames. All 36,236 isolated live
RGB444 frames and final RAM seals match. Local DMA passes 72 instructions /
2,304 cases; fresh combined DMA passes 30,239 /967,648, independently
reconciled as previous 30,167 plus 72 with zero overlap. All 40 older generator
outputs, shared core/observers, protected native-check files and previous
control/readout implementation remain unchanged. Unrelated batch48 exports
remain exact. GNU headless and MSVC Release builds pass.
Build artifacts occupy 2.041 GiB.

Family timing is exact through frame 600; ALL remains at frame 424 /34,144
different pixels. Copper fade, timing parity and Kickstart services remain
deferred under the user's stopping instruction.

The next readable game batch is the complete display-record parents
C0D74A/C0D752 and their five pair writers. Their 259 unique /229 shared
instructions are read; each parent has 57 original child sites. Replace the
older register replay with original child results and preserve shared frame
exits. C2C392 still requires action-producer reconciliation: the eight original
stream descriptors publish 35, 11, 39, 14, 17, 20, 23 and 26, and the packed
C2BB98-C2BCC8 parameter pairs publish 2, 5 or 9 for nonnegative bounded scene
limits. Record copies and saved-state producers remain to inspect. No new
selector guard or parent implementation is supplied by this batch. Larger
parents, older partial adapters, C1612C graphics-wait integration and full
original cold/indirect/callback reconciliation remain game work.

The separate read is reproducible with `read_action_dispatch_producers.py`;
`analysis/data/action_dispatch_producer_read.json` seals all 255 nonzero byte
indices, 40 pointers, 76 parameter pairs and eight stream descriptors. It
explicitly retains incomplete producer-domain and whole-parent status.

Exact source scope, per-owner coverage, hardware counts, raw logs, recording
hashes and implementation hashes are in
`analysis/figures/native_record_steering_checkpoint.json`.

```text
python tools/recomp/audit_record_steering_source.py
python tools/recomp/check_record_steering.py --cases 16384
python tools/recomp/check_record_steering_dispatch.py --cases 1024
python tools/recomp/check_active_planes_step.py --group record_steering --cases 32 --bus
python tools/recomp/check_active_planes_step.py --group all --cases 32 --bus
python tools/recomp/verify_record_steering_recordings.py
```
