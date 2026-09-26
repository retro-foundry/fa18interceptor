# run075 frame 559 `$C2F688` plane packet

The canonical transition reaches `$C2F688` at engine frame 561. Entry
coordinates are `D0.w=140` and `D1.w=105`. The leaf selects a table entry
using the low nibble of `$C45954`, derives the planar row and word offset from
the coordinates, and loads four plane destinations from a caller supplied
packet. The selected destinations are separated by `$1F40` bytes.

At the first visible word update, the four operations are:

| Lane | Operation | Word value |
| --- | --- | ---: |
| lane 0 | OR | `$0000` |
| lane 1 | OR | `$0000` in the low word of `D5` |
| lane 2 | AND | `$FFF0` in the low word of `D2` |
| lane 3 | AND | `$FFFF` in the low word of `D3` |

The leaf therefore has the same semantic shape as `FA18PlaneWordUpdate`: a
caller selected word coordinate and page, followed by per-plane clear/set
masks. Its address arithmetic is not part of the native state model.

Authority: `build/run075_c2f688_558x_v2/trace.jsonl`, captured from the
canonical run075 restore and playback with a breakpoint at `$C2F688`, armed at
frame 558. The breakpoint reached frame 561.
