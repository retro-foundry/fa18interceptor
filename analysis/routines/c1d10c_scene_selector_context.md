# `$C1D10C`: scene-selector context setup

Classification: **scenario-backed selector-input setup**. This routine prepares the live inputs that the `$C1D330-$C1D3E6` band walk later combines with the immutable segment-65 control stream. The observations establish two context packs; they do not identify either pack as a camera position, world coordinates, terrain cells, or LOD state.

## Authority

- Sealed `run033` replay restored before replay frame 404 and traced for the next three chipset frames: `build/run033_placement_bulk_404_trace/trace.jsonl`.
- The downstream byte-control-to-static-group records are retained in the [workspace-band selector stream](../data/workspace_band_selector_stream.md).
- The related static `C1C63E` slice is independently observed in `pcode/raw/attract_cockpit_1800_tenframe_trace/observed.asm.txt`.

## Proven context packs

`$C1D10C` creates an `A6` frame and selects one of two observed setup paths. Both populate the common selector pointers:

| local | value written | downstream role observed |
| --- | --- | --- |
| `-4(A6)` | `$C42390` | static group-offset base |
| `-8(A6)` | `$C19A9C` | static bit-gate table base |
| `-14(A6)` | `$C4F03A` | runtime placement-cache base |
| `-24(A6)` | `$C459AE` | helper context pointer |
| `-18(A6)` | `$C1D78E` | helper table pointer |
| `-1C(A6)` | `$C1D7E2` | helper table pointer |

On the frame-1 route, `$C1D21C`, `$C1D224`, and `$C1D22C` copy `$C45948`, `$C4594A`, and `$C45850` respectively into `-26(A6)`, `-28(A6)`, and `-2A(A6)`. On the frame-3 route, the nonzero test at `$C1D116` branches to `$C1D186`; `$C1D16A`, `$C1D172`, and `$C1D17A` instead copy `$C4594C`, `$C4594E`, and `$C45851` into those same locals. The subsequent shared path begins at `$C1D234`.

At `$C1D376` the shared band walk loads `-26(A6)` into `D1`; at `$C1D388` it loads `-28(A6)` into `D0`. The selector path then applies its static control-byte transforms and calls `$C1D3F4` with a static group index plus the live row term. This accounts for the two different ranges in the trace: the frame-1 calls have row terms `$000F-$0012`, while frame-3 calls have `$0040-$0043`.

## Upstream producers

The snapshot-valid static writer at `$C1C8B0-$C1C8F8` produces the first
pack before the calls to `$C1D10C` at `$C1C920` and `$C1C946`. If `$C45785`
is clear, it resolves `A1=$C46184 + word($C458DE)`, reads words at `A1+$06`
and `A1+$08`, arithmetic-shifts each right by two, and stores the results to
`$C45948` and `$C4594A` at `$C1C8F2/$C1C8F8`. If `$C45785` is set, it
instead derives the two words from the high words of `$C45C3E` and `$C45C46`
with an arithmetic right shift by eight before joining the same stores.

Thus, the first pack is not an untraced mystery source: it is either a
coarsened view of two current-control-record words, or an alternate dynamic
source from the `$C45C3E/$C45C46` pair. The exact mode and source values for
the captured frame-1 call still require a bounded execution trace; this
static writer contract alone must not be promoted to a coordinate claim.

For the second pack only, the observed `$C1C63E` path gives an immediate runtime producer. Provided `$C45785` is clear, it establishes `A3=$C46184 + word($C458DE)`, calls `$C1C7F6`, then writes:

```
$C1C6D4: word(A3 + $06) -> $C4594C
$C1C6DC: word(A3 + $08) -> $C4594E
$C1C6E4: byte(A3 + $0A) -> $C45851
```

`$C1C6EC-$C1C70C` also derives `$C45850` from the low two bits of the two words. The alternative observed path at `$C1C716` derives comparable values from `$C45C3E/$C45C46`, then compares them against `$C4594C/$C4594E` at `$C1C7CE/$C1C7DE` before setting a request bit in `$C45858`.

This is evidence that both selector packs are fed by mutable runtime state and a record family rooted at `$C46184`, not directly by the byte-stable segment-65 control stream. Other independently reconstructed consumers call `$C458DE` the current control-record byte offset: `$C2DAF2` adds it to `$C46184` before reading that record's angle at `+$68`, and the bounded raw `R` command uses the selected record's radar-range field. That makes a terrain-source reading less likely, but does not prove what the selector words mean spatially, where `$C458DE` originates, or that the control-record family is authoritative terrain data.

## Consequence for map and LOD claims

The selector has a measurable two-input shape: immutable control bytes choose candidate static groups, while mutable terms choose rows within them. That shape is compatible with several designs (spatial paging, scene mode, or a detail policy), so it is deliberately not named a grid or LOD system. The control-record evidence also means that the next terrain lead should not assume this selector is the world-map index: it needs a producer trace for the first pack and a controlled position experiment that changes selected static sources.
