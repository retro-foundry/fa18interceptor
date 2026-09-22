# run034 live terrain band-walk page content

Classification: **ordinary-flight static control → workspace-band → template
stream/copy inventory**. It is a bounded active-page-content inventory, not a
global terrain export, global coordinate table, or LOD table.

Two ordinary run034 page-refreshes were traced from `$C1D330` to `$C1DC08`.
This keeps the immutable segment-65 control byte, its workspace band, the
actual `$C1D3F4` call, the selected static stream, and the `$C1D488` record
copies in one continuous flight-update interval.

## Frame 4,252 refresh

| control byte address/value | workspace band | group / row | static stream | copied records |
| --- | --- | --- | --- | ---: |
| `$C4216A` / `$04` | `$C48390` | 16 / 16 | `$C42BD4` | 16 |
| `$C42170` / `$FF` | `$C48990` | terminator | none | 0 |

Thus this refresh installs `$C42BD4`'s 16 template records in the first
active band, then terminates the byte-control walk.

## Frame 9,656 refresh

| control byte address/value | workspace band | group / row | static stream | copied records |
| --- | --- | --- | --- | ---: |
| `$C417FF` / `$06` | `$C48390` | 15 / 15 | `$C42ADA` | 8 |
| `$C41807` / `$03` | `$C48990` | 16 / 15 | `$C42706` | 4 |
| `$C41819` / `$08` | `$C48F90` | 15 / 17 | none | 0 |
| `$C4181E` / `$07` | `$C49590` | 15 / 16 | none | 0 |
| `$C4182A` / `$05` | `$C49B90` | 16 / 17 | none | 0 |
| `$C4182F` / `$04` | `$C4A190` | 16 / 16 | `$C42BD4` | 16 |
| `$C41840` / `$FF` | `$C4A790` | terminator | none | 0 |

The later flight state therefore activates three distinct immutable template
streams across six band positions, with 28 exact template-record reads. The
same `$C42BD4` stream appears at a different band than in the earlier refresh;
this demonstrates reusable page content placed into a mutable cache, not a
fixed global coordinate record.

## Result

The static segment-65 bytes are active page-control data and the selected
streams are active terrain-template content in real flight. The remaining
map-extraction gap is the relationship between the live selector context and
absolute world position: the bands/cache placement and template records are
proved, but no static record has yet been tied to one global location.

Authorities:

- `build/run034_page_4252_band_walk_trace/trace.jsonl`;
- `build/run034_page_9656_band_walk_trace/trace.jsonl`;
- their frame-0 Slow-RAM snapshots.
