# `$C3B720 -> $C3B6B0`: five-triple static bridge component

Classification: **trace-proven compact immutable source block plus static face/control stream**.

The frame-12,000 bounded trace begins at `$C1F4AC` with `A1=$C3B720` and
transforms exactly five consecutive triples:

| Source triples | Destination workspace |
| --- | --- |
| `$C3B720-$C3B73D` (5 × 3 words) | `$C48390-$C483AD` (slots 0--4) |

After the fifth store, `A1=$C3B73E`; `$C1F578` immediately takes the
`$C1F6F8` route.  Unlike `$C35932`, there is no intervening mode word or
conditional coordinate/control packet in this path.  This exact 30-byte
source range is therefore safe to classify as immutable geometry input.

The renderer then loads `A5=$C3B6B0`; its static control stream dispatches
`$C2005C`, which resolves faces against the just-written `$C48390` slots.  The
source triple block and `$C3B6B0` control stream must remain separate assets:
the former is raw geometry input, while the latter encodes face/control
selection.  Neither `$C48390` nor `$C45BEA` is source data.

Authority: `build/run031_frame12000_c3b720_mixed_boundary_trace/trace.jsonl`,
instructions 0--260.

## Map-mode continuation

The independently video-hash-matched `M`-transition trace reproduces this
same source/control handoff in the map scenario itself. At trace index 90,909
(frame 9), `$C1F4AC` enters with `A1=$C3B720` and `A3=$C48390`; at index
91,104 the following `$C1F6F8` entry has `A1=$C3B73E`. Before another control
walker entry, the trace reaches four `$C2FF48` wrappers with static
`A5=$C3B6B0`; two take the wrapper's `$C2FA7E` line route and two take its
`$C304F4` direct span route. The same transition prepares the Copper-presented
map page with both renderer job families.

This provides a bounded map-mode chain:

```text
$C3B720-$C3B73D immutable five triples
  -> $C1F4AC transform -> $C48390 mutable slots
  -> $C3B73E / $C1F6F8 control entry
  -> $C3B6B0 static face/control stream
  -> line and span primitives -> prepared M-map bitplanes
```

It proves this one compact geometry/control component contributes to the map
renderer. It does not turn the five local triples into global world
coordinates, establish a named landmark, or make this partial component the
complete map.
