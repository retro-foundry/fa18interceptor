# Postflight text pipeline: bounded evidence map

This map joins the byte-exact postflight display components without claiming a
single uninterrupted trace from a gameplay result to a particular message.
The parent update tail calls `$C322EE` in its postflight route; the run024
continuation independently enters and returns from the earlier `$C3180C` scan.
Run060 and run062 remain the separate outcome oracles.

```text
$C3180C scan (run024 entry/return)
  -> writes selected index/result/slot state

postflight parent tail
  -> $C322EE/$C322F8 signed and timer gates
  -> $C32318 slot/delay controller
     -> $C3238A record-code label selection
     -> $C32464 ALT:/HDG:/SPD: value preparation
     -> $C32510 table-message fallback
     -> $C325A6 buffer/style submission
        -> $C3278C or $C32794 glyph entries
        -> $C327A0 glyph loop
        -> $C32806 row compositor
```

## Proven components

| Address range | Proven local contract |
| --- | --- |
| `$C3180C-$C318F5` | scans 16-byte candidates and publishes selected index/result/slot state when requested. |
| `$C322EE-$C32317` | signed state, control-mask, and timer/reset gates. |
| `$C32318-$C32389` | slot word and two-tick delay control. |
| `$C3238A-$C3250F` | selects literal labels, copies them to postflight buffers, and prepares the associated numeric fields. |
| `$C32510-$C32678` | copies 27-byte table payloads or selected buffers and chooses submission parameters. |
| `$C3267A-$C328A5` | converts packed values, expands glyphs, and merges glyph bytes into strided renderer buffers. |

The source files cited by each range are byte-exact and pass
`python scripts/verify_reconstructions.py --no-coverage`.

## Explicit limits

- The candidate record class and its relationship to aircraft/mission entities
  are not proved by this pipeline.
- No observed branch here is a qualification success or failure predicate.
- The run060 successful-qualification text has not been traced through this
  pipeline; its native replay is a scenario oracle, not evidence that this
  exact postflight route emitted that screen.
- The run062 menu return is likewise an outcome oracle, not an assignment of
  a table payload or state byte to failure.
