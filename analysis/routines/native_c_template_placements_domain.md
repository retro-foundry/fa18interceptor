# Complete template-placement domain: C1D10C

2026-10-02. `port/game/template_placements.c` recreates the complete memory
behavior of the 648-instruction $C1D10C parent. The domain is independently
proven but **not registered**: readable coverage remains 422/624, with 204
registered timing-step entries. Its normal CPU adapter and instruction/event
timing are the next implementation work. This is not an integration or live
frame-parity claim.

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
liveness table remain unchanged. The normal masks retain all sixteen
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

## Next integration work

Publish semantic expansion, placement and descriptor-copy outputs for the
normal CPU adapter, preserving all live register widths and N/V/C without
repeating child writes. Add the complete parent timing bridge and replace
$C1D722's fixed charge with its 33 source instructions; the other static
children already have timing steps. Prove the adapter separately with original
liveness, then run instruction/DMA, isolated live RGB/sealed RAM, and full
shadow/sandbox/sealed-RAM/poison gates before registering entry 423.

After that milestone, return to the causal frame-313 HUD/countdown checkpoint.
The Copper fade remains one frame late (reset 394 versus 393, terminal write
437 versus 436); this domain-only work does not change the registered path.
