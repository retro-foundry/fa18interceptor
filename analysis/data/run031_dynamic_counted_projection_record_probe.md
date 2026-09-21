# Run031 dynamic counted-projection record probe

Classification: **runtime-backed dynamic-data observation with negative mutation result**.

At ordinary full replay frame 12,001, execution reaches `$C211DC` with `A2`
at `$C35734` and the expected header `$0D01,$002A`. The first six-word record
at `$C483BA` is then:

```text
0101 0157 029E 0101 017E 029B
```

This differs from the `$C483BA` sample captured at the restored frame-12,000
checkpoint (`027D 0081 0026 027D 00A7 0023`). Thus `$C483BA` is not an
immutable model-data blob: it contains per-frame transformed or otherwise
runtime-dependent projection-record data.

## Controlled mutation

At the `$C211DC` breakpoint, the first word of the live record was changed
from `$0101` (257) to `$0301` (769) with `e9k_debug_write_memory`; all other
record words remained unchanged. The resulting ordinary screenshot at the
same replay frame is pixel-identical to the independent baseline.

This rejects that particular live component as a directly visible contributor
in the frame. It may be clipped, rejected, or superseded by later scene work;
the negative result does not reject the counted record packet as a whole.

## Evidence

- Mutation metadata: `build/run031_frame12000_c211dc_source_mutation_probe/probe.json`
- Baseline: `build/run031_frame12001_baseline/frame_12001.png`
- Mutation screenshot:
  `build/run031_frame12000_c211dc_source_mutation_probe/mutated_frame12001.png`
