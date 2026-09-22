# Map display renderer census

Classification: **scenario-backed prepared-map-page renderer output**.

The supplied job inventory identifies CPU blits whose pointers fall in the prepared map page; the trace identifies renderer entries in the same bounded transition. This proves renderer output to that mutable display page, not an immutable terrain mesh, a coastline-pixel-to-record match, or a complete world-map extraction.

- Polygon wrapper entries (`$C2FF48`): 42
- Line emitter entries (`$C2FA7E`): 19
- Blitter jobs with a pending-map-page pointer: 124
- Direct span jobs (`$C304F4`): 44
- Line-plane jobs (`$C2FBE6/$C2FC4E/$C2FCB6/$C2FD1C`): 76

## Polygon wrapper entries by frame

| Frame | Entries | `$C2FA7E` seen before next wrapper entry | `$C304F4` seen before next wrapper entry |
| ---: | ---: | ---: | ---: |
| 1 | 4 | 0 | 4 |
| 2 | 3 | 0 | 3 |
| 3 | 6 | 2 | 4 |
| 4 | 4 | 0 | 4 |
| 5 | 5 | 0 | 5 |
| 6 | 5 | 0 | 5 |
| 7 | 6 | 0 | 6 |
| 8 | 5 | 1 | 5 |
| 10 | 4 | 3 | 2 |

## `$C2FF48` entry contexts

| Context at wrapper entry | Count |
| --- | ---: |
| `$C3B6B0` | 4 |
| `$C4BFA6` | 2 |
| `$C4BFAC` | 5 |
| `$C4BFB2` | 5 |
| `$C4BFB8` | 1 |
| `$C4BFBE` | 9 |
| `$C4BFC4` | 4 |
| `$C4BFCA` | 5 |
| `$C4BFD0` | 2 |
| `$C4BFD6` | 3 |
| `$C4BFE2` | 1 |
| `$C4BFE8` | 1 |
