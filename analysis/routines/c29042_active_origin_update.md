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
- The common static adjustment/publish tail `$C29548-$C295D0` is byte-exactly
  reconstructed as [`adjust_terrain_selector_origin.asm`](../../source_amiga/observed/adjust_terrain_selector_origin.asm).
- The static adjustment-mode selector `$C291D4-$C29225` is byte-exactly
  reconstructed as [`select_terrain_origin_adjustment_mode.asm`](../../source_amiga/observed/select_terrain_origin_adjustment_mode.asm).
- Its static jump-table threshold cases `$C29226-$C29367` are byte-exactly
  reconstructed as [`dispatch_terrain_origin_adjustment_thresholds.asm`](../../source_amiga/observed/dispatch_terrain_origin_adjustment_thresholds.asm).
- Two candidate-preparation targets reached by those cases are byte-exactly
  reconstructed as [`load_terrain_origin_candidate_preset.asm`](../../source_amiga/observed/load_terrain_origin_candidate_preset.asm)
  (`$C29488-$C294AB`) and [`transform_terrain_origin_candidate_small_mode.asm`](../../source_amiga/observed/transform_terrain_origin_candidate_small_mode.asm)
  (`$C294AC-$C29505`).
- The control-record candidate scan `$C29368-$C29408` is byte-exactly
  reconstructed as [`select_terrain_origin_control_record.asm`](../../source_amiga/observed/select_terrain_origin_control_record.asm).
- The active-record candidate blend `$C2940A-$C29487` is byte-exactly
  reconstructed as [`blend_terrain_origin_candidate.asm`](../../source_amiga/observed/blend_terrain_origin_candidate.asm).
- The threshold-exit range `$C29506-$C29547` is byte-exactly reconstructed as
  [`finalize_terrain_origin_adjustment_mode.asm`](../../source_amiga/observed/finalize_terrain_origin_adjustment_mode.asm).
  Together with the prior slices, this completes byte-exact reconstruction of
  the producer continuation `$C291D4-$C295D0`.

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

The reconstructed common tail later updates these values again through the
`$C45C4A` accumulator at `$C29574-$C295AE`, and writes masked/negated companions to
`$C45C32-$C45C3A`. Thus `$C45C3E/$C45C46` are derived mutable origin state,
not bytes copied directly from a static terrain template.

The tail repeatedly quarters a candidate while its supplied magnitude `D3` is
at least `$4800`, obtains a variable shift from `$C2574A`, and averages a
nonzero candidate with the previous `$C45C4A` triple before adding it to the
live origin. This proves an adjustment/smoothing dataflow. It does not prove
what physical quantity `D3` represents, nor that any preceding threshold is a
distance LOD decision.

The reconstructed selector establishes `D3` before that tail: it computes the
largest absolute component difference between candidate `$C45C56` and the
live origin `$C45C3E`, then indexes the `$C28F2C` jump table with byte
`$C457B6`. This is static/dataflow evidence for a component-delta adjustment
mode. It does not establish what selected mode values mean or connect them to
physical distance.

The decoded table has nine targets for mode bytes 0-8. Cases 0-5 and 7-8 use
literal comparison thresholds (from `$8000` through `$1C00000`) to select a
subsequent mode, route to a candidate-preparation path, or return after a
matrix helper. This demonstrates a static multi-tier threshold policy, but
does not establish mode semantics, a rendered primitive change, or a
same-instance physical-distance transition.

Two selected paths now have a bounded candidate contract: `$C29488` publishes
one of the static triples at `$C46198/$C46998` to `$C45C56`; `$C294AC` or
`$C294D2` instead calls `$C091CE` with small signed constants selected by
`$C45848`, then publishes that output. Neither source triple is a whole-world
terrain mesh, and the preset-selection context is not yet live-traced.

The mode-7 control-record path scans signed-word-terminated entries from the
live list pointer `$C4573A`, seeking a `$C46184` record with a high-nibble
`$10` class and bit 6 set. Its selected `$14/$1C` values and a derived middle
component publish another candidate triple. The list ownership, record
semantics, and any terrain relationship remain unassigned.

The selected blend path combines the live origin with `$14/$18/$1C` of the
active `$C46184 + word($C458DE)` record, then performs four rounds of
component-wise arithmetic right-shift averaging before writing `$C45C56`.
That proves a smoothed candidate construction, not world coordinates or a
terrain-cell address.

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
