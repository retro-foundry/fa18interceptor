# `$C13E10` isolated joystick-state consumer stage

Classification: **behavioural input-state evidence**, restricted to this
complete observed interval. It is a point inside a larger routine, rather than
a claimed function entry or a reconstructed source slice.

## Runtime packet

- Restored state: `run001` frame 2,696 with only its rebased frame-2,697
  joystick press (`J 0 5 1`). No future input is present while stepping.
- Breakpoint `$C13E10` hit at replay frame 17. The active frame record gives
  the real return address `$C25D84`.
- The interval completes in 301 instructions at that return boundary.
- P-code: `pcode/raw/run001_c13e10_control_state_stage/`, with 301 observed
  starts, 1,854 P-code operations, and the two dynamically reached helper
  entries `$C14876` and `$C26428`.

## Verified caller context

The static callsite and active stack agree on the enclosing edge:

```text
$C25D7C  move.l A1,-(A7)
$C25D7E  jsr    $C13D84
  ... measured `$C13E10` stage ...
$C25D84  movea.l (A7)+,A1
```

Before the measured point, `$C13D84` selects a record at
`$C46184 + ($C459B4 << 9)` and stores that pointer in `$C18210`. Its initial
mode branch reads `$C4577C` at `$C13E0A`; the measured route then directly
reads `$C45778` at `$C13E10`. This identifies the control stage as an indexed
record update, without identifying what the record represents.

Its entry prologue through `$C13E0F` is byte-exact in
`source_amiga/observed/prepare_indexed_control_record_context.asm`. This is a
verified context-setup slice rather than a reconstruction of the larger
function's unobserved branches.

The first helper is fully bounded at `$C14876-$C148A1` and has a byte-exact
source reconstruction in
`source_amiga/observed/apply_parent_delta_to_shared_word.asm`. It takes a
caller-frame word, arithmetic-shifts the difference from word `$26` of the
record referenced by `$C18210`, then writes the adjusted word back. The
record and field ownership are still unknown.

The second child is the complete helper `$C2641E-$C2651B`, entered at
`$C26428` in this packet. Its byte-exact reconstruction is
`source_amiga/observed/update_indexed_shared_record_fields.asm`; it derives
and smooths two record words from indexed values and lookup tables, without a
subsystem name.

## Measured data flow

At entry, `$C13E10` directly reads `$C45778`; its value is `$0208`. The caller
has supplied the same `$0208` in `D0`, and the stage stores their zero
difference in its `-$28(A6)` local. The observed route later reads
`$C45778` again at `$C1402A`, shifts the local value by three, and writes the
result to `$C4577C` at `$C14044` (zero for this packet).

This establishes a later, bounded consumer relationship from the joystick
callback's second accumulator to control-stage state. The accumulator is still
not named as pitch, roll, or another flight axis: this one event does not
separate those possibilities. The broader static routine has many unobserved
branches, so no source reconstruction is claimed from this packet.

## run060 saturated-control check

- Restored state: sealed `run060`, breakpoint armed before replay frame 940.
  The instruction breakpoint hits at replay frame 942 and execution reaches
  the real caller return `$C25D84` after 260 instructions.  Future replay
  input is deliberately not delivered while that bounded interval is stepped.
- `$C13E10` reads `$C45778 = $03C0`, and the observed route writes that same
  word to `$C4577C` at `$C14114`.  The three sampled values (`$C45776`,
  `$C45778`, `$C4577C`) are respectively `$01BF`, `$03C0`, and `$03C0` both
  before and after the interval.  This is a held/saturated control-state
  observation, not proof of the physical control axis.
- The selected record remains `$C46184`.  Comparing the entry and return RAM
  images shows no change in its pose coordinates `+$14..+$1F`, angle tuple
  `+$66..+$6B`, or attitude matrix `+$92..+$A3`.  The only changed bytes in
  the 512-byte record are `+$39: $02->$01`, `+$6D: $78->$AE`,
  `+$6F: $78->$AE`, and `+$74: $23->$1D`.

Therefore this live control-stage invocation is upstream state preparation or
auxiliary record maintenance, not a direct publisher of the measured flight
pose, orientation-angle tuple, or attitude matrix.  That conclusion is
limited to this invocation; later stages in the enclosing update chain can
consume its changed fields.
