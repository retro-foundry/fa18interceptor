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

The top-level `$C22D4E` caller initially selects `A1=$C46184`. At the live
`$C26102` invocation, prior code has reached `$C261AA`: its comparison
`D0 == $0040` is false, hence it sets byte bit 7. The subsequent path requires
`D2 > $03C0`, `$C4589A == 0`, and bit 6 of `D0` clear before `$C26178` ORs
word bit 9. That same static path increments `+$46` of the structure pointed
to by `$C1AB74`, writes one to `$C457C5`, and writes `$04` to `$C45798`.

`$C45798=$04` is later the saved callback code that routes run062 through the
non-success-side `$C10DAE` continuation. This establishes an update-to-
postflight-callback dataflow chain. It does **not** establish that `$03C0` is
a landing threshold, that `D2` is an altitude/speed/position value, or that
this record belongs to the player.
