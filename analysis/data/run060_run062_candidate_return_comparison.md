# Run060/run062 candidate-return comparison

Classification: **sealed-replay scenario comparison**.  This is evidence about
the two recorded qualification outcomes, not a complete definition of the
candidate scan or of the physical landing rule.

## Success-side direct return

The following sealed run060 probe arms `$C279B8` from the first replay frame:

```powershell
python scripts/trace_from_breakpoint.py `
  --restore captures/run060/restored-state.bin `
  --playback captures/run060/playback.e9k `
  --address 0xC279B8 --arm-frame 1 --return-pc 0xC26014 `
  --frames 10085 --max-instructions 16 --ignore-future-input `
  --output build/run060_full_c279b8_trace
```

It hits at GUI frame 192.  At `$C27968`, `+$10(A3)` supplies `D1=+119`
(`$00000077`), but `+$7B(A3)` supplies `$FF`.  Its signed test at `$C27972`
takes `BLT $C279B8` directly, before the type or three-component checks.  At
`$C279B8`, the accumulated `D1` is therefore still `+119`; its own
`BLT $C279C2` is not taken, `$C279BC` writes `D0=0`, and the routine returns
to `$C26014` after five instructions.  The retained trace is
`build/run060_full_c279b8_trace/trace.jsonl`.

The same direct-bypass values recur in a late success-side sample: arming at
run060 frame 9,000 reaches `$C27968` at frame 9,005 with `+$10=+119` and
`+$7B=$FF`, then returns zero through the same path.  The retained fixture is
`build/run060_late_c27968_probe/trace.jsonl`.  This makes the observed
zero-return route relevant to the landing-success interval, but it does not
make it a component-bound acceptance.

The complementary direct probe keeps a breakpoint at `$C279C2` for every one
of run060's 10,085 recorded GUI frames:

```powershell
python scripts/trace_from_breakpoint.py `
  --restore captures/run060/restored-state.bin `
  --playback captures/run060/playback.e9k `
  --address 0xC279C2 --arm-frame 1 --return-pc 0xC26014 `
  --frames 10085 --max-instructions 16 `
  --output build/run060_full_c279c2_probe
```

It terminates with `Breakpoint c279c2 not reached`.  This is direct
no-hit evidence for the negative-return address over the full canonical
successful recording, rather than an absence inferred from a sampled profile.

## Failure-side comparison

Run062 reaches `$C279C2` with `D1=-$15`, returns `$10`, and that return flows
into the documented `$C26102/$C26178` record-flag transition.  See
[the instruction-level run062 transition](run062_c26102_postflight_record_transition.md).

Thus the sampled outcomes differ before and within this candidate-result
logic: run060 samples take the signed `+$7B` direct-bypass zero route, whereas
run062 enters the class-$10 component path and receives a negative `$10`
return that prepares its later failure-side postflight route.  The full-run
no-hit at `$C279C2` only proves that run060 never reaches this negative-return
address; it does not prove that a zero return means component-bound success.
Neither route identifies the physical quantity represented by the components.
