# Run062 `$C26102/$C26178` postflight record transition

Classification: **sealed-replay, instruction-level dataflow**. This proves the
two writes that prepare the failure-side `$C10DAE` route; it does not identify
the physical qualification predicate.

## Authority and method

Starting from the native `run062` GUI-frame-2,000 checkpoint, 124 ordinary
no-input frames were replayed. The following transition frame was then
instruction-stepped while comparing `$C46184` before and after every executed
instruction. This avoids the Engine9000 address-watch limitation documented in
the preceding checkpoint comparison.

The exact observed mutations are:

| Instruction | Live operands | Word transition |
| --- | --- | --- |
| `$C26102` `BSET.B #7,$0(A1)` | `A1=$C46184`, `D0=$0010`, `D2=$0A1B` | `$11C8 -> $91C8` |
| `$C26178` `ORI.W #$0200,$0(A1)` | `A1=$C46184`, same live `D0/D2` | `$91C8 -> $93C8` |

The frame sampler observes `$93C8` after replay frame 125. The later
`$C10DAE` handoff trace enters on replay frame 132, sees bit 9 set, and takes
the gate-positive route. That route clears bit 9 to `$91C8` before installing
`$C11788`.

## Static branch context

The entry is reached from the standard first-record update chain:

```text
$C0F01C -> $C1C6BC -> $C22D8E -> JSR $C25B66 -> ... -> $C26102
```

The top-level `$C22D4E` caller initially selects `A1=$C46184`. In this exact
execution, `$C2601C` reloads a zero low-word record index into `D1`; its
following `TST.W` therefore falls through the `$C260AC` path. `D0=$0010` has
bit 4 set, so `$C260BE` sets byte bit 7 at `+$03(A1)` and branches directly to
`$C26102`. (It does not use the distinct `$C261AA` route.) The subsequent path
requires `D2 > $03C0`, `$C4589A == 0`, and bit 6 of `D0` clear before
`$C26178` ORs word bit 9. The fall-through sequence then is:

```text
$C26184  MOVEA.L  $C1AB74,A0
$C2618A  ADDQ.W   #1,$46(A0)
$C2618E  MOVE.B   #1,$C457C5
$C26196  MOVE.B   #4,$C45798
```

Thus `$C26196` is the exact static producer of selector code `$04` on that
route. A 60,000-instruction step trace beginning at `$C25B66` from the
frame-2,000 checkpoint reports no *value transition* at `$C45798`: its
checkpoint value is already `$04`, so re-storing `$04` is invisible to the
tracer's before/after memory comparison. This does not establish that the
observed frame-125 invocation executes the store; it does establish the
instruction that produces the code whenever this fall-through is taken.

## Upstream return code

The live `D0=$0010` is not an arbitrary surviving register value. Immediately
before `$C26014` restores `A1=$C46184`, `$C279B8` tests its signed accumulated
`D1`; the trace has `$D1=-$15` and takes its `BLT $C279C2` branch.
`$C279C2` executes `MOVEQ #$10,D0; UNLK A6; RTS`, returning code `$10` to the
record-update continuation. The preceding helper's byte-exact checks show
that this is its negative accumulated-candidate return, but do not assign the
candidate components physical axes or a landing meaning.

### Exact first-component failure arithmetic

A replay from the same frame-2,000 checkpoint, with `$C27968` armed only
from replay frame 120, reaches this failure invocation at frame 125. The
retained 20-instruction trace is
`build/run062_frame2000_c27968_failure_candidate_trace/trace.jsonl`.

It establishes the exact first failed comparison:

```text
$10(A3)       = +11                  -> D2
$62(A3)&$F0   = $10                  (takes the three-component check)
$7B(A3)&$0F   = 0                    (passes its low-nibble gate)
$A6(A3)       = $FF03 = -253
$7D(A3)&$0F   = 3
ASR.W #3,-253 = -32
-32 + D2      = -21                  -> BLT $C279C2
```

The first `+$A6` component alone is sufficient in this invocation: the
second and third component checks are not executed. This promotes the failure
route to an exact signed fixed-point comparison, but neither the component's
physical axis nor its relationship to runway geometry is proven.

At that live `$C279C2` entry, `A3=$C46184`: the candidate is the same selected
base record later restored into `A1` at `$C26014` and modified by
`$C26102/$C26178`. The preceding check reads that record's `+$A6`, `+$AC`, and
`+$B2` words, shifts them by the low nibble at `+$7D`, and combines each with
the accumulated value prepared from its `+$10` long. This closes the
candidate-to-flagged-record identity for this run062 path. It does not prove
the base record is the player record or identify any component as an axis.

`$C45798=$04` is later the saved callback code that routes run062 through the
non-success-side `$C10DAE` continuation. The checkpoint proves the code is
already live by global frame 2,000; `$C26196` proves its route-local producer,
but retained delta traces do not date the producing write. This establishes a
negative-candidate return -> record flag/update -> postflight-callback dataflow
chain. It does **not** establish that `$03C0` is a landing threshold, that
`D2` is an altitude/speed/position value, or that this record belongs to the
player.

## Run060 negative evidence

The complete sealed run060 replay (all 10,085 recorded GUI frames from its
canonical restored state) was run with a breakpoint at `$C26178`. It does not
hit. The same instruction is directly executed in run062's transition frame.
Thus setting word bit 9 at this site is a strong observed discriminator between
the successful run060 scenario and run062's later failure-side progression;
it remains a scenario discriminator rather than a decoded physical rule.
