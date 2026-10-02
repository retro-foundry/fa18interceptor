# Placement-cache ordering: complete domain body

2026-10-02. `port/game/placement_order.c` implements the complete memory
behavior of $C1E540-$C1EBAE as readable C. It is not yet registered: the
CPU adapter and source/event timing are outstanding. Coverage remains
421/624 and 197 registered timing-step entries.

## Source contract

The parent is the 523-instruction function called by the $C1C9AE return
site. Its early return at $C1E53C/$C1E53E precedes the public entry. The C
body covers all early exits and the complete suffix-classification and
partition paths, rather than just the selection prefix.

- Scan the negative-word-terminated 24-byte cache at $C4F6CA.
- Find the last descriptor whose selected flag byte has bit $10; publish
  its index at $C4FD5A.
- Read its packed/control-record cell pair and identify the nearby suffix
  with the source's $C00 component limit; publish $C4FD5C.
- Classify the suffix against that reference. Later entries first use the
  $A00 proximity check. Preserve ordinary packed positions, control-record
  bit-4 positions, workspace-record bit-6 positions, both special height
  routes, and the $22 fault-hook path.
- Preserve both plane lists and indexed polygon/triangle lists, including
  the grouped 2D edge test and the optional triangle-side test.
- Set $C000/$8000 class bits, publish the last classified index at $C4FD5E,
  copy to $C48390 scratch, and write the first class, reference and second
  class back in source order. Preserve scratch masks and negative markers.

Positions retain the source's mix of signed words and signed longs. Cell
steps use the existing source-backed helpers. Arithmetic wrappers express
16/32-bit wrapping explicitly. SUB/ADD followed by signed branches retain
the original overflow behavior; saved-dot TST uses the wrapped value.
Record copies read all six longs before writing, preserving alias behavior.
The domain has no CPU registers, CCR, instruction handlers or timing fees.

## Evidence and discovered argument dependency

The first 4,096 structural memory fixtures passed, but the recorded demo
exposed 95 domain-memory mismatches. The first was call 1044, at scratch
$C483A8: the original marked the second copied entry negative, while the
C body classified it into the other group. Carrier/failure recordings
already matched; they did not establish this demo path.

The bounded original-instruction trace of the captured call resolves the
cause. At $C1E7F6 the reference position helper inherits the preceding
cell-step/index result in D1. Its low four bits are zero. It does not
receive the reference header's shift, unlike the initial anchor-position
call. For the observed reference $C4F7EA, the plane path's source position
is x=$3150, z=$0200, rather than the anchor's x=$0540, z=$0800. The native
plane path now passes zero shift at that specific call. The captured
failing entry then matches all Chip/Slow RAM outside its dead private stack.

The fixtures now also vary the reference shift; 8,192 complete structural
calls pass. They include empty/single/ineligible caches, descriptor flag
formats, bit-4/bit-6 entries, proximity exits, special heights, plane lists,
indexed polygon/triangle lists, scratch partitioning, all incoming CCR
combinations and randomized incoming registers. This is a memory-effects
proof; it does not compare the resulting CPU registers or SR.

The fresh recorded memory-only check now passes on all three full sessions:

| Recording | Shadow memory matches | Shadow incomplete | Sandbox memory matches |
| --- | ---: | ---: | ---: |
| demo01 | 2,072 | 15 | 2,087 |
| qual_carrier_success | 1,650 | 0 | 1,650 |
| qual_fail_crashes | 353 | 0 | 353 |
| Total | 4,075 | 15 | 4,090 |

There are zero mismatches and zero hardware classifications. Incomplete
calls remain separate. The small summary is retained in
`analysis/figures/native_placement_order_memory_checkpoint.json`.
Reproduce both structural and recorded comparisons:

```text
python tools/recomp/check_placement_order.py --cases 8192 --recorded
```

The launcher builds an isolated `placement_memory.exe` with one proof entry
and a temporary caller-liveness mask retaining A6/A7. It intentionally
excludes the other CPU result registers/CCR. The production registry,
generated liveness and normal runner remain intact. It uses the existing
shadow/sandbox memory comparison, including its dead-stack/DMA exclusions.
The structural oracle independently compares all Chip/Slow RAM outside the
bounded private stack under held machine events, without the recorded
framework's DMA exclusion. No complete port, live RGB, timing, sealed RAM
or poison claim is inferred from this narrower proof.

GNU headless and MSVC Release builds pass. Ignored proof logs and JSON
reports remain under `build/recomp/placement_order_*` and
`placement_memory_*.json`. The entry capture and short source CSV there are
small diagnostic fixtures, not bulk RGB streams. Capture/replay a specific
original entry with:

```text
python tools/recomp/check_placement_order.py --recorded --capture-call 1044
python tools/recomp/check_placement_order.py --fixture build/recomp/placement_demo01_shadow_call1044.bin
```

## Remaining integration

Implement the temporary CPU adapter in `port/game/glue/`, reconstructing
the live D5-D7 and A1-A7 outputs, including high halves. Keep those effects
out of `placement_order.c`; do not execute side-effecting children twice.
Prove the full adapter with original-byte fixtures and the unchanged normal
liveness table. The typed domain body is now the implementation to reuse.

Add source timing for the complete parent and the remaining fixed-charge
children C1EBC0, C1EBE0, C1EC3A, C1EC96, C1ECD4 and C1ECFC. C1EBB0/C1EC84
already have timed shared tails and can share the relevant child bridge
operations. C06C02 is already timed. Run changed-group DMA fixtures, the
short isolated probe, whole-call CPU/memory proof, full isolated RGB/seal
gate, and the full normal shadow/sandbox/seal/poison gate before registering
this as the 422nd entry. C1D10C remains the other parent for 423/624.
