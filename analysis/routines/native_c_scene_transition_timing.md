# Scene transition timing and Copper fade

2026-10-02. Readable coverage remains 421/624. Seven existing replacements
now preserve source instruction/event timing; there are 197 timing-step entries.
This removes one of the two delayed fade frames. ALL still first differs at
one-based RGB frame 416 by 361 pixels.

## Source-backed correction

`port/game/glue/glue_scene_transition_step.c` replaces fixed charges for the
complete $C0FAA4 initializer and its root-setup chain: $C0924A, $C09266,
$C092A0, $C095C0, $C09620 and $C0840E. The three scene-root entry variants
retain their shared tail. The batch has 311 distinct original instructions.
Readable operations remain in `stages.c`, `scene_setup.c` and
`control_records.c`; CPU registers, CCR, memory accesses, calls, loops and
event boundaries remain in glue. No opcode handlers or adjusted average
fees are used.

The $C0FA0E-to-$C0FA12 initialization span previously consumed 32,018 cycles
in ALL, crossing an extra vertical blank. It now matches the original's
16,280 cycles, including the call and children. On the combined path the
initializer is still entered one frame late, but adds no further frame:

| Boundary (machine frame counter) | Source OFF | Before correction ALL | After correction ALL |
| --- | ---: | ---: | ---: |
| C0FA0E: before initialization | 393, line 251 | 394, line 251 | 394, line 251 |
| C0FA12: after initialization | 393, line 287 | 395, line 3 | 394, line 287 |
| C0FA32: fade mode reset | 393, line 287 | 395, line 25 | 394, line 287 |
| C1741A: terminal mode/state write | 436, line 142 | 438, line 142 | 437, line 142 |

All 15 increments still occur three machine frames apart. After correction
the first dim image appears in RGB frame 417 rather than 418; source reveals
it in 416. RGB frame 417 now matches source, as does frame 420. Source's
larger reveal in frame 419 appears in ALL at 420. The before/after images
and JSON are separate, so the original failing checkpoint is retained.
See `analysis/figures/native_frame_416_after_scene_timing.json` and
`native_viewport_fade_after_scene_timing.json`.

## Remaining delay: actual countdown path

The original $C0F5F8 tick decrements $C45AD6 and invokes the $C0FA04 callback.
Fresh 450-frame traces show identical counter values and callback targets.
The first tick is in machine frame 313 on both sides; delay appears before
the second tick, rather than inside the fade callback:

| Tick | Counter before decrement | Source entry frame | ALL entry frame |
| --- | ---: | ---: | ---: |
| 1 | 4 | 313 | 313 |
| 2 | 3 | 342 | 343 |
| 3 | 2 | 359 | 360 |
| 4 | 1 | 377 | 378 |
| 5 | 0 | 393 | 394 |

The enclosing $C0EFD4 update entered in frame 313 has the same 152 original
instruction PCs in both traces. Entry is 108 cycles earlier in ALL, but its
return is 70,498 cycles later, in machine frame 342 rather than 341. Later
loop/input scheduling brings the second tick entry difference to 116,904
cycles. The exact loop selected here is frame 313, after the first tick;
an earlier frame-311 update is not substituted for this causal checkpoint.

HUD return boundaries accumulate the difference. It grows by 11,950 cycles
at $C0F256 after $C3003A, and by 36,896 at $C0F25C after $C328A8. The latter
span also crosses a frame boundary; this is elapsed machine time, not a
measured instruction charge for the child alone. Further readouts add debt.
`native_scene_countdown_checkpoint.json` preserves paired boundary cycles
and the counter/target evidence.

An ALL complement retaining original execution for these 17 HUD entries
resets the fade in machine frame 393 and finishes in 436, matching source:

```text
C3003A,C328A8,C322EE,C30B5C,C30764,C309B6,C3112A,C30F78,
C3201A,C3212A,C31A64,C31ACC,C30A00,C321D2,C32260,C31EB6,C31F4C
```

Its first RGB difference moves to frame 425 (36,650 pixels). Restoring only
the four $C3003A/$C328A8/$C322EE/$C30B5C entries, or only the other 13,
still differs at 416/361. Restoring just $C328A8, $C3003A or $C322EE also
leaves 416/361. These are diagnostic original-code controls, not removal of
registered readable C or proof of complete parity. The group, excluded
entry lists and outcomes are retained in the small checkpoint JSON.

## Validation

GNU and MSVC Release builds pass. The changed-group independent oracle
matches all 311 instructions on 9,952 DMA-contention fixtures: registers,
full SR, PC, cycles and RAM. Combined with previously independently proven
groups, coverage is 10,007 instructions / 320,224 cases. The last fresh full
combined run remains 9,627 / 308,064; no full combined rerun is claimed.

```text
python tools/recomp/check_active_planes_step.py --group scene_transition --bus
```

The seven-entry isolated live group matches source OFF on all 36,236 frames
and sealed final RAM across the three recordings; the 600-frame probe also
matches. Use `PORTS_ONLY=C0FAA4,C0924A,C09266,C092A0,C095C0,C09620,C0840E`
with `scripts/recomp_live_check.sh`.

The full 421-entry shadow/sandbox/sealed-RAM/poison gate passes: 703,337
completed shadow matches, 1,110,694 sandbox matches, zero mismatches, exact
sealed RAM and identical poison frames. Per-entry completed/incomplete
classifications across the three recordings are:

| Entry | Shadow matches | Shadow incomplete | Sandbox matches |
| --- | ---: | ---: | ---: |
| C0924A | 4 | 0 | 2 |
| C09266 | 2 | 1 | 2 |
| C095C0 | 1 | 0 | 0 |
| C09620 | 2 | 1 | 2 |
| C0FAA4, C092A0, C0840E | 0 | 0 | 0 |

Zero recorded comparisons are not passes for those individual whole-call
adapters. The initializer is exercised by the isolated ON transition and
bounded ALL trace; its complete instruction bridge has independent proof.
The unchanged readable bodies retain their preceding evidence.

Reproduce the combined after-checkpoints without overwriting the before ones:

```text
python tools/recomp/trace_viewport_fade.py --output build/recomp/fade_after.json
python scripts/render_recomp_comparison.py --frame 416 --context-frames 4 --output build/recomp/frame416_after.png
```

CSV capture uses `FA18_BOUNDARY_TRACE`, `FA18_BOUNDARY_RANGE` and a 64 MiB
`FA18_BOUNDARY_TRACE_MAX_MIB` cap. Countdown range is C0F5F8-C0F812 for 450
frames; enclosing-update range is C0EFD4-C0F3C4 for 360 frames. Select the
update entered in machine frame 313 when pairing the enclosing rows. Bulk
CSVs and RGB scratch streams are removed after extracting the checkpoint.

This is the second timing-only batch since the planning review. The fade
phase improved, but readable coverage and ALL's first failing frame did not.
The next implementation milestone remains the complete $C1D10C/$C1E540
selector parents. Preserve the demonstrated HUD interaction for the next
parity work; do not resume a queue of unrelated early cycle gaps.
