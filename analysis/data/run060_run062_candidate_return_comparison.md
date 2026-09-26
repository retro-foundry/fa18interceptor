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

It hits at GUI frame 192.  At `$C279B8`, the live accumulated `D1` is `+119`
(`$00000077`).  The `BLT $C279C2` is not taken, `$C279BC` writes `D0=0`, and
the routine returns to `$C26014` after five instructions.  The retained trace
is `build/run060_full_c279b8_trace/trace.jsonl`.

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

Thus the sampled outcomes differ at a concrete geometry-filter result:
run060 has an observed nonnegative zero return, while run062 has an observed
negative `$10` return that prepares its later failure-side postflight route.
This does not prove that every zero return is success, that every negative
return is failure, or what physical quantity the candidate components encode.
