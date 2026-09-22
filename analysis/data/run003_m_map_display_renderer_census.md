# Map display renderer census

Classification: **scenario-backed prepared-map-page renderer output**.

The supplied job inventory identifies CPU blits whose pointers fall in the prepared map page; the trace identifies renderer entries in the same bounded transition. Polygon routes are bounded from wrapper entry to its observed `$C24D66` return; control rows are bounded to the next walker entry (or trace end), so the final control row can include later unrelated line work. This proves renderer output to that mutable display page, not an immutable terrain mesh, a coastline-pixel-to-record match, or a complete world-map extraction.

- Polygon wrapper entries (`$C2FF48`): 42
- Line emitter entries (`$C2FA7E`): 19
- Blitter jobs with a pending-map-page pointer: 124
- Direct span jobs (`$C304F4`): 44
- Line-plane jobs (`$C2FBE6/$C2FC4E/$C2FCB6/$C2FD1C`): 76

## Polygon wrapper entries by frame

| Frame | Entries | `$C2FA7E` within wrapper return path | `$C304F4` within wrapper return path |
| ---: | ---: | ---: | ---: |
| 1 | 4 | 0 | 4 |
| 2 | 3 | 0 | 3 |
| 3 | 6 | 2 | 4 |
| 4 | 4 | 0 | 4 |
| 5 | 5 | 0 | 5 |
| 6 | 5 | 0 | 5 |
| 7 | 6 | 0 | 6 |
| 8 | 5 | 0 | 5 |
| 10 | 4 | 2 | 2 |

## Static control entries and bounded primitive outputs

| Trace frame | Transform input (`A1`) | Raw triples | Control entry (`A1`) | Line endpoints to next control entry | Polygon line routes | Polygon span routes |
| ---: | --- | --- | --- | --- | ---: | ---: |
| 9 | `$C35BDE` | `[[11136, 0, -9088], [6656, 0, -3840], [-13568, 0, 2944]]` | `$C35BF0` | `[[148, 123, 152, 118], [152, 118, 173, 112]]` | 0 | 0 |
| 9 | `$C35BAA` | `[[0, 0, 3744], [0, 416, -3168]]` | `$C35BB8` | `[[97, 56, 97, 60]]` | 0 | 0 |
| 9 | `$C35BC2` | `[[0, 0, -3744], [0, 416, 3168]]` | `$C35BD0` | `[[97, 63, 97, 59]]` | 0 | 0 |
| 9 | `$C36220` | `[[4096, 172, -5248], [-2816, 172, 1152], [-8960, 0, 2816]]` | `$C36232` | `[[118, 66, 125, 60], [125, 60, 132, 59]]` | 0 | 0 |
| 9 | `$C36220` | `[[4096, 172, -5248], [-2816, 172, 1152], [-8960, 0, 2816]]` | `$C36232` | `[[118, 66, 125, 60], [125, 60, 132, 59]]` | 0 | 0 |
| 9 | `$C3B720` | `[[2112, 0, -896], [-2080, 0, -2048], [-1600, 0, 1280], [1088, 0, 1568], [-640, 1024, 0]]` | `$C3B73E` | `[[107, 94, 111, 95], [110, 92, 111, 95], [211, 0, 211, 179], [89, 0, 89, 179], [0, 140, 319, 140], [0, 11, 319, 11], [31, 130, 31, 130], [25, 131, 35, 131], [26, 132, 34, 132]]` | 2 | 2 |

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
