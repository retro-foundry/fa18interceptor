# run075 frame 559 `$C2F8B4` plane packet

The canonical transition reaches `$C2F8B4` at engine frame 564 after the
`$C2F688` coordinate transform. Entry values are `D0.w=$FFFD`,
`D1.w=$FFFD`, `D2.w=$FFFD`, `D3.w=$FFFF`, `D4.w=$0002`, `D6.w=$0002`, and
`D7.w=$0000`. The four lane pointers are separated by `$1F40` bytes.

The leaf performs three word operations:

| Lane | Operation | Mask |
| --- | --- | ---: |
| lane 2 | AND | `$FFFD` |
| lane 1 | OR | `$0002` |
| lane 0 | OR | `$0000` |

This is a planar lane handoff. The low bit masks are meaningful only after
the caller's plane order and word coordinate are retained; collapsing the
lanes immediately into one chunky pixel cannot represent the intermediate
operation. The native renderer therefore needs a semantic temporary plane
page for this transition rather than an Amiga address mirror.

Authority: `build/run075_c2f8b4_558x_v2/trace.jsonl`, captured from the
canonical run075 restore and playback with a breakpoint at `$C2F8B4`, armed at
frame 558. The breakpoint reached frame 564.
