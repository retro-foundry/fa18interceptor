# `$C37EA0`: bridge-silhouette-frame projected-edge list candidate

Classification: **runtime-backed projected-edge candidate**. The bridge's
identity is not yet user-confirmed for this frame.

## Evidence

The six list bytes match original Hunk `CODE` segment 49 at payload offset
`$510`. Its resulting runtime payload base is `$C37990`. Comparing the entire
1,512-byte payload with `captures/baseline_menu/slow.bin` gives zero
non-relocation mismatches; all 20 relocation sites consistently resolve their
target segment 46 to `$C37218`. This is therefore a verified runtime placement
of the enclosing CODE Hunk, while this six-byte range remains separately
classified as inline data.

The sealed run031 frame-14,500 checkpoint has a cockpit view with a visible
bridge silhouette. In its following no-input invocation,
`build/run031_frame14500_c1f6f8_probe/` reaches:

```text
$C1F6F8 walker, A5 = $C37E62
  -> $C1F942 indirect dispatcher
  -> $C212B0, A2 = $C37EA0
  -> $C2EE4A projection preparation
  -> $C2FA7E line setup
```

The `$C212B0` invocation returns normally to `$C1F944` after one submitted
pair. Subsequent execution continues through other scene-table callbacks, so
this packet is not evidence that the complete bridge is one primitive.

## Decoded list

`$C37EA0` begins with selector `$000C`. Its sole endpoint-offset pair is:

| First offset | Second offset |
| ---: | ---: |
| `$0000` | `$800C` (final; `$000C`) |

`$C212B0` resolves those offsets through `$C48390` and copies the resulting
triples to `$C4C592` before projection. The endpoint pair is a candidate for
geometry visible in the bridge-silhouette frame, not a named bridge component.
Its `$C2FA7E` setup receives the screen-space segment `(280,67)->(282,67)`.
That short, distant segment is an endpoint record for later screenshot/pixel
correlation; it does not identify a bridge member on its own.
