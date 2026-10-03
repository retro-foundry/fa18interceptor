# Complete menu follow-ups and table-file owner (2026-10-03)

C1029E/C10418/C10458/C10678/C1643A now have complete readable C, normal CPU
adapters and source timing. Three callbacks are original source-only entries;
C1643A adds a translated entry, and C10678 replaces its older adapter. The
registry has 470/624 translated entries plus 13 source-only callable entries,
483 total rows and 277 timed entries (264 translated plus 13 source-only).
The seeded translation count is not whole-game scope.

## Original behavior

`python tools/recomp/audit_menu_followup_source.py` seals 151 unique / zero
shared instruction boundaries: 12, 13, 23, 44 and 59 for the five owners.
The earlier `menu_followup_scope_inventory.json` is an inventory, rather than
implementation evidence. `menu_followup_source_scope.json` now drives the
complete timing generator and independent instruction coverage.

The viewport callbacks compare the source mode and target bytes before
publishing their distinct context/pause state, countdown and callback.
C10458 retains the key/message latch, sequence/key gate, actual C11312 reset
child and negative-countdown alternative. Stores after a child consume its
changed registers and state. Partial D0/D1 writes retain their high halves.

C10678 preserves LINK -6, the sign-extended mode word and advancing queue
pointer locals. Modes 3 through 8 optionally queue 5F or 60, followed by 47
and zero. Its sequence path publishes context state and C1078A. The older
typed `queue_mode_messages()` API delegates to the same readable domain.

C1643A preserves LINK -12 and every original DisownBlitter, check, Delay,
Open, Read, Close and OwnBlitter child site. The Open handle and Read result
are saved after clearing the next Delay argument, before calling Delay;
later child register changes cannot replace either saved value. Read uses
the current, child-changed MODE_TABLE pointer and requests exactly 78 bytes.
The source accepts every read result except -1, including zero and short
reads. Invalid signed handles clear the ready word and return zero. Error
paths keep the source's actual cleanup behavior; no retry or invented OS
return is introduced. Handwritten production glue contains no opcode handlers.

## CPU/RAM proof and file-path limits

`python tools/recomp/check_menu_followup.py` passes 122,880 complete calls:
five real-child owners at 16,384 cases and five controlled-child owners at
8,192 cases. All registers, PC, full SR and all Chip/Slow RAM, including
stack bytes, are compared without register or RAM exclusions.

Controlled children cover all 151 owner boundaries. Their entry snapshots
compare full CPU/RAM at every actual source child-call boundary. Changed
registers/CCR, saved handle/read-result paths, changed table pointers and
positive/zero/negative read results are exercised independently of OS execution.

Real children cover every boundary of the four callback/message owners.
Original C1643A with zero initial status stopped at FC0EC0, fixture zero,
before its C adapter was compared. The actual diagnostic is retained in
`build/recomp/menu_followup_os_source_stop.log`. Returning real-child fixtures
use nonzero initial status and cover its six-boundary status gate only;
the remaining 53 file-owner boundaries require the controlled-child layer.
This does not establish original OS/file-loading parity. Production still
calls the actual original children, and OS work remains deferred.

Independent recorded normal-C checks disable timing only in their temporary
registry and retain original liveness. C10678 matches 18 shadow and 18 sandbox
calls; C1643A matches one in each mode, with 74 source cycles identifying its
initial status gate. Hardware, incomplete and mismatched counts are zero.
The three source-only callbacks are cold in all recordings. Their raw zero
reports and `C1029E: no completed comparisons` rejection remain retained;
recorded nonregression does not supply their original-byte path coverage.

## Runtime and integration

`python tools/recomp/check_menu_followup_dispatch.py` passes 3,840 actual
ON/shadow/sandbox fixtures. ON must start a native continuation; reference
modes must complete an actual matched comparison. Full entry/exit CPU/RAM
matches without exclusions. Non-call, OFF and selection guards require zero
port calls, including translated entries. Same-value writes to each owner's
original entry invalidate future dispatch, leaving PC and SP unchanged.
These are entry guards, rather than a claim of every self-modification path.

Local DMA proof passes 151 instructions / 4,832 cases, comparing registers,
SR, PC, cycles and RAM. The full 483-row registry gate passes 554,025 shadow
and 413,303 sandbox matches, zero mismatches, exact RAM seals and identical
poison frames. All 36,236 isolated live frames and seals match source OFF.
The isolated 600-frame probe is exact; ALL remains frame 416 / 361 pixels.
Copper/HUD fade work remains user-deferred. OFF/ON share the machine model
and do not prove independent Amiga timing parity. GNU and MSVC Release pass.
The fresh combined DMA oracle passes 15,370 instructions / 491,840 cases.
Build artifacts occupy 0.654 GiB. Counts, scope and evidence hashes are in
`analysis/figures/native_menu_followup_checkpoint.json`.

## Next original owners

`python tools/recomp/audit_menu_outcome.py` seals C104C2/C105F4/C1072E/C1078A,
159 unique / zero shared boundaries. Three are source-only callbacks;
C1072E has an older registered adapter. C1078A's indirect jump is bounded
by the original four eight-byte table records and their branch instructions:
keys 9, 7D, 2 and 1. Each record's key, bytes and branch destination are checked
against the original. This inventory implements none of those owners.
Continue their installed continuations and the original callback/call graph.
Stage D game C -> Stage F plain native backend -> necessary Stage E remains
the objective; generated source and the emulator are still scaffolding.
