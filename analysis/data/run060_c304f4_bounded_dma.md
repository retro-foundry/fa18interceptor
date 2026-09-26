# run060 `$C304F4` bounded DMA result

The replay probe now stops at `$C304F4`, arms the next instruction at
`$C304F8`, and continues for 80 single instructions without settling a full
frame. The before/after Chip snapshots differ in exactly 1,790 bytes, with
the same range and values as the earlier isolated capture:

```text
range: $13A82..$14267
destination plane: active plane 1
rows: 94..144
```

The deplanarized changed spans are therefore an operation result, not a later
frame update. This is the authoritative run060 output fixture for implementing
the `$0DFC` (`A OR B`) area operation against the semantic page.

Authority: `build/run060_c304f4_bounded_dma/chip.bin`,
`trace_final_chip.bin`, and `trace.jsonl`.
