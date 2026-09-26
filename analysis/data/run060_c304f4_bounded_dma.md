# run060 `$C304F4` bounded DMA result

The replay probe stops at `$C304F4`, arms the next instruction at `$C304F8`,
and continues for 80 single instructions. The bridge resumes ordinary frame
execution between those instructions, so this is a bounded replay window,
not a single-frame DMA isolation. Its before/after Chip snapshots differ in
exactly 1,790 bytes, with the same range and values as the earlier isolated
capture:

```text
range: $13A82..$14267
destination plane: active plane 1
rows: 94..144
```

The matching delta strengthens the earlier capture correlation, but later
frame writes cannot be excluded from this window. It is supporting evidence
for implementing the `$0DFC` (`A OR B`) area operation, not an authoritative
single-operation output fixture.

Authority: `build/run060_c304f4_bounded_dma/chip.bin`,
`trace_final_chip.bin`, and `trace.jsonl`.
