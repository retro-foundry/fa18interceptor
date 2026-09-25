# Demonstration-flight `$C3B720` matrix observation

Classification: **event-faithful local-to-renderer transform observation**.
This fixes the transform formula and one live matrix/output sample; it does
not prove that the placement tuple is directly added to each local vertex.

An event-faithful replay reaches `$C1F4AC` at demo frame 117 with
`A1=$C3B720`, `A3=$C48390`, and `A4=$C45BD8`. The source's five triples are
processed by `$C1F4AC/$C1F524`. The matrix words at entry are:

```text
$C45BD8: (168, 0, 0,  0, 0, 252,  0, -128, 0)
```

The first five transformed `$C48390` slots after the bounded trace are:

```text
(-5355, -9513, 16384)
(-6731, -10080, 16384)
(-6573, -8442, 16384)
(-5691, -8301, 16384)
(-6258, -9072, 16128)
```

The byte-exact routine first shifts each source triple by a live count, adds
live `D0/A5/D1` terms, then applies three signed dot products from `$C45BD8`
and arithmetic-shifts each result by eight. Thus the renderer slots are a
matrix transform of local data plus live state, not a direct copy or a proven
`placement + vertex` world-space sum.

Authority: `build/attract_demo_c3b720_matrix_trace/{report.json,slow.bin,trace_final_slow.bin,trace.jsonl}` and
[`transform_c1ee14_alt_branch_record.asm`](../../source_amiga/observed/transform_c1ee14_alt_branch_record.asm).
