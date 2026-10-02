# Complete follow-up placement traversal and position children

2026-10-02. `port/game/followup_placements.c` implements the entire
$C1CCBC-$C1D0A2 parent, plus the complete $C1D0A4 workspace-position entry
and its shared $C1D0B6 control-position body. The parent has 269 original
instructions. Workspace/control position entries have 31 unique instructions
together; their shared $C1D0C6 tail is not another registered function.
The existing $C25876 list-point child is also newly source-timed.

## Complete source contract

The parent has three consecutive phases; all are implemented:

1. Reset the follow-up flags and cursor, then traverse positive word selectors
   at $C4E98A. A selector chooses a 512-byte control record. Word cell deltas
   and 22-bit origin terms form packed X/Z components. Prepared offsets and
   the depth component determine the signed-word maximum, logical half and
   original $EF table cap, including the returning $29 fault hook. The signed
   byte table at $C1DF46 supplies the shift. Publish packed coordinates,
   position-valid/biased-level state and their shifted words, compute distance,
   then dispatch the 20-byte descriptor selected through $C22188. Advance the
   shared selector offset by two until the source's nonpositive terminator.
2. Seed the alternate cursor from $C459B0 and walk 24-byte placements at
   $C4F6CA until exactly $FFFF. Preserve negative-descriptor and height gates,
   the extra record long, unsigned header kind, plain/control/workspace
   coordinate routes, cached-distance refresh, countdown/visit bytes, selected
   magnitude publication and descriptor-result storage. After the callback,
   reread the shared offset before storing a positive word result or -1.
3. Clear the source's tail flags, apply its mode/clock gate and visit all
   sixteen 512-byte control slots. Flagged records other than the selected
   slot supply wrapped relative long coordinates. Find the signed maximum of
   their absolute terms, shift down by powers of four, and append a tagged
   point through the existing readable list-point child. Preserve its two
   matrix rows, 16-byte stride, untouched padding word and trailing zeros.

Descriptor consumers remain required independent child owners. They are not
counted as newly ported renderer parents. The domain contains no CPU state,
CCR, instruction handlers or cycle charges. Semantic observations supply
computed values to the normal CPU adapter, which publishes caller-live widths
and high words without running a memory-writing child again.

## Source distinctions retained

- Workspace bit 6 takes precedence over control bit 4. The workspace helper
  uses signed `ASR.W #3` on the masked high byte; it can select below $C48184.
  The control helper doubles the masked high byte at word width and adds the
  signed result to $C46184. The unsigned workspace selection of other source
  families is not substituted for this distinct helper.
- Transformed placements bypass the negative-result/countdown gate but still
  participate in distance refresh. This differs from $C1CB14/$C1CB26's
  descriptor-type bypass, so the two parent loops remain separate contracts.
- Decrementing countdown -128 stores +127 but takes the negative branch
  through N/V. Kind is unsigned here; the earlier primary/alternate parent
  sign-extends its header high byte instead.
- The selected-distance override affects the live dispatch word and $C45B42,
  while $C45B40 retains the actual cached distance. Preserve this partial
  publication rather than replacing both globals with the override.
- Depth `ADD.L` followed by `BGE` tests the mathematical signed sum through
  overflow. Relative `MOVE.L`/`BGE` tests the stored sign instead. Negating
  $80000000 preserves that value; maximum selection remains signed.
- Long position additions and list-matrix sums now wrap explicitly through
  unsigned arithmetic before signed shifts. This removes C signed-overflow
  ambiguity while retaining the original long results.

## Independent readable C proof

`python tools/recomp/check_followup_placements.py` passes 8,192 complete
parent cases against independent original execution. Every register/high word,
PC, SR control and every Chip/Slow RAM byte match outside a 160-byte private
child stack allowance. Caller flags are dead; production liveness is not
relaxed. The allowance covers the original empty renderer's 152-byte locals,
saved A6 and child return words.

Fixtures cover empty/positive selected lists, zero/negative selected
terminators, alternate offsets and zero-to-five records, every header high
byte/shift, both position-selection families, invalid descriptors, height
gates, distance thresholds, refresh phases, countdown edge values, selected
overrides, mode/clock tail gates, relative points and extreme depth/long sums.
Original descriptor children use terminated control streams or the original
line-style reset. No instruction bytes are replaced. These bounded consumers
do not constitute complete renderer-parent proofs; real recorded consumers
are covered by the independent whole-call check below.

`python tools/recomp/check_followup_placements.py --positions --cases 16384`
separately proves 8,192 complete calls per position entry, including all
incoming CCR combinations, randomized full register values, all header high
bytes and shifts and wrapped coordinate/bias sums. Every register, PC,
**full SR and all RAM** match, with no stack or liveness exclusions. This is
the independent coverage for the workspace entry, which is cold in sealed
recordings; its live replay is a nonregression check, not an exercised proof.

`python tools/recomp/check_whole_call_glue.py C1CCBC` disables only the
parent's timing bridge in an isolated registry. The original return mask at
$C0F116 remains FFFF/FF/00: all sixteen registers and all eight data-register
high words are live, CCR is dead. Child execution uses the shared whole-call
CPU/RAM bridge, with the same held event deadline as the reference. Runtime
child dispatch between resumable steps supplies live timing separately.

| Native recording | Shadow matches | Shadow incomplete | Sandbox matches | Sandbox incomplete |
| --- | ---: | ---: | ---: | ---: |
| demo01 | 239 | 1,841 | 1,606 | 493 |
| qual_carrier_success | 49 | 1,598 | 3,174 | 206 |
| qual_fail_crashes | 0 | 351 | 351 | 0 |
| Total | 288 | 3,790 | 5,131 | 699 |

All completed comparisons match, with zero mismatches/hardware classifications.
Incomplete calls are not counted as passes, including every shadow call in
the failure recording. Sandbox replay can encounter different call totals;
it is a CPU/RAM proof rather than a live timing oracle.

## Timing and integration

The four-entry local oracle compares 333 unique instructions / 10,656 DMA
cases, including full SR, PC, cycles and RAM. The original low-bit register
`BSET` takes two fewer cycles than its base fee; the timing bridge retains
that adjustment. The 600-frame isolated group probe is exact. ALL still
first differs at frame 416 by 361 pixels; Copper fade work remains deferred.
The combined oracle now restores the sealed machine snapshot before every
case and before reading each source opcode. A synthetic indexed write into
code-addressed RAM cannot contaminate a subsequent instruction fixture.

GNU and MSVC Release builds pass. Full registered/live results, cold and
incomplete classifications, recording seals, combined DMA proof and build
size are retained in `analysis/figures/native_followup_placements_checkpoint.json`
and `CURRENT_PORT_HANDOFF.md`.

The full 427-entry gate matches 633,759 shadow and 932,215 sandbox calls,
with zero mismatches, sealed RAM exact and poison frames identical. The four
isolated entries match all 36,236 live frames and all three sealed final RAM
hashes. Workspace entry $C1D0A4 remains cold in those runs and retains its
independent structural proof. Aggregate calls fall as the parent absorbs
children; neither reduced counts nor incomplete calls are new passes.
The fresh combined oracle passes 11,735 instructions / 375,520 DMA cases
after isolating every fixture. Build storage is 0.294 GiB after replay cleanup.

Review of the next $C0F5F8 parent also corrected its older typed offset limit
from decimal 4650 to source hexadecimal $4650 (18,000). The contract test
passes 5,000/17,999 as valid and 18,000 as invalid. That entry remains
unregistered; this preparatory correction is not another completed native
parent or a change to the deferred Copper-fade work.
