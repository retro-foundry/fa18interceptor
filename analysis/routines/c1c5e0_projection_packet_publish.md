# `$C1C5E0`: projection packet publication

Classification: **scenario-backed port contract**.

The observed source at `$C1C5E0-$C1C63D` reads a root record's `+$14`, `+$18`,
and `+$1C` longwords. It masks X and Z with `$003FFFFF`, adds the incoming
three longwords, negates all three values, shifts each by eight bits, and
writes the low words to the renderer projection tuple. The complete shifted
middle value is also retained as the depth metric.

The run060 frame-8246 packet supplies:

```text
root  = (11982C00, 00007708, 1059A000)
input = (00000000, 00000500, 00001400)
```

The unshifted negated tuple is `(-1584128, -31752, -1684480)`. The published
native packet is `(x,y,z)=(-6188,-125,-6580)` with `depth_metric=-31752`.

Authority: `analysis/data/run060_root_projection_packet.md` and
`build/run060_frame8241_c1c5e0_projection_packet/trace.jsonl`.

`port/projection_packet.c` represents this as named root/input/output structs.
It does not model the original record address or `$C45A62/$C45A72` workspace.
