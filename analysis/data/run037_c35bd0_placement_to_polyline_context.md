# Run037 `$C35BD0`: placed map polyline to line-emission context

Classification: **single-trace placement-to-source-to-line ownership**. This
establishes a bounded placed map component, not a named landmark or global
mesh.

The uninterrupted run037 M-map trace records template `$C42707` creating
runtime placement `$C4EC02=(0,0,-328)` with descriptor `$C22714`. At trace
index 140523, `$C1CC70` reads that descriptor's `+8` field `$C35BD0`; at
140560, `$C1EF10` publishes `$C35BD2`.

Before the following walker, `$C1F4AC` enters at 140629 with immutable source
`$C35BDE`, transforming exactly its three direct local triples. The walker
then enters at 140748 with cursor `$C35BF0`, and `$C1F70E` loads the published
`$C35BD2` into `A5` at 140751. The decoded `$C35BF2` offset-pair list selects
local edges `0 -> 1` and `1 -> 2`; both reach `$C2FA7E` at indices 140900 and
141154 with `A5=$C35BD4`.

```text
$C42707 -> $C4EC02 (0,0,-328) -> $C22714 +8 $C35BD0
  -> $C35BD2 -> $C35BDE local triples -> $C35BF0/$C35BF2
  -> two $C35BD4 line submissions
```

The exact local geometry, topology, and retained trace indices are exported
in [`c35bd0_bounded_instance_asset.json`](c35bd0_bounded_instance_asset.json).
The output points remain matrix-transformed workspace values, not a static
global-coordinate polyline.
