# Run031 Golden Gate-frame renderer pipeline

Classification: **runtime-backed end-to-end scene-rendering dataflow**.

This map joins the no-input frame-12,000 Golden Gate traces. It covers the
observed control records through the renderer's blitter-line path.

```text
$C355A6 active control stream
  └─ $C35720 dispatch packet
       ├─ three/four-point record formats
       │    ($C21490, $C2159E, $C21500, $C2168A, $C2139E, $C21412)
       ├─ counted/variable segment formats
       │    ($C211DC, $C2122A, $C212B0, $C2131C)
       └─ workspace triples ($C4BF90 / $C4C592)
              ↓
          $C2469E outer clipping
              ↓ $C247C0 paired-boundary clipping
          tuple cache ($C248B2 / $C24996)
              ↓
          final polygon tuples ($C4B990)
              ↓ $C24CFE perspective projection
          screen-pair list ($C4B390)
              ↓
          $C2FF48 → $C301F6
              ↓
          $C2FA7E blitter-line emitter
```

## Proven links

- `$C35720` dispatches ten observed record formats in this frame; all ten
  handlers are reconstructed byte-exactly. See
  `analysis/data/c35720_golden_gate_control_dispatch_sequence.md`.
- The three- and four-point formats converge at `$C2469E`; its outer loop,
  paired continuation, tuple-cache handoffs, and four post-loop closures are
  reconstructed byte-exactly.
- `$C24CFE` projects finalized triples with the 160/90 perspective scales,
  clamps the 320-by-180 viewport, reflects its stored coordinates, and calls
  `$C2FF48`.
- A no-input renderer probe captured four triples becoming exact `$C4B390`
  pairs at `$C2FF48`; `$C301F6` then consumes the same list. See
  `analysis/data/run031_frame12000_polygon_projection_sample.md`.

## Remaining boundary

The chain proves how scene data becomes renderer input. It does not yet prove
which `$C35720` entry or `$C48390` record owns a specific visible Golden Gate
tower, cable, deck segment, aircraft, or other landmark. That requires a
controlled record mutation or an equivalent output-to-pixel experiment using
an ordinary full-replay screenshot oracle.
