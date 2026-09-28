# `$C2AA9C` map parent pass composition

`map_packet_parent_pass.{c,h}` composes the ported depth stage and
original-data pass in source order:

```text
$C1C636 projection depth → $C2AA9C metric/bounds
→ normal pass when metric > $3F8 → wide pass always
```

The frame-382 source trace has full depth `-125` and scale inhibition set,
therefore metric `125`; it skips normal and enters the wide low-filter route.
The native contract follows that route through caller-owned filter-row state,
the original `$C2ACA8` control bytes, source packet directory/payload, and the
existing display callback.

The caller still owns the live record fields, display matrix, detail selectors,
page identity, and page submission callbacks. No frame-number trigger or
captured page is used.
