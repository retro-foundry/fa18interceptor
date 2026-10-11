# Complete control-record readout and magnitude owners

Six original owners are complete in `port/game/control_readouts.c`, with
CPU/stack adapters and source-boundary continuations in
`port/game/glue/glue_control_readouts*`. C12950 and C131BE are newly
registered; four older adapters are replaced. Registration is 539 translated
plus 73 source-only entries, 612 total, 537 source-timed. These counts do
not establish whole-game completion.

| Original owner | Complete game behavior |
| --- | --- |
| C12950 | Selected-record publication, engine-start/stop state, prioritized voice messages, countdown sounds, action dispatch and control readouts |
| C131BE | Original magnitude conversion, view/count factors, both guarded four-way branches, division and record-flag damping |
| C13176 | Event/sound selection, original child results and countdown clearing |
| C133B2 | Record-dependent word magnitude, view scaling and minimum step |
| C13396 | Original five-eighths scaling and argument replacement |
| C52EC8 | Full restoring long division, original quotient/remainder, sign and zero behavior |

The audit seals 867 unique instructions, no shared instructions within this
batch, 43 distinct child contracts/sites, three oracle ownership arrays and
actual original incoming calls. C131BE includes 48 cold instructions beyond
the older report. The eight computed branch stubs at C13256/58/5A/5C and
C132BE/C0/C2/C4 retain the C131BE LINK frame and belong to that owner.
They are not separate callable registrations. The original unsigned and
signed guards bound each dispatch to its four original targets.

All 196,608 completed whole calls match every register, PC, full SR, all
Chip/Slow RAM, ordered Custom writes and terminal device state: 98,304 with
controlled children and 98,304 with original children. Each union covers
866/867 instructions. Every owner except C133B2 has full coverage in both
sets; C133B2 covers 33/34. The remaining C133FA clamp is unreachable from
valid entry: exhaustive testing of all 65,536 original word inputs shows
NEG.W followed by ASR.W #9 produces -64 or 0..63. The signed upper-bound
branch therefore always skips that clamp. Its native body and timing
boundary remain implemented and independently DMA-tested.

The controlled provider checks each child entry contract and the exact
completed child count, and independently varies returned registers, full SR
and RAM. C12950 preserves the caller-visible stack write at C130F0. The
division retains the 32 original iterations, ROXL through X, saved registers,
dead stack writes and original overflow behavior. No host division exception,
new selector bounds, loop caps or substitute game results are supplied.

Actual dispatch passes 18,432 calls, 1,024 per owner in ON/shadow/sandbox.
Each mode classifies 6,144 source calls as hardware-free and observes 5,376
ordered Custom writes, including fixture activity. Admission classifications
and observed device packets remain separate measurements.

The unchanged normal-C checker has zero mismatches: four owners have completed
comparisons, totaling 15,961 shadow /16,016 sandbox. C12950 has 122 incomplete
shadow calls; C13176 has one incomplete shadow call and one completed isolated
sandbox comparison; C52EC8 has one incomplete shadow call. C133B2 and C13396
are uncalled in the recordings and have separate whole-call evidence. The
generic rejection `C133B2: no completed comparisons` remains in the raw log.

The full 612-row gate passes 571,427 shadow /458,087 sandbox comparisons,
three exact RAM seals and identical poison frames. All 36,236 isolated live
RGB444 frames and final RAM seals match. Local DMA passes 867 instructions /
27,744 cases; fresh combined DMA passes 30,167 /965,344, independently
reconciled as previous 29,300 plus 867 with zero overlap. All 39 older generator
recipes produce unchanged outputs. Shared core, observers, previous corner
implementation and protected native-check files remain unchanged; historical
committed command-dispatch output is retained exactly. GNU headless and
MSVC Release builds pass. Build artifacts occupy 2.015 GiB.

The initial full C12950 comparisons passed but did not cover all branches;
the coverage check rejected that run. Independent valid caller-input witnesses
complete final coverage with both child providers. Earlier logs remain sealed.
Family timing matches through frame 600. ALL still first differs at frame
424 /34,144 pixels; timing and Copper fade remain deferred.

Next game work includes C2C392's action dispatcher and steering helpers,
other larger parents and older partial adapters, C1612C graphics-wait
integration, and complete original cold/indirect/callback reconciliation.
Forty contiguous original action pointers are read, but their producer domain
still requires evidence; table extent alone cannot justify selector bounds.
Kickstart services remain deferred. The user's game-function stopping point
has not been reached.

Exact source scope, per-owner coverage, packet counts, recording and raw log
hashes, implementation hashes and the unreachable-path proof are in
`analysis/figures/native_control_readouts_checkpoint.json`.

```text
python tools/recomp/audit_control_readouts_source.py
python tools/recomp/check_control_readouts.py --cases 16384
python tools/recomp/check_control_readouts_dispatch.py --cases 1024
python tools/recomp/check_active_planes_step.py --group control_readouts --cases 32 --bus
python tools/recomp/check_active_planes_step.py --group all --cases 32 --bus
python tools/recomp/verify_control_readouts_recordings.py
```
