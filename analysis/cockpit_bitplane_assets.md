# Cockpit display bitplanes

## Evidence

`build/attract_focus_600/chip.bin` and `build/attract_focus_1800/chip.bin` are Chip-RAM snapshots from the deterministic attract playback. The latter shows the full cockpit. `scripts/analyze_cockpit_bitplanes.py` parses the active Copper list and extracts its four bitmap planes without treating mutable graphics as static program data.

At Copper-list address `$00A468`, the list loads these plane pointers:

| Plane | Chip-RAM range | Bytes |
| --- | --- | ---: |
| 1 | `$012BC0`–`$014AFF` | 8,000 |
| 2 | `$014B00`–`$016A3F` | 8,000 |
| 3 | `$016A40`–`$01897F` | 8,000 |
| 4 | `$018980`–`$01A8BF` | 8,000 |

The buffers are adjacent with a `$1F40` stride. Dividing that size by the captured 200 display rows yields 40 bytes per row, or 320 pixels per plane. The Copper switches `BPLCON0` from `$0200` to `$4200` at `$00A488`, enabling the four-plane section at vertical position `$2A`.

[Uncoloured planar-index inspection image](visuals/attract_cockpit_bitplanes_600_1800.png) shows the resulting bitplanes. Frame 1800 visibly contains the cockpit panel, HUD, gauges, and labels. Frame 600 instead contains an external aircraft view and small HUD readouts. The palette is deliberately represented as greyscale indices; its display colours must be recovered separately from Copper `COLORxx` state.

## Mutability result

All four buffers change between frames 600 and 1800. The row-level differences begin at row 37 and extend through row 199 in planes 1 and 3; plane 4 has several smaller unchanged intervals. This rules out treating these ranges as a single immutable cockpit asset. They are the active render target containing static-looking panel artwork plus frame-varying scenery, HUD, and instruments.

The machine-readable measurements, including SHA-256 hashes and changed rows, are in [cockpit_bitplane_map.json](cockpit_bitplane_map.json). The next constrained step is to use frame-to-frame diffs within this same buffer set to separate persistent panel pixels from display updates, then trace each writing primitive through its blitter setup.
