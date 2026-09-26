# `$C1F4AC/$C1F524`: local vertex fixed-point transform

Classification: **port-contract transform core**.

The byte-exact slices `transform_c1ee14_alt_branch_record.asm` and
`transform_c1ee14_alt_branch_loop.asm` establish the common local-triple
operation. `$C1F4AC` transforms one triple; when its caller-owned loop state
permits, `$C1F524` repeats the same operation for later triples.

For local triple `(x, y, z)`, live shift `s`, translation `(tx, ty, tz)`, and
signed matrix rows `m[0..2][0..2]`, the proved core is:

```text
v = (asr16(x, s) +16 tx, asr16(y, s) +16 ty, asr16(z, s) +16 tz)
out[row] = low16(asr32(sum(v[column] * m[row][column]), 8))
```

`+16` denotes 68000 word-width wrapping. Each product is signed 16 Ã— 16;
the additions are long-width wrapping before the arithmetic right shift.

## Demonstration fixture

The event-faithful run075 replay reaches `$C1F4AC` at demo frame 117 with
five raw triples. The source trace supplies shift `1`, translations
`(-9216, -32768, -9216)`, and rows:

```text
(168, 0, 0)
(0, 0, 252)
(0, -128, 0)
```

The raw-to-output contract is:

| local input | transformed output |
| --- | --- |
| `(2112, 0, -896)` | `(-5355, -9513, 16384)` |
| `(-2080, 0, -2048)` | `(-6731, -10080, 16384)` |
| `(-1600, 0, 1280)` | `(-6573, -8442, 16384)` |
| `(1088, 0, 1568)` | `(-5691, -8301, 16384)` |
| `(-640, 1024, 0)` | `(-6258, -9072, 16128)` |

Authority: `build/attract_demo_c3b720_matrix_trace/{slow.bin,trace.jsonl,
trace_final_slow.bin}`, `analysis/data/attract_demo_c3b720_matrix_observation.md`,
and the observed assembly slices above.

## Native port

`port/transform.c:fa18_transform_vertices` exposes the proved core with
`FA18LocalVertex`, `FA18VertexTransform`, and `FA18TransformedVertex`. It is
a C data transformation, not an emulation of the source register or workspace
layout. `fa18_transform_contract_test` checks all five live demo vertices and
a separate word-wrap boundary.

The caller-owned condition controlling `$C1F524`, record decoding before the
first triple, and routing into projection remain separate contracts.
