# `$C38B0A`: external-aircraft-frame projected-edge list candidate

Classification: **runtime-backed projected-edge candidate**. Its visual owner
is not assigned below the external-view scene level.

## Evidence

`build/run031_frame7500_c1f6f8_probe/` restores the sealed run031 external
aircraft checkpoint, performs no future input, enters `$C1F6F8`, and reaches
the reconstructed `$C212B0` offset-pair submitter. At that entry:

```text
A5 = $C384CE   active walker stream
A2 = $C38B0A   offset-pair list
D0 = $00000034
```

The dispatcher first calls `$C21C2E`, then dispatches `$C212B0`; the latter
calls `$C2EE4A` and `$C2FA7E` repeatedly. This establishes a live
walker-to-edge-list-to-projection path in the external-aircraft frame.

## Decoded list

The first word is the `$C212B0` selector (`$0003`). It is followed by endpoint
offset pairs; a negative second offset marks the final pair and retains its
low 15 bits as the usable endpoint offset.

| Pair | First offset | Second offset |
| ---: | ---: | ---: |
| 0 | `$000C` | `$0012` |
| 1 | `$0006` | `$0018` |
| 2 | `$0018` | `$0024` |
| 3 | `$000C` | `$001E` |
| 4 | `$0012` | `$801E` (final; `$001E`) |

The selected source triples are resolved relative to `$C48390` by `$C212B0`.
The record is a candidate for geometry visible in the external-aircraft frame,
but no individual pair is named as an aircraft edge until a matched screen and
pixel/line correlation is obtained.

The first four `$C2FA7E` entries in the same trace receive short screen-space
segments around `(196,44)`: `(196,44)->(198,44)`,
`(197,44)->(197,43)`, `(197,43)->(197,44)`, and
`(196,44)->(197,44)`. These coordinates are an implementation-level endpoint
record, not yet a pixel correlation or named aircraft feature.
