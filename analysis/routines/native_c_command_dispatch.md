# Complete command dispatch owners (2026-10-03)

C1AC28 pending-command dispatch and C1AD74 keyboard dispatch now have complete
readable C owners, independent CPU adapters and resumable source timing.
This adds two registered entries: **444/624, 235 timed entries**. Selection
prefixes and internal action labels are components, not additional functions.

## Source ownership and behavior

`analysis/data/command_dispatch_source_scope.json` follows original branches
and tail jumps, stops at returns and treats JSR/BSR targets as child owners.
It records the sealed demo state hash, every owned PC and instruction byte,
and a digest of the ordered PC/source-byte pairs. Reproduce the audit with
`python tools/recomp/audit_command_dispatch.py`.

C1AC28 owns 562 instructions, C1AD74 owns 1,024; 482 are shared, for 1,104
unique boundaries. The static audit finds no additional omitted cold paths.
The keyboard owner's reset exit includes the noncontiguous C06BF0 tail and
its three actual children. The invalid-command exit includes C1AC18's error
$33 and fault child. Source gaps skipped by C1BEE4's unconditional branch
are not silently added to this owner; C1BEE8 remains a distinct peer.

The domain separates command selection, flight actions, view/origin/zoom,
indexed function-key actions, context transitions and queue publication.
`port/game/command_dispatch.c` composes the complete owners. All source CPU,
flags, stack and child-return adaptation lives under `port/game/glue/`.
No handwritten production adapter invokes opcode handlers.

The implementations retain ordered key tests, signed event counters and
release bits, pending-bit consumption, partial-width results, inherited
selection words and child-clobbered results. Countermeasure decrement tests
preserve signed overflow at $80. Indexed selection writes C45849 and reads
the original mode/level/pose tables. Context commands preserve local/world
coordinate children, map deltas and recorder writes. Reset and fault exits
retain their exact child entry/return addresses.

The shared publisher claims an event even when the queue is full, keeps
signed negative queue indices, and preserves raw store, translation lookup,
index update, count reread and translated store order. Negative indices can
alias queue globals; caching those inputs would change behavior. Existing
`queue_view_key` and `start_view_mode_zero` now use this proven shared body.

## Independent readable-C proof

`python tools/recomp/check_command_dispatch.py` runs two distinct layers:

- Real original children: 8,192 complete calls per owner, 16,384 total.
  C1AC28 observes 429 parent boundaries and C1AD74 observes 759; the union
  is 947. Chipset work is held for this CPU/RAM structural comparison.
- Controlled child contracts: another 8,192 complete calls per owner,
  16,384 total. The original parent instructions remain unchanged. Before
  each child, every CPU register, full SR, all Chip/Slow RAM and exact
  entry/return addresses are compared. Contracts then change registers and
  selection/recorder state. The observed union is 948 boundaries; these
  cases do not claim to execute the real child bodies.

Both layers compare all registers, PC, full SR and every Chip/Slow RAM byte
without exclusions or original-code patches. The fixtures exercise all raw
key bytes, CCR combinations, queue edges and inherited selection values.
The complete-owner tool requires the reset, invalid, empty/wait and final
publication exits to be visited at its default case count.

Separate original-instruction component proofs cover action paths left cold
by complete-owner fixtures and recordings:

| Component | Command | Cases | Observed boundaries |
| --- | --- | ---: | ---: |
| Selection prefixes | `python tools/recomp/check_command_selection.py` | 16,384 | 317 |
| Shared publication | Same command | 8,192 | 28 |
| 28 aircraft actions | `python tools/recomp/check_flight_commands.py` | 14,336 | 250 |
| 16 view/origin/zoom actions | `python tools/recomp/check_view_commands.py` | 32,768 | 247 |
| Three indexed actions | `python tools/recomp/check_indexed_commands.py` | 98,304 | 212 |
| Five context actions | `python tools/recomp/check_context_commands.py` | 20,480 | 162 |

Component proofs total 190,464 cases. Together with complete-owner real-child
proofs, their visited-PC union covers **every one of the 1,104 owned source
boundaries**. The child-contract layer alone does not cover every boundary.
All structural layers total 223,232 cases; prefixes do not count as complete
calls or extra registered routines.

`python tools/recomp/check_whole_call_glue.py C1AC28 C1AD74` disables timing
steps and preserves normal liveness and hardware/incomplete classifications.
It independently matches **798 shadow / 891 sandbox** readable-C comparisons
with zero mismatches:

| Recording | Pending shadow / sandbox | Keyboard shadow / sandbox |
| --- | ---: | ---: |
| demo01 | 162 / 167 | 1 / 2 |
| qual_carrier_success | 0 / 0 | 507 / 554 |
| qual_fail_crashes | 0 / 0 | 128 / 168 |

Both owners have completed replay comparisons. Their action branches retain
separate structural evidence; not every branch is observed gameplay.

## Source timing and acceptance

The timing bridge implements original instruction boundaries without opcode
handlers. `make_command_dispatch_step.py` reproduces its PC/operation mapping
from the audited manifest; it does not generate the readable domain owners.
Each owner also supplies an exact owned-PC predicate to the runtime. This
allows noncontiguous shared tails while leaving actual children in the gaps
under their own ownership. Existing entries without a predicate retain the
contiguous-range contract. Resume and nested-return selection use the same
predicate.

`python tools/recomp/check_active_planes_step.py --group command_dispatch --bus`
passes **1,104 instructions / 35,328 fixtures**. The fresh combined `--group
all --bus` oracle passes **14,240 / 455,680**. Counts are unique, so overlaps
with existing view timing are not added twice. Each fixture resets the sealed
state and compares registers, full SR, PC, cycles and all RAM with DMA bus
contention enabled. This is not a raw bus-access-log comparison.

GNU and MSVC Release builds pass. The full 444-entry registered gate matches
**555,784 shadow / 413,307 sandbox** completed calls across all three sealed
native recordings, with zero mismatches, exact RAM seals and identical poison
frames. The isolated command pair matches all **36,236 live RGB444 frames**
and final RAM seals against fresh original-code OFF runs.

The 600-frame probe is exact for this pair. ALL retains its accepted deferred
Copper-fade difference at **frame 416, 361 pixels**; no gate or comparison was
weakened. Temporary frame streams are removed; `build/` is **0.472 GiB** after
acceptance. Immutable counts, report rows, source/log hashes and RAM seals are
in `analysis/figures/native_command_dispatch_checkpoint.json`.

Next audit remaining complete command/context publisher peers and the
postflight-mode scheduler family. Preserve child ownership, prove cold peers
explicitly, and do not count action labels or redo these completed parents.
