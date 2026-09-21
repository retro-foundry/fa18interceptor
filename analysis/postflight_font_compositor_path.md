# Postflight font-compositor path

Run024's frame-23000 continuation establishes this renderer chain. Each arrow
below is either a runtime-observed call/join in that trace or a byte-exact
static continuation explicitly marked where unobserved.

```text
$C33F70 / $C33F8A transform helpers
  -> A1 = $C33264 / $C33258 offset-mask pair table
  -> $C32AA4 loads lane and long offsets
  -> $C32AD0 formats $C45B22 into backward ASCII bytes at A0
  -> $C32B00 reads A1 pair + A2 character byte
  -> $C3D790 glyph-offset table selects D4 glyph stream
  -> $C32B40 / $C32B72 (observed) select compositor pointer slots
  -> $C330FE merges glyph bytes into strided display longwords
  -> $C32B90 tail (static-only) repeats slots 8(A5), 12(A5), then DBRA
```

The `$C33258` entries are operationally a destination-offset/mask pair:
`$C32B00` loads the first word into `D5` for a renderer-pointer-relative
destination and the second into `D3` for the update mask. They are not proven
to be x/y coordinates. The character byte comes from `A2`, is normalized by
subtracting `$20`, and selects a glyph through `$C3D790`.

This is a compositor path, not yet a cockpit-field identification. `$C45B22`
is a reusable packed-value workspace, and the current evidence does not prove
which postflight display label or gameplay state produced a given invocation.
For the separately observed cockpit-numeric bridge, see
`analysis/routines/c32740_packed_nibble_font_renderer.md` and
`analysis/cockpit_numeric_oracles.md`.
