# Connected native frame cleanup and debug overlays

The actual `fa18_native` path is `native_frontend_tick` ->
`native_flight_tick` -> HUD -> `native_frame_selection_cleanup` -> yielding
timer poll -> counter -> `native_frame_debug_overlay` -> the frontend's
C32CEE final message. The idle branch also executes the overlay gate.

C0F2DC/C0F2E4 now set marker $1D4 and call C12242's existing lost-selection
owner. An inactive selected record clears target/view selection, requests
redraw and, outside a context, resets the cockpit view and publishes key zero.
The following C31F4A is an empty RTS. C0F386's two original byte gates now
control C2F49C's page mark and C31B76's numeric fields, followed by marker $220.
C2F49C sets the original line style/colour and draws (0,0)-(8,9) on page one.
C31B76 uses the shared source owner, with explicit values passed to native
BCD/text children. No CPU registers, opcodes or chipset services enter the
native runner.

`native_frame_tail_oracle.c` compares the actual native helpers against the
original C0F2DC-C0F2F0 and C0F386-C0F3BA caller ranges, executing their original
children. All non-stack RAM, including every drawing plane, agrees in 64
selection cases and 256 overlay cases. Cases cover all 16 target slots,
active/lost records, both context routes, both page/gate states, signed numeric
edges, optional input fields and last-row clipping. The exact native raster
branch is used on the host side; original instructions/blitter are test only.

`check_frame_tail.py` runs 2,400 recorded native updates, then compares those
source ranges using its live disk-backed state. There are 275 HUD passes; its
pending timer checkpoint now carries $1D4. The checkpoint PPM is unchanged
from the preceding viewport batch. The entire native demo completes 4,892
updates and differs from the previous final RAM only in the stage marker byte.
Crash/menu/qualification re-entry and frontend/menu/save/SDL/link omission
checks pass. Both reference compiler builds and the 12 reference contracts
pass. No full original replay was run; Copper fade remains excluded from
recorded-frame acceptance.

This batch connects **3/3 source owners (100% of this batch)**. Of the six
identified missing frame owners (C12242, C2F49C, C31B76, C30A00, C2B564,
C2B3C2), **3/6 are now connected (50% of that inventory)**. This measures
connections, not equal effort or whole-game completeness. Debug gates remain
zero in the demonstration recording; their active rendering is demonstrated
by the bounded caller-range cases, not by that recording. Stores icons,
grid/record markers and scene-position labels remain unconnected. Native full
recorded-frame parity is still 0/3 accepted and the startup lead is 37 ticks.
