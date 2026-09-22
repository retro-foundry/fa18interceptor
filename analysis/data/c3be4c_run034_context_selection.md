# `$C3BE4C` run034 renderer-context comparison

Classification: **scenario-backed shared static-input / divergent renderer batch observation**. It is a candidate scene-or-range selection boundary, not a demonstrated LOD rule.

## Matched static input

No-input `$C0F090` windows restored from run034 checkpoints at replay frames 4,200 and 9,500 both execute `$C1F4AC` with the same immutable input `$C3BE4C = (576, 4224, -576)`, destination `$C48390`, and matrix `$C45BD8`. `$C48390` is a mutable transform workspace, so this does not export a terrain vertex or establish a world position.

## Source-bounded renderer batches

`collect_matrix_instance_geometry.py` captures submissions from that `$C1F4AC` entry up to the next entry (`$C3A96E` in both samples):

| Checkpoint | bounded interval | observed output |
| --- | ---: | --- |
| far 4,200 | 50,949 stepped instructions | Three polygons (3-, 4-, and 5-point) plus three two-segment line lists (selectors 7 and 12); polygon state was mutable `$C4BFCA` / `$0001B6`. |
| retreat 9,500 | 4,061 stepped instructions | Three four-point polygons, all with static `A5=$C3BBF6`; no line-list submission. |

A shared transform input therefore enters materially different renderer contexts across this flight. It does not identify a terrain cell, city object, or LOD level: position, heading, clipping, and distance all differ, and the far batch has mutable final-renderer contexts.

## Consequence

The next trace must walk from `$C3BE4C` to the branch selecting `$C3BBF6` or the far alternative, and record that branch's live selector inputs. Only an independent distance tie could promote this to LOD evidence.

Authorities: `analysis/data/run034_{far,retreat}_matrix_inputs.json` and `build/run034_{far,retreat}_c3be4c_instance_geometry/instance_geometry.json`.
