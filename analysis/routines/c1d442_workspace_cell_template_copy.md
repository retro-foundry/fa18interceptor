# `$C1D442-$C1D4C2`: static template stream to workspace-cell copy

Classification: **scenario-backed static-to-mutable scene-workspace expansion**.
This is a direct upstream input to the placement builder, but not yet an
extracted terrain mesh, global coordinate table, or LOD scheme.

## Authority

- Sealed `run033` replay, restored before replay frame 404 and traced for the
  next three chipset frames:
  `build/run033_placement_bulk_404_trace/trace.jsonl`.
- Original CODE segment 66 is verified at `$C42290-$C429C7`.  Its relocation
  operands do not overlap source offset `$531-$536` (`$C427C1-$C427C6`), as
  recorded in `analysis/hunk_inventory.json`.
- The downstream placement-cache contract is documented in
  [`$C1DC1C-$C1E0B0`](c1dc1c_scene_placement_record_builder.md).

## Captured copy

After the `$FFFF` cell-marker reset, the frame-1 trace reaches this exact
source/destination pair:

| Trace instruction | Source or destination | Observed value / effect |
| --- | --- | --- |
| `$C1D488` | `(A5)+=$C427C1` | reads header byte `$6E` |
| `$C1D490` | `(A2)+=$C4B270` | writes header bit 7 only: `$00` |
| `$C1D496` | `(A2)+=$C4B271` | writes header low seven bits: `$6E` |
| `$C1D4BC` | `$C427C2-$C427C5` to `$C4B272-$C4B275` | copies `$08 00 08 00` as one longword |
| `$C1D4BE` | `$C4B276` | writes the workspace terminator `$FF` |
| `$C1D442-$C1D448` | next source byte `$C427C6` | reads `$FF` and exits this copy loop |

Thus the static six-byte sequence `$6E 08 00 08 00 FF` becomes the mutable
cell prefix `$00 6E 08 00 08 00 FF`.  The split header is intentional in the
observed code: `$C1D48C` masks `D7` with `$80`, while `$C1D492` masks `D0`
with `$7F`.  This is a transformation, not a blind byte-for-byte copy.

## Downstream consumer observed on the next frame

At trace frame 2, `$C1DD2C` accepts `$C4B270` because its leading byte is no
longer `$FF`.  `$C1DD36` then reads its first word as `$006E`; the selector
calculation reaches `$C1DD88` with `A1=$C22A20`.  The following reads at
`$C1DD98` and `$C1DE0A` consume the two copied `$0800` words at `$C4B272` and
`$C4B274`.  The builder combines them with mutable/live translation terms
before emitting a runtime placement record, as separately established by the
placement-builder trace.

This proves that a verified static segment-66 template stream repopulates a
cell that the placement builder subsequently consumes.  It also resolves the
apparent reset contradiction: `$C1D722` marks the cell rejected, then this
copy path selectively replaces the prefix before the next builder pass.

The selector's immediate `$C1D520` child is now byte-exactly reconstructed as
[`append_template_workspace_matches.asm`](../../source_amiga/observed/append_template_workspace_matches.asm).
It conditionally scans two bounded mutable record regions, clears a matched
bit, and appends only `$10/$40`, index, `$FF` marker triplets through the same
`A2` workspace cursor. This is auxiliary mutable-workspace dataflow, not an
additional immutable terrain-template source.

## Boundary

The source record supplies a selector-like header and two word inputs in this
scenario.  Its direct semantic meaning and coordinate frame are unproven; the
final placement words are computed rather than directly copied.  Therefore it
is evidence for an authoritative *terrain-scene placement-control input*, not
for raw terrain vertices, a complete flat-world map, or any LOD rule.  The
next useful trace follows multiple segment-66 records through this copy and
the builder while varying the flight position.
