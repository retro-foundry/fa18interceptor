# `$C29042-$C295D0`: active-origin update

Classification: **scenario-backed live-origin producer**. This bounded path
computes and writes the three-component origin later consumed by the
terrain-template selector. It does not expose an immutable world-coordinate
or map-page table.

## Authority

- `pcode/raw/no_key_c1c63e/observed.asm.txt` executes the `$C29042` path and
  the `$C291C8` origin store during the normal update stage.
- `build/run033_origin_control_trace/trace.jsonl` and the paired mutation
  experiment establish that the X/Z components at `$C45C3E/$C45C46` causally
  change workspace template selection. See the
  [origin-selector mutation probe](../data/origin_selector_mutation_probe.md).
- The direct producer prefix `$C29042-$C291D3` is byte-exactly reconstructed
  as [`publish_terrain_selector_origin.asm`](../../source_amiga/observed/publish_terrain_selector_origin.asm).
  Its later `$C291D4-$C295D0` adjustment/dispatch continuation remains outside
  this bounded source slice.

## Observed producer path

The entry calls `$C2DAF2`, then applies state gates before selecting the active
control record as `A0 = $C46184 + word($C458DE)`. It chooses one of the
matrix/record transform helpers (`$C091A8` or `$C091CE`) based on observed
record type and mode fields. Their output triple is first retained at
`$C45C56-$C45C61` and loaded into `D5-D7`.

At `$C291B6-$C291C6`, the path derives a lower bound from the selected
record's word `+$4E` (`(word + 7) << 8`) and clamps the middle output component
against it. `$C291C8` then performs the direct store:

```
D5 -> $C45C3E
D6 -> $C45C42
D7 -> $C45C46
```

The path later updates these values again through the `$C45C4A` accumulator
at `$C29574-$C295AE`, and writes masked/negated companions to
`$C45C32-$C45C3A`. Thus `$C45C3E/$C45C46` are derived mutable origin state,
not bytes copied directly from a static terrain template.

## Terrain-selection consequence

When `$C45785` is nonzero, `$C1C8B8-$C1C8F8` reduces the high words of the
X/Z origin pair and uses them as live inputs to `$C1D10C`'s static-template
selection. The controlled probe proves that changing the pair changes both
the chosen static source subset and generated placements. This connects
normal active-record/matrix state to terrain paging, but leaves the upstream
meaning and immutable source of the selected control-record fields open.

It is therefore valid to describe `$C45C3E/$C45C46` as the selector's live
origin inputs. It is not yet valid to call them aircraft coordinates, absolute
world coordinates, or a decoded map-cell address without a controlled motion
experiment and the upstream control-record producer.
