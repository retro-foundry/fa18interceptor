# `$C34C` plus `$C34A9A`: co-rendered flight-object composite candidate

Classification: **common renderer pass and adjacent geometry construction; not a merged immutable mesh**.

The C34C five-face component and the much larger `$C34A9A/$C34A9C` face family should remain separate export boundaries, but they have a trace-proven association in the same renderer pass:

- At replay frame 1966, `$C0D334` first selects `$C46228` from the active `$C46184` record bank and calls `$C0D384`; it then selects `$C48390` plus the next static-stream offset and calls the same derived-vertex helper again.
- In the Golden Gate pre-clip collector, the five `$C45BEA` / `$C34C06-$C34C48` submissions occur consecutively through `$C203C4`. They are immediately followed by `$C34A9A` submissions through `$C200EC`, reading the `$C483xx` workspace built by the second helper call.

This makes a common composite renderer pass plausible. It is compatible with the C34C detail being a forward/nose-like or other supplemental part of the aircraft-like C34A family. It does **not** prove that the static vertex sources belong to one immutable model, because `$C34A9A` still lacks its own source transform and uses a distinct mutable workspace.

The discriminating evidence is deliberately retained as dataflow rather than an object-name claim: C34C faces use `$C46228` and `$C45BEA`; C34A faces use `$C48390` and `$C34A9A`.

A captured `$C1F99A` lane beginning at `$C48390` resolves to `$C485E2`, not the `$C483xx-$C484xx` C34A face records. [That rejected source candidate](../routines/c1f99a_descriptor_lane.md) prevents this common pass from becoming an unsupported shared-static-mesh claim.
