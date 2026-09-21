# `$C2F766` renderer word and dispatch tables

Classification: **runtime-backed dataflow table map**. `$C2F5F4` loads
`$C2F766/$C2F786`; `$C2F60A` loads `$C2F7C6/$C2F786`; and the bounded entry
loads `$C2F7C6/$C2F7E6`. Those loads are present in the recorded renderer
P-code packets.

| Table | Entry width | Values / target range |
|---|---:|---|
| `$C2F766` | word | `$8000,$4000,$2000,$1000,$0800,$0400,$0200,$0100,$0080,$0040,$0020,$0010,$0008,$0004,$0002,$0001` |
| `$C2F786` | long | `$C2F826`, then `$C2F83A-$C2F8C6` in `$0A`-byte steps |
| `$C2F7C6` | word | `$C000,$C000,$6000,$3000,$1800,$0C00,$0600,$0300,$0180,$00C0,$0060,$0030,$0018,$000C,$0006,$0003` |
| `$C2F7E6` | long | `$C2F8D0-$C2FA56` in `$1A`-byte steps |

The word tables are indexed from the low nibble of `D0`; the long tables are
indexed from the low nibble of `$C45954`. The pointer-table targets are
reconstructed in `apply_primary_renderer_lane_masks.asm` and
`apply_offset_renderer_lane_masks.asm`. This maps numerical table ownership and
control flow only, not the visual meaning of their masks or lanes.
