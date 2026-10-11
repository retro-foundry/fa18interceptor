# Run031 frame-14,500 later-bridge polygon submissions

Classification: **runtime-backed later-bridge candidate subset**. The checkpoint is a cockpit view with a visible multi-pier bridge silhouette; the bridge's proper name remains unassigned.

The bounded `$C2FF48` collector recorded 15 finalized polygon submissions over 12 no-input replay frames from the sealed frame-14,500 checkpoint. [Raw records](../../build/run031_frame14500_bridge_polygon_submissions/polygon_submissions.json) preserve all contexts.

Exactly three submissions retain `A5=$C36298`, an address inside static, byte-stable scene Hunk 43 (`$C36208-$C36A1B`). They yield three closed polygons / ten edges, available as [the `$C36298` orthographic subset](../plots/later_bridge_c36298_polygons_orthographic.svg). This is materially stronger than the earlier single `$C37EA0` line packet, but it is not yet the complete bridge extraction: `$C4BFD0`, `$C4BFBE`, `$C3B50A`, `$00000A`, `$C3B4FE`, and `$000009` submissions remain unassigned and deliberately excluded.
