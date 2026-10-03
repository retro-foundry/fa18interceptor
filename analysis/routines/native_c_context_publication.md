# Complete context publication and selected-record helpers (2026-10-03)

`port/game/context_publication.c` recreates complete entries C1B7A6,
C1BEE8, C1C214, C083A6 and C09DD0. Shared view changes and event queue
publication use the existing readable domains. Internal C1B906/C1B9CC view
continuations and C083B6 selected-record body are not additional routines.
CPU adaptation and resumable source timing remain in the glue directory.

## Source behavior

`analysis/data/context_publication_source_scope.json` seals 153 unique original
instructions, including 28 shared boundaries. The static audit follows every
branch and jump through its return, treating calls as child boundaries and
checking generated scopes against sealed original bytes. Reproduce with
`python tools/recomp/audit_context_publication.py`.

C1B7A6 sets context detail to four before publishing the event. C1C214 toggles
the byte addressed by A0 before publishing. C1BEE8 preserves the full update,
selection, fire-state and indexed-view setup. Its index shift wraps at word
width; the source zoom child returns both the event and signed record offset.
The owner consumes those changed outputs before reading the record's angle
and type. The non-special type uses the complete zero-view continuation.
The special type sets both span origins and invokes C1BA86, whose complete
body already publishes the event. The parent then stores the final row and
reaches the publication tail again. Both visits remain; KEY_TAKEN determines
whether another queue write occurs. Signed queue indices and overlapping
globals preserve source write order.

C083A6 saves D4/D7/A1 with the original MOVEM stack order, forces byte level
one, and enters the shared C083B6 body. The body preserves signed selected
record addressing, the lock bit and set/clear bit-seven paths. Its clear path
is unreachable from the forced-one entry but remains implemented and separately
proven with the enclosing save frame. C09DD0 saves D0, checks the selected
record sentinel and equality, invokes the real selection-tone child, then
clears the selection and masks the original warning causes. Stack bytes,
register high words and full condition flags are preserved by the adapters.

## Independent readable-C proof

`python tools/recomp/check_context_publication.py` provides three distinct
original-byte comparisons of the independent normal C adapters:

- Five complete entries with real source children: 16,384 calls each,
  totaling 81,920 full CPU/RAM cases.
- The shared C083B6 body with its original save frame: 8,192 cases covering
  every byte level and all 32 initial CCR combinations.
- C1BEE8 and C09DD0 with controlled child contracts: 8,192 cases each,
  totaling 16,384. Contracts compare complete child-entry CPU/RAM and provide
  changed register/global results to expose otherwise cold parent branches.
  They do not replace production children or patch original parent bytes.

Together these 106,496 cases cover all 153 owned boundaries. Every comparison
checks all registers, PC, full SR and all Chip/Slow RAM, including stack bytes,
without exclusions. Real-child evidence and controlled-contract evidence remain
separate in the checkpoint. The shared view-helper refactor additionally passes
32,768 component cases over 247 boundaries and fresh whole-call proof of
C1AC28/C1AD74: 798 shadow / 891 sandbox comparisons with timing disabled.

All five new entries have zero completed calls in all three sealed recordings,
even with timing disabled. `check_whole_call_glue.py` correctly rejects the
group with `C1B7A6: no completed comparisons`; the rejection and raw reports
remain retained. The original-byte structural tests establish their readable-C
proof. Recorded live checks provide nonregression evidence for these cold entries.

## Timing and integration

`python tools/recomp/check_active_planes_step.py --group context_publication --bus`
passes 153 source instructions / 4,896 DMA-contention cases, checking CPU,
cycles and RAM. The new group adds 51 unique instructions to earlier coverage:
the independent union is 14,650 instructions / 468,800 cases. This batch did
not rerun the combined oracle; the last fresh combined result remains
14,599 / 467,168. Shared runtime CPU/bus/math helpers and fixture setup did
not change. The generator now handles the original MOVEM predecrement store
when generating this family's bridge; existing bridges were not regenerated.

All 36,236 isolated live RGB444 frames and final RAM seals match fresh source
OFF streams. The full 459-entry gate passes 554,025 shadow / 413,303 sandbox
calls with zero mismatches, sealed RAM exact and poison frames identical.
GNU and MSVC Release builds pass. There are 251 source-timed entries.
Evidence hashes and artifact size are recorded in
`analysis/figures/native_context_publication_checkpoint.json`.

The isolated 600-frame probe is exact. ALL still first differs at frame 416
by 361 pixels; the user-deferred Copper/HUD issue remains unchanged. Native
OFF/ON comparisons use the same machine model and do not establish independent
Amiga timing parity. Continue complete postflight/menu callback owners next,
preserving enclosing-frame exits in C0FFE2/C1000A. The full objective remains
Stage D game C, then Stage F native backend and only necessary Stage E OS work.
