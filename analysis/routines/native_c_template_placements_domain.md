# Complete template-placement parent: C1D10C

2026-10-02. `port/game/template_placements.c` recreates the complete memory
behavior of the 648-instruction $C1D10C parent. Its complete normal CPU adapter
and resumable source timing are now independently proven and registered:
readable coverage is 423/624, with 206 registered timing-step entries.
The earlier domain-only checkpoint remains historical evidence, separately
reproducible without claiming CPU or live timing parity.

## Complete source contract

The parent includes the selector prefix and the placement builder at $C1DC1C,
the reverse descriptor-packet walk, and the final control-record list through
$C1E326. $C1DC1C is an internal label, not another completed routine.

- Select the route, normal or derived directory and the X, Y or append pack.
  Preserve the selector guard, both term pairs, signed byte maps, origin and
  translation tables, cache/context pointers and retain-context flag.
- Fill all 224 cell markers, resolve the positive signed-relative control
  stream, and expand up to fourteen bands through `expand_cell_templates`.
  Preserve the original skip/count bytes and coordinate wrapping branches.
- Build complete 24-byte placements, descriptor pointers, scaled coordinates,
  magnitude-selected shifts, rolling ordinal/cycle fields and group records.
  Preserve control/workspace flag paths and their partial memory writes.
- Preserve the cache limit's actual behavior: the count continues increasing
  while the last slot is repeatedly overwritten; the final marker overwrites
  that slot. Do not replace this with an early emission limit.
- Walk append placements in reverse, resolving descriptor flag formats,
  copying translated bounds and vertices, and replacing positive section
  links with output pointers. Preserve the eleven accepted-descriptor limit,
  signed byte visit count and emitted-count comparison.
- Publish the active control-record list only on the original append/nonretain
  path, including its negative terminators and untouched trailing bytes.

The domain has no CPU registers, CCR, instruction handlers or timing fees.
`control_records.c` and `render_state.c` supply the already proven complete
template expansion and column-fill children.

## Source distinctions retained

The existing typed selector/placement groundwork is useful context, but its
bounded contracts cannot replace the whole parent without source review.
The complete source contains several consequential distinctions:

- The route cursor maps its first result through $C411A0 again at $C1D302.
  Derived selection uses selector A for the first map and selector B for the
  group-helper lookup, even when the selected context differs.
- The placement builder always uses the B term pair for packed cell data.
  `MOVEM.W` origins/translations sign-extend; ordinary payload words remain
  unsigned and their initial fourfold scaling wraps at word width.
- The component base is $C1D7E2 unless bit 4 selects a record. Bit 6 alone
  leaves that base intact. The unflagged height path unconditionally negates
  the projection height. ADD followed by a signed branch sees signed overflow
  differently from a later comparison of the stored wrapped value.
- The packed cell high word changes after each emitted tail. Ordinary tails
  retain that history. Cycle bytes decrement and reset to three when negative.
- Descriptor packets stride **256 bytes**: $C1E228/$C1E22A multiply the slot
  by four before $C1E236 shifts it six more bits. Source review corrected the
  first draft's 64-byte stride before the recorded gate.
- The packet vertex loop preserves the source's zero-count DBRA underflow
  behavior. Structural packet fixtures use positive counts and exercise
  linked and nonpositive terminal sections; zero-count packets are not an
  independently proven fixture path.

## Independent domain evidence

`tools/recomp/check_template_placements.py --recorded` builds an isolated
single-entry registry. Its temporary caller masks retain A6/A7 only, so it
proves domain memory, not CPU results. Production `ports.c` and the generated
liveness table are not relaxed by that proof. The normal masks retain all sixteen
registers and all eight data-register high words at $C1C924/$C1C94A/$C1C982;
$C1C924 also retains N/V/C.

| Recording | Completed shadow memory matches | Shadow incomplete | Sandbox memory matches |
| --- | ---: | ---: | ---: |
| demo01 | 343 | 457 | 800 |
| qual_carrier_success | 134 | 70 | 219 |
| qual_fail_crashes | 15 | 12 | 27 |
| Total | 492 | 539 | 1,046 |

All comparisons have zero mismatches and zero hardware classifications.
Incomplete calls are not counted as passes. Sandbox and shadow call counts
can differ; neither total supplies an instruction/event timing proof.

`template_placements_oracle.c` separately compares original execution with
the domain on all Chip and Slow RAM bytes, excluding only a bounded 128-byte
private stack region. The deepest returning child path uses 124 bytes:
return word, saved A6 and 60 locals, saved control cursor, child return,
40-byte MOVEM save, nested return and four-byte D6 save. This does not use the
runner's broader dead-stack exclusion or any caller liveness masks.

All 8,192 complete fixtures pass. Case zero replays a captured original
demo entry. Later fixtures install terminated source-format directories,
sorted columns, bitmaps, template streams, records and descriptor sections;
original instructions independently compute the expected output. They cover
all incoming CCR combinations, randomized register values, zero/negative
selector bytes, empty control streams, one/two/fourteen bands, all three
packs, retain-context variants, word wrapping and extreme projection heights.
Measured source outcomes include 1,419 empty caches, 2,743 cache-limit cases,
1,941 actual linked packet copies and 621 recoverable shift errors.
Captured carrier/failure first entries also independently replay and pass.

GNU headless and MSVC Release builds pass. Small reports and captured entries
are retained; these checks create no bulk RGB streams. Unchanged proof
registry/mask text reuses cached objects. The compact checkpoint is
`analysis/figures/native_template_placements_domain_checkpoint.json`.

```text
python tools/recomp/check_template_placements.py --recorded --frames 600 --capture-call 1
python tools/recomp/check_template_placements.py --fixture build/recomp/template_demo01_shadow_call1.bin --cases 8192
python tools/recomp/check_template_placements.py --recorded
```

## Normal CPU adapter and source timing

Synchronous domain observations publish actual expansion, placement and
descriptor-copy results. `glue_template_placements.c` reconstructs the live
register widths and N/V/C without repeating children or memory writes.
The shared cell-output preparation is read-only; filing explicitly reports
whether it selected a level offset so an unchanged high word is preserved.
All CPU, CCR and source-instruction effects remain in `port/game/glue`.

Both `check_template_placements.py --recorded --glue` and the generic
`check_whole_call_glue.py C1D10C` retain the original caller masks and match
492 shadow / 1,046 sandbox calls across all three recordings. Per-recording
counts are identical to the domain table above. All 539 incomplete shadow
calls retain that classification; there are zero mismatches/hardware cases.
The structural adapter oracle independently matches all 8,192 fixtures,
checking all sixteen registers (including all high words and A6/A7), PC,
SR control bits and N/V/C, plus RAM outside only the 128-byte private stack.
The liveness mask `1A` encodes N/V/C in the runner's X,N,Z,V,C order;
the corresponding actual SR-bit comparison mask is `0B`.

`glue_template_placements_step.c` implements the complete bounded parent
and replaces $C1D722's fixed charge with its 33 source instructions. The
parent's broad address span encloses existing cell-expansion source islands;
those PCs delegate to the proven $C1D3F4 bridge before fetching instructions.
The 681 local source instructions pass 21,792 DMA-contention cases comparing
registers, full SR, PC, cycles and RAM. A fresh combined oracle passes
11,303 instructions / 361,696 cases. It exposed a shared ASL edge case:
shifting an all-ones operand by its full word/long width must set overflow.
Both shared math helpers now preserve the original CPU behavior.

The full 423-entry registered gate passes 669,031 completed shadow and
1,075,296 sandbox comparisons, zero mismatches, sealed final RAM exact and
poison frames identical. Aggregate calls decrease when the parent absorbs
child calls. The registered parent itself has 492 shadow / 1,067 sandbox
matches, zero mismatches/hardware and 539 incomplete shadow calls; the full
registry can change call counts relative to its single-entry control.
GNU headless and MSVC Release builds pass. Build/ is 0.271 GiB after replay
cleanup; no bulk RGB scratch streams remain.

Reproduction commands for the integration proof:

```text
python tools/recomp/check_template_placements.py --recorded --glue --capture-call 9
python tools/recomp/check_template_placements.py --glue --fixture build/recomp/template_demo01_shadow_call1.bin --cases 8192
python tools/recomp/check_whole_call_glue.py C1D10C
python tools/recomp/check_active_planes_step.py --group template_placements --bus
python tools/recomp/check_active_planes_step.py --group all --bus
```

The isolated seven-entry live group matches all 36,236 frames and sealed
final RAM (demo01 20,833; carrier success 12,353; qualification failure 3,050).
The bounded group also matches through demo frame 600;
ALL still first differs at frame 416 by 361 pixels. This advances readable
coverage without claiming a combined parity improvement. The full live gate
uses `PORTS_ONLY=C1D10C,C1D722,C1D3F4,C1D4E4,C1D520,C1D5D8,C06C02`.
The integration checkpoint is
`analysis/figures/native_template_placements_checkpoint.json`.

The user deferred the minor Copper fade on 2026-10-02. Preserve its source/ALL
reset 393/394 and terminal 436/437 comparisons and the causal frame-313
HUD/countdown evidence for later; continue complete readable parent batches.
This deferral does not change automated parity comparisons.
