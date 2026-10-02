# Placement-cache ordering: complete registered parent

2026-10-02. `port/game/placement_order.c` implements the complete memory
behavior of $C1E540-$C1EBAE as readable C. The CPU adapter and complete
source/event timing are now independently proven and registered. Coverage
is 422/624 with 204 registered timing-step entries. Six existing children
also replace fixed charges with source timing.

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
combinations and randomized incoming registers. The original memory-only
proof remains reproducible; the new `--glue` mode also compares the original
caller's live CPU outputs and SR control.

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

## Complete CPU adapter and timing

`glue_placement_order.c` calls the domain once with a synchronous semantic
observer. Positions, plane normals, cross products, descriptor selections,
copy tails and cursors supply the original CPU outputs. CPU state stays in
glue; children and partition writes are not repeated. The observer retains
the MOVEM.W sign extension and the distinct high words left by partial-word
cross-product moves. The original C1C9AE liveness contract is D5 low,
D6/D7 full, A1-A7 full; CCR is dead and SR control remains checked.

All 8,192 complete adapter fixtures pass that contract and the memory check.
The captured demo call 1044 also passes. The recorded `--glue --recorded`
proof keeps the production liveness table intact and matches 4,075 shadow
and 4,090 sandbox calls, with the same 15 incomplete shadow calls and zero
mismatch/hardware classifications. The generic `check_whole_call_glue.py`
independently reproduces these counts with registered timing steps disabled.

`glue_placement_order_step.c` preserves all 523 parent instructions, including
the preceding C1E53C/C1E53E early exit, and the packed-position/cell helpers.
`glue_workspace_records.c` supplies C1EBC0 and C1EC96 through the existing
workspace shared tails. The six newly timed children are C1EBC0, C1EBE0,
C1EC3A, C1EC96, C1ECD4 and C1ECFC. C1EBB0/C1EC84 and C06C02 already had
source timing. No original opcode handler is called by the handwritten glue,
and no average fee or input/frame shift is substituted.

The nine-entry placement group has 649 distinct source instructions and
matches 20,768 independent DMA fixtures: all registers, full SR, PC, cycles
and Chip/Slow RAM. A fresh full combined oracle also passes 10,622
instructions and 339,904 DMA fixtures after the placement-specific fixture
setup was added. This supersedes the previous 9,627-instruction combined run.

## Integration gates

GNU headless and MSVC Release builds pass. The 600-frame isolated demo probe
is exact. The complete nine-entry isolated live group matches all 36,236
frames (demo 20,833; carrier 12,353; failure 3,050) and each sealed final RAM
hash, using fresh OFF and ON streams from the same executable/model/inputs.

The full 422-entry registered gate passes: 688,103 completed shadow matches,
1,111,316 sandbox matches, zero mismatches, sealed final RAM exact and poison
frames identical. Parent totals are 4,075 shadow matches, 15 incomplete,
and 5,796 sandbox matches. Aggregate call totals change when a complete C
parent absorbs child calls; the isolated whole-call counts above are separate.
Family classifications in the full registered gate are:

| Entry | Shadow matches | Shadow incomplete | Sandbox matches |
| --- | ---: | ---: | ---: |
| C1E540 | 4,075 | 15 | 5,796 |
| C1EBB0 | 0 | 0 | 0 |
| C1EBC0 | 6 | 0 | 0 |
| C1EBE0 | 62 | 0 | 0 |
| C1EC3A | 2 | 0 | 0 |
| C1EC84 | 0 | 0 | 0 |
| C1EC96 | 8 | 0 | 0 |
| C1ECD4 | 33 | 0 | 0 |
| C1ECFC | 2 | 0 | 0 |

All family hardware and mismatch counts are zero. The two cold workspace
helpers retain their independent complete-call structural proof; zero
recorded calls are not new evidence for their whole-call adapters.

```text
python tools/recomp/check_placement_order.py --glue --cases 8192 --recorded
python tools/recomp/check_whole_call_glue.py C1E540
python tools/recomp/check_active_planes_step.py --group placement_order --bus
python tools/recomp/check_active_planes_step.py --group all --bus
```

Use `PORTS_ONLY=C1E540,C1EBB0,C1EBC0,C1EBE0,C1EC3A,C1EC84,C1EC96,C1ECD4,C1ECFC`
with `scripts/recomp_live_check.sh`, followed by `scripts/recomp_ports_check.sh`
without a selector. Logs and reports are under `build/recomp/placement_order_*`
and the small retained summary is
`analysis/figures/native_placement_order_checkpoint.json`.

The fresh combined ALL probe still first differs at one-based RGB frame 416
by 361 pixels through the 420-frame bound. Source coverage has increased;
combined live parity has not. The causal HUD/fade checkpoint in
`native_c_scene_transition_timing.md` remains the next parity evidence.
No invented or placeholder behavior was added. C1D10C remains the complete
selector parent needed for the 423/624 milestone. Stage D -> F -> E remains
unfinished. Temporary frame streams are removed; build/ is 0.228 GiB.
