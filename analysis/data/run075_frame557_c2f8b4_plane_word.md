# run075 frame 557 `$C2F8B4` plane word update

The canonical run075 replay reaches `$C2F8B4` at engine frame 557 during the
HUD transition after the frame553 display operation. The leaf performs these
word operations:

| Destination lane | Operation | Mask |
| --- | --- | ---: |
| first active lane (`A0`) | OR | `$0000` |
| next lane (`A1`) | OR | `$0002` |
| next lane (`A2`) | AND | `$FFFD` |
| fourth lane (`A3`) | unchanged by this leaf | — |

The four entry pointers are `A0=$01A360`, `A1=$0183C0`, `A2=$016320`, and
`A3=$0145A0` in the trace's address view. The active lane pointers are
separated by `$1F40` where the caller supplies the plane sequence. The leaf
establishes a clear of one bit in one plane and a set of the same bit in the
next plane; it does not establish the displayed coordinate or page identity.

Authority: `build/run075_c2f8b4_536x/trace.jsonl`, captured from the canonical
run075 restore and playback with a breakpoint at `$C2F8B4`, armed at frame
536. The breakpoint reached frame 557.
