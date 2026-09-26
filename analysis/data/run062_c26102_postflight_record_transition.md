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
`$C26178` ORs word bit 9. That same static path increments `+$46` of the
structure pointed to by `$C1AB74`, writes one to `$C457C5`, and writes `$04`
to `$C45798`.

## Upstream return code

The live `D0=$0010` is not an arbitrary surviving register value. Immediately
before `$C26014` restores `A1=$C46184`, `$C279B8` tests its signed accumulated
`D1`; the trace has `$D1=-$15` and takes its `BLT $C279C2` branch.
`$C279C2` executes `MOVEQ #$10,D0; UNLK A6; RTS`, returning code `$10` to the
record-update continuation. The preceding helper's byte-exact checks show
that this is its negative accumulated-candidate return, but do not assign the
candidate components physical axes or a landing meaning.

`$C45798=$04` is later the saved callback code that routes run062 through the
non-success-side `$C10DAE` continuation. This establishes a negative-candidate
return -> record flag/update -> postflight-callback dataflow chain. It does
**not** establish that `$03C0` is a landing threshold, that `D2` is an
altitude/speed/position value, or that this record belongs to the player.

## Run060 negative evidence

The complete sealed run060 replay (all 10,085 recorded GUI frames from its
canonical restored state) was run with a breakpoint at `$C26178`. It does not
hit. The same instruction is directly executed in run062's transition frame.
Thus setting word bit 9 at this site is a strong observed discriminator between
the successful run060 scenario and run062's later failure-side progression;
it remains a scenario discriminator rather than a decoded physical rule.
