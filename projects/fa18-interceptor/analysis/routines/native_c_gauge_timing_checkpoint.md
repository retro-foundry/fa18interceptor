# Gauge timing checkpoint ($C30918)

2026-10-02. Readable coverage remains 419/624. This corrects an existing
renderer replacement; it does not add a translated function or resolve the
combined first visible difference.

## Source and visible reproducer

The original 35 instructions implement the cached 22-row gauge in
`port/game/view_marks.c`. The cache/refresh path can return without drawing;
the drawing path calls the already timed $C2F60A pixel pair twice per row.
The former replacement charged 15,000 instruction cycles for every call.

Fresh 420-frame demo runs with the preceding executable show that $C30918
alone and ALL produce identical incorrect frame-416 bytes, not merely the
same count of 361 changed pixels. The changed region spans x=7..318 and
y=101..199: 168 pixels change RGB444 111 to 000 and 193 change 100 to 000.
This broad region is consistent with display phase/cadence, rather than a
gauge-shaped output error. The checkpoint hashes are:

| Artifact | SHA-256 |
| --- | --- |
| Source frame 416 | de4e8b22018529118b2a4125b19a5a92b9ae30489a4dbcdf430f720e1d5d2c89 |
| Both replacement frame 416 outputs | 6cdd259c8ecbe61fbc369f3293c1961541386954a223b17a37899d7fd9ad42da |
| Changed pixel signature | 77300957d56e26f4445d99656def106ed7381d70d640bfbd2ef6f0df3a0fe5b9 |

The complete source and replacement frame hashes independently identify the
reproducing bytes. Small frame excerpts and `gauge_checkpoint_before.json` remain
under ignored `build/recomp/`; the bulk RGB scratch streams are removed.

The original parent $C0F242 JSR / $C0F248 continuation show actual source
spans, including JSR and return, of 148/142/142 cycles on the first three
skipped calls and 50,310/49,590 on the first two drawing calls. The old
replacement instead charges roughly 15,000 on both paths and moves later
calls to different machine frames. The first drawing call plots into Chip
addresses $18324..$1866E; the next into $53294..$535DE, using the alternate
DRAW_PAGE ownership. No source colour or page rule was changed.

## Correction and proof

`port/game/glue/glue_gauge_step.c` preserves the original cache branches,
clamps, 22 iterations, MOVEM.W sign extension, child calls, stack, CCR and
memory-access/event boundaries. The readable `view_marks.c` body is retained.
No average fee or interpreter opcode handler is used in the bridge.

The independent DMA oracle passes all 35 instructions on 1,120 cases,
matching registers, full SR, PC, cycles and Chip/Slow RAM. Together with the
preceding independently proven groups this covers 9,662 instructions and
309,184 cases; the full combined oracle was not rerun for this local bridge.
GNU and MSVC Release builds pass. Reproduce the new instruction proof with:

```text
python tools/recomp/check_active_planes_step.py --group gauge --bus
```

The isolated $C30918 live replacement matches fresh source OFF RGB444 on all
36,236 frames and each recording's sealed final RAM. The bounded parent
trace $C0F230-$C0F254 matches all 49 rows and every field through frame 420,
including both skip and drawing returns. The isolated short probe is exact
through frame 600. Use `PORTS_ONLY=C30918` with `scripts/recomp_live_check.sh`.

The full 419-entry shadow/sandbox gate passes: 703,337 completed shadow
matches and 1,110,694 sandbox matches, zero mismatches, exact sealed final
RAM and identical poison frames. The gauge itself has 791 completed shadow
matches, 150 incomplete shadow calls and 627 completed sandbox matches.
Incomplete calls are retained as a separate classification, not counted as
matches. There are now 188 registered timing-step entries.

ALL still first differs at one-based frame 416 by 361 pixels after the
correction. The planning subset/complement controls already demonstrated
other interacting defects. The exact isolated replay proves this gauge
correction; it does not establish that the remaining combined error has the
same writer, or that our machine timing matches independent UAE hardware.
The next work returns to the complete selector family in the handoff.
