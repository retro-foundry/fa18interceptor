# `$C34C` plus `$C34A9A`: co-rendered flight-object composite candidate

Classification: **shared static template transform with separately transformed renderer layers**.

The C34C five-face component and the much larger `$C34A9A/$C34A9C` face family should remain separate export boundaries, but they have a trace-proven association in the same renderer pass:

- At replay frame 1966, `$C0D334` first selects `$C46228` from the active `$C46184` record bank and calls `$C0D384`; it then selects `$C48390` plus the next static-stream offset and calls the same derived-vertex helper again.
- In the Golden Gate pre-clip collector, the five `$C45BEA` / `$C34C06-$C34C48` submissions occur consecutively through `$C203C4`. They are immediately followed by `$C34A9A` submissions through `$C200EC`, reading the `$C483xx` workspace built by the second helper call.
- The direct writer for a C34A-consumed record, `$C483B8`, is `$C1F2CE: move.w d4,(a3)+`. A bounded lead trace reaches it from the same `$C1F100/$C1F21C` pass that enters with `A1=$C3515E`, `A0=$C46228`, and `A3=$C48390`. `$C1F2CE` writes the alternate lane with matrix `$C45BD8`; this is a distinct matrix from the `$C45BEA` display context attached to the C34C face calls.

This makes a common composite renderer pass trace-proven. The subsequent [dual-lane source trace](c351_shared_dual_lane_flight_object.md) proves that both layers derive from the same `$C351xx` input stream. Their captured coordinates are [not spatially comparable](c34a_c34c_frame12000_instance_separation.md), because each lane applies a different transform. Export must retain the two static face-record layers and procedural transforms rather than collapsing them into an asserted immutable mesh or world-instance placement.

The discriminating evidence is deliberately retained as dataflow rather than an object-name claim: C34C faces use `$C46228` and `$C45BEA`; C34A faces use `$C48390` and `$C34A9A`.

A captured `$C1F99A` lane beginning at `$C48390` resolves to `$C485E2`, not the `$C483xx-$C484xx` C34A face records. [That rejected source candidate](../routines/c1f99a_descriptor_lane.md) prevents this common pass from becoming an unsupported shared-static-mesh claim.
