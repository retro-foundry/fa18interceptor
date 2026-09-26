# run075 frame 537 `$C32FCE` control scan

The canonical run075 replay reaches `$C32FCE` at engine frame 537 during the
HUD transition. Entry state includes `D0=$005A`, `D1=$0006`, `D2=$00099000`,
`D3=$FFFF9000`, `D5=$0798`, `D6=$0009`, `D7=$01C2`, `A2=$C40EBC`, and
`A3=$C3D8FC`.

The routine first consumes a byte from `(A2)`, checks the record class, and
reads the next byte as an index. It scales that index by four and stores the
result at `$C45746`, then stores the source byte at `$C457DC`. It later masks
the low nibble of `D1`, selects a table entry from `$C4574A`, stores the
selected value at `$C457DB`, and refreshes the saved layout pointers at
`$C4570A` and `$C456FE` before branching to `$C330F4`.

This entry does not directly call `$C33058`, `$C330FE`, `$C2F8B4`, or the
display blitter leaf. The frame537 pixel delta therefore comes from the
downstream display loop or state consumed by it; the native frame path remains
unresolved until that producer is traced.

Authority: `build/run075_c32fce_536x/trace.jsonl`, captured from the canonical
run075 restore and playback with a breakpoint at `$C32FCE`, armed at frame
536. The breakpoint reached frame 537.
