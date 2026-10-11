# Model geometry boundaries

This report separates immutable model-input candidates from the mutable triples consumed by perspective projection. It does **not** assign a model name without an upstream coordinate-source trace and frame correlation.

## Projection consumer workspace

`$C48390-$C4E76B` is Hunk 72, declared CODE but positive-data referenced. `$C212B0` reads selected triples here into `$C4C592`, then `$C2EE4A` projects them. It is not a source-model export.

Byte-exact `$C21C2E` writes midpoint-derived triples into selected records at `+$1E` and `+$24`; therefore this range is a mutable geometry working table.

| Capture | Non-relocation bytes different from original Hunk payload |
| --- | ---: |
| baseline menu | 1,000 |
| attract frame 600 | 2,234 |
| attract frame 1800 | 2,398 |
| Golden Gate frame 12000 | 3,703 |
| bridge silhouette frame 14500 | 4,006 |

## Pre-projection record workspace

`$C45630-$C48383` is Hunk 71. `$C21C2E` selects its observed `$C47628` member before writing midpoint-derived values. It also differs from the original payload in every sampled state and is not an immutable model export.

| Capture | Non-relocation bytes different from original Hunk payload |
| --- | ---: |
| baseline menu | 481 |
| attract frame 600 | 1,390 |
| attract frame 1800 | 1,455 |
| Golden Gate frame 12000 | 1,802 |
| bridge silhouette frame 14500 | 1,845 |

A CPU write watch at `$C47628` is an expected **miss** during sealed run031, frames 1-7500. No CPU write occurs during ordinary replay from the saved initial state. The earlier no-input stepping trace writes derived midpoint fields, so population precedes this sealed replay or follows another unobserved path.

## Verified byte-stable scene-family candidates

The following Hunk payloads match their original bytes in every sampled state after complete relocation longwords are excluded. They are immutable scene-family candidates, not whole-model exports: original HUNK_CODE remains authoritative and segment 46 begins with valid 68000 instructions. Individual data records still need producer-to-projection traces before model names can be assigned.

| Hunk | Range | Bytes | Snapshot result |
| ---: | --- | ---: | --- |
| 41 | `$C34A50-$C3555F` | 2,832 | zero non-relocation differences in 5 snapshots |
| 42 | `$C35568-$C361FF` | 3,224 | zero non-relocation differences in 5 snapshots |
| 43 | `$C36208-$C36A1B` | 2,068 | zero non-relocation differences in 5 snapshots |
| 44 | `$C36A28-$C3720F` | 2,024 | zero non-relocation differences in 5 snapshots |
| 45 | `$C08718-$C089EB` | 724 | zero non-relocation differences in 5 snapshots |
| 46 | `$C37218-$C37677` | 1,120 | zero non-relocation differences in 5 snapshots |
| 47 | `$C37680-$C37983` | 772 | zero non-relocation differences in 5 snapshots |
| 49 | `$C37990-$C37F77` | 1,512 | zero non-relocation differences in 5 snapshots |
| 50 | `$C37F80-$C383E3` | 1,124 | zero non-relocation differences in 5 snapshots |

## Upstream immutable candidates

Hunk 42 `$C35568-$C361FF` is verified at runtime and supplies scene-control streams. It is an immutable candidate boundary, but controls dispatch/offsets rather than proving a particular model.

The exact static edge-list ranges below select pairs from the mutable working table and are useful topology evidence:

- `$C37EA0-$C37EA5` — `analysis/data/c37ea0_bridge_silhouette_projected_edge_list.md`
- `$C38B0A-$C38B1F` — `analysis/data/c38b0a_external_view_projected_edge_list.md`
- `$C3985A-$C39883` — `analysis/data/c3985a_projected_edge_list.md`

## Observed external-frame subdivision

The existing `build\run031_frame7500_c1f6f8_probe\trace.jsonl` trace executes `$C21C2E` twice. Its second offset is read from `$C38B06` and selects the mutable `$C48390` base.

| Selected record | Address | Six signed words before midpoint writes |
| --- | --- | --- |
| first | `$C47628` | `[47, 9, -301, -25, -5, 158]` |
| second | `$C48390` | `[-537, 1126, 2207, -525, 1109, 2181]` |

Both selected inputs are mutable Hunk-71/Hunk-72 workspace records, so this operation supplies no immutable model coordinates yet.

## Exact static-copy check

The following exact twelve-byte (six-word) records were searched across the original executable. This detects only an unmodified source copy; empty results do not exclude transformed, packed, or generated model coordinates.

| Traced record | Runtime address | Exact matches in original executable |
| --- | --- | --- |
| external-frame subdivision endpoint | `$C48390` | 0 |
| Golden Gate consecutive projection record 0 | `$C483BA` | 0 |

## Required next evidence

The sealed run031 state is already after `$C47628` population. Capture an earlier loader/scene-initialization phase, then trace the first writer into `$C46184` and `$C48390`, record its source pointers and pre-write triples, and require the source range to remain byte-identical to its original Hunk payload across snapshots. Only then correlate the resulting projected edges with a frame and name the model.
