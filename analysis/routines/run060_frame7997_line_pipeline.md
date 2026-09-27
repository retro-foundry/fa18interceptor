# run060 frame 7997 line pipeline

The first four frame 7997 submissions use the line family captured in
`run060_dma_frame7997_wide.json`. The first family uses `BLTCON0=$EBFA`
(A/B/C enabled, D disabled), `BLTCON1=$55`, `BLTAMOD=-24`, `BLTBMOD=0`, and
`BLTCMOD=BLTDMOD=5`.

## Inherited B input

The B pointer writes immediately before the first family are:

| submission | BLTBPT | frame 7996 word |
|---:|---:|---:|
| 0 | `$00B038` | `$0000` |
| 1 | `$00AE88` | `$0000` |
| 2 | `$00ACD8` | `$0000` |
| 3 | `$00AB28` | `$0000` |

The four words come from the frame 7996 Chip snapshot. Since `BLTBMOD=0`,
the B input remains zero for every line step. This is distinct from the
inherited `BLTBDAT=$FFFF` register value: line mode reads B from the supplied
pointer when the B channel is enabled.

## C/D pipeline ordering

The DMA records alternate C and D events at one repeated destination. The
first C event is `$FFFF`, while the first D result is `$FFFD`; subsequent D
values follow the next C event. The first D therefore consumes the C value
already present in the line pipeline when the new C read is recorded. The
terminal D value is also generated after the final line stage, so it cannot be
modeled as a simple one element shift of the captured C array.

Engine9000's `actually_do_blit()` explains the ordering: with `hblitsize=2`,
each iteration reads B and C, computes the first minterm, advances the line
state, writes the pending D value, and then computes the terminal minterm that
feeds the following iteration. The first D is therefore the submission's
pending line value at start, followed by values produced by the preceding
iteration's terminal stage.

The native packet keeps the captured D stream as an explicit oracle until the
cycle ordering is implemented. It carries the B source and pipeline state as
semantic fields and does not recreate the Amiga address space.
