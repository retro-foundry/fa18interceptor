# run075 frame 584 `$C2F8B4` plane word update

The canonical run075 replay reaches `$C2F8B4` at engine frame 584 immediately
after the later `$C330FE` workspace update. The leaf performs three 16-bit
plane word operations and returns:

| Destination lane | Operation | Mask |
| --- | --- | ---: |
| first active lane (`A0`) | OR | `$0800` |
| next lane (`A1`) | OR | `$0800` |
| next lane (`A2`) | AND | `$F7FF` |
| fourth lane (`A3`) | unchanged by this leaf | — |

The entry pointers are `$013F54`, `$015E94`, `$017DD4`, and `$019D14` in the
trace's native address view, separated by `$1F40` (8,000) bytes. That spacing
matches the four 320×200 planar page sizes. The leaf itself does not establish
the displayed page, screen coordinate, or palette index; those remain caller
state. Its proven semantic effect is a clear/set update to one planar word in
three lanes.

This is the visible handoff following the frame584 `$C330FE` glyph workspace
packet. The native port already represents the corresponding renderer family
through `FA18RendererState` and `fa18_apply_pixel_mask`; this note supplies the
later run075 lane evidence without treating the original pointers as C buffer
addresses.

Authority: `build/run075_frame580_c2f8b4_trace/trace.jsonl`, captured from
`captures/run075/restored-state.bin` and `captures/run075/playback.e9k` with a
breakpoint at `$C2F8B4`, armed at frame 580. The breakpoint was reached at
frame 584.
