# Display-record selection handoff

Updated 2026-10-04. Implementation and targeted proofs pass; integration
validation remains pending. The last fully gated baseline is `0639fdee`.

`port/game/display_record_selection.c` implements both original display
parents C0D74A/C0D752 and the five pair writers C0DAA0/C0DAD0/C0DAD4/C0DADC/
C0DAE6. The parents retain rounded matrix products, four original candidate
pairs, actual corner-child results, all seven selection arms and original
accepted/rejected frame exits. Pair writers preserve signed word indexing,
mirrored screen coordinates and original store widths/order. The older
enclosing register replay is removed from glue_display_records/glue_batch55;
the five older pair adapters are removed from glue_batch5.

The source manifest seals 259 unique /229 shared instructions, 57 distinct
child contracts, original incoming callers, candidate bytes and three
oracle ownership arrays. C0DA38 is an arm within the parent frame, not a
separate callable owner. Counts remain 539 translated plus 75 source-only,
614 registrations and 543 source-timed; these are not completion criteria.

All 229,376 whole calls pass full registers, PC, SR, Chip/Slow RAM, ordered
Custom writes and terminal hardware state: 16,384 per owner for both
controlled and original children. Controlled coverage is 259/259. Original
coverage is 256/259; only C0D794/C0D7AC/C0D7BE rounding increments are missing
from the bounded positive-depth fixture, and all three are covered by the
controlled whole calls and local instruction proof. Actual original-child
fixtures cover every selection arm. Original non-returning corner behavior
remains documented by the existing corner/view suite.

The first large fixture matched CPU/RAM but failed controlled coverage
because flags correlated with the crossing selector. The corrected fixture
varies them independently; the failed log is retained locally. Full child
entry CPU/SR/RAM and exact child counts are compared independently.

ON/shadow/sandbox smoke passes 1,344 calls (64 per owner per mode). Local DMA
passes 259 instructions /8,288 cases. MSVC Release and GNU oracle builds
pass; all 41 older generator outputs and shared/protected files are unchanged.
Raw log hashes and implementation hashes are recorded in
`analysis/figures/native_display_record_selection_checkpoint.json`.

The following checks have **not** been run for this implementation: dispatch
at 1,024 cases per owner per mode, unchanged normal-C recording comparisons,
fresh combined DMA, full 614-row shadow/sandbox/sealed-RAM/poison gate,
isolated full live frames, and the bounded family/ALL timing probe. Do not
claim that the steering baseline's integration results validate this batch.
The recordings verifier intentionally still requires those larger dispatch
and normal-C reports. Resume with these commands, serializing GNU builds:

```text
python tools/recomp/audit_display_record_selection_source.py
python tools/recomp/check_display_record_selection_dispatch.py --cases 1024
python tools/recomp/check_whole_call_glue.py C0D74A C0D752 C0DAA0 C0DAD0 C0DAD4 C0DADC C0DAE6
python tools/recomp/check_active_planes_step.py --group all --cases 32 --bus
python scripts/build_recomp.py
```

Then run `scripts/recomp_ports_check.sh`, isolated
`scripts/recomp_live_check.sh` with PORTS_ONLY set to these seven entries, and
`scripts/probe_recomp_timing.py` with that group and ALL through 600 frames.
Retain explicit cold/incomplete/hardware rows and generic checker rejections.
Run `verify_display_record_selection_recordings.py` after those reports exist,
then update the checkpoint to reflect the completed integration gates.

Remaining game work includes C2C392 action-producer reconciliation, larger
parents, older partial adapters, C1612C graphics-wait integration and full
original cold/indirect/callback graph reconciliation. Additional original
owners C0E78A (count loop then OS child), C2F49C (line style and conditional
line child) and C500D8 (audio callback) have been read in ignored local logs;
none is newly implemented. C500D8 callback installation/callability still
needs direct original evidence. The whole-game goal remains incomplete.
Stop when game-function porting is complete and only Kickstart services and
timing remain, as instructed by the user.
