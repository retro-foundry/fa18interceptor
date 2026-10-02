# Complete control-record update and enclosing update stage

2026-10-02. C22C80's complete 226-instruction parent is readable in
`port/game/record_update_stage.c`; C1C63E's complete 112-instruction owner
is in `port/game/update_stage.c`. Both CPU adapters and all 338 instruction
boundaries remain in glue. The registered set is now **434/624**, with
**225 timing-step entries**. Internal labels are not additional entries.

## Source-owned record dispatch

C22C80 saves/restores the full D5 through the actual source RAM stack.
When the post-input event is clear, decrement all sixteen workspace words
at C48188 + slot*32, preserving word wrap. The periodic low-nibble gate
retains its original C28996 child. Clear bit zero in control-record header
words at +2 for slots **0..14**, leaving slot 15 untouched. Decrement the
two signed-positive byte counters only on the source event route, then call
the selection-release owner before preparing root state.

The root's optional countdown decrement, bit-one clear and four ordered
control/view/marker/pose children stay distinct. Other slots publish the
source slot/stride words and pair addresses before their active-bit tests,
ready/placement children and conditional dispatch/pose calls. The ready
and dispatch decisions consume the source child's **Z decision**, retaining
all its outputs; they do not infer readiness from a fabricated record model.
Standalone slots preserve A2 from the preceding stage. Slots 14/15 force
header bit two only when active. Slot **7 has preparation and a bit test,
then the source replaces A1 with slot 8 without an update call for slot 7**.
Finish through C09E06, then restore D5 and the original final flags.

## Enclosing update stage

C1C63E retains the view-octant/attitude request gate, signed threshold
crossing against A000, negated position bias and CLR.W/SWAP/ASR.L #5 coarse
key. The record-update child owns its complete effects. The selected-record
route calls C1C7F6 with the signed offset, publishes that child's resulting
record fields, and computes the reversed low-bit cell key.

The active-origin route saves D5.W through the real source stack around the
original **C29042** child; its changed upper half remains live. Then call
the root rate owner, reduce the original origin pair with the source word
shifts, and publish fine/coarse keys in order. Changed fine keys request all
only under the source mode/projection guard; changed coarse words request
all unconditionally. OR the resulting byte into the live request mask.
The domain modules contain no CPU, CCR, timing or instruction machinery.

## Independent proofs

`python tools/recomp/check_record_update_stage.py --cases 8192` matches
**8,192 complete original-byte calls per entry, 16,384 total**: all sixteen
registers/high words, PC, full SR and all Chip/Slow RAM. No stack/liveness
exclusions or patched source instruction bytes are used. Fixtures retain
real source-owned record types/pointers, cover each dormant slot and all
slots active, both group gates, all byte-counter values, word-wrap boundaries,
periodic gates, selected-record offsets, origin mode, request guards and
signed long threshold/extreme values. Real original children execute once.

The two-entry normal readable-C proof passes **305 shadow / 5,070 sandbox**
completed comparisons. Enclosing-parent absorption leaves the record child's
own batch sandbox comparisons incomplete. An additional **C22C80-only**
registry independently matches its body on every recording: **305 shadow /
5,070 sandbox**, with one hardware/3,782 incomplete shadow calls and 809
incomplete sandbox calls kept separate. C1C63E's own batch body matches
**296 shadow / 5,070 sandbox**, retaining one hardware/3,791 incomplete shadow
calls and 745 incomplete sandbox calls. Together the accepted independent
proof per entry has **601 shadow / 10,140 sandbox** completed comparisons;
none of the hardware/incomplete calls is accepted as a match.

The local DMA oracle matches **338 instructions / 10,816 cases**. The fresh
combined oracle matches **12,463 instructions / 398,816 cases**, including
registers, full SR, PC, cycles, RAM and bus timing, with the sealed machine
reset before each fixture. The full **434-entry** registered gate passes
**599,422 shadow / 817,839 sandbox**, zero mismatches, all three final RAM
seals and identical poison frames. Parent absorption changes aggregate totals.
GNU headless and MSVC Release builds pass. Both entries together also match
all **36,236 isolated live RGB444 frames and sealed final RAM** against fresh
source OFF: demo 20,833, carrier 12,353 and failure 3,050. Temporary frame
streams are removed; build output after cleanup is **0.329 GiB**.

The stepped proof runner now captures a source child's return PC/stack depth
**before** its local LINK/MOVEM frames or nested calls. Preserve that boundary
through generated dispatches and cold original-byte continuations in the
runtime. Reading (A7) only after a cold dispatch can mistake saved local data
for a return address. Interpreter-only/incomplete results retain their failure
classification; no comparison mask is relaxed and handwritten game glue
never invokes opcode handlers. The full gate, including poison, exercises
the corrected runner after the failed experiments retained under build/recomp.

The isolated 600-frame timing probe is exact. ALL remains at **416 / 361
pixels**, with Copper fade still deferred and automated comparisons intact.
See `../figures/native_record_update_stage_checkpoint.json` for the live
all-recording result, raw proof classifications and current build size.

## Remaining active-origin scope

C29042 is **not registered** by this batch. Its generated list has 153
instructions, but the cold mode-table targets add **229** original instructions
at C29226-C295B5. The complete static span has **382 unique instructions**,
with the shared final tail counted once. The nine original targets, decoded
instructions/opcodes and sealed-byte hashes are in
`../data/active_origin_complete_source.json`. `port_info.instructions` now
includes those paths and rejects a changed source-state hash, so future parent
planning and timing oracles cannot silently use the recorded slice alone.

Complete those thresholds, terminated record scan, local preset call, blend,
small matrix variants, mode-six countdown, scale and smoothing routes next.
Preserve all original local save frames and source child boundaries. After
that, complete C0EFD4's whole update sequence; C0F090/C0F132 are internal labels.
The older typed direct-origin helper is corrected separately: C2908A jumps
straight to C291C8, preserving Y and skipping the matrix route's floor clamp.
Its focused GNU -O2/-Wall/-Wextra contract passes; it is not an extra ported entry.
