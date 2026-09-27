# run036 `$C2FF48` area-blit oracle

Classification: **scenario-backed, bounded polygon-to-blitter register
oracle**.  This is an oracle for the native area-path boundary, not proof that
the page can yet be rendered by normal replay.

Authority is the preserved, no-future-input return trace at
`build/run036_7000_c2ff48_submission_trace_no_future/`.  It begins at the
first `$C2FF48` reached in chipset frame 7000 and terminates at its actual
caller return `$C24D66` after 540 instructions.  The input/final Chip images
are external diagnostic artifacts; none belongs in the executable.

## Accepted route and packet

The entry tuple is `(57,130) (97,127) (130,138) (74,145)` in the source
stored coordinate form.  `$C301F6` reduces it to `x=57..130`, `y=127..145`.
With `$C45984=$0090`, the call takes the far continuation
`$C302DE -> $C302EC -> $C302E6`; it does not enter `$C2FA7E`.

At `$C302E6`, before its local decrement/exchange, the trace has:

```text
D0..D3 = 0039 007F 0082 0091
D6     = FFFF0049
A4     = 0090
```

The caller-owned renderer fields in the same pre-call Slow-RAM image are:

```text
C456E2 = 00006048    C456E7 = 0003    C456EA = 0000
C45954 = 0007        C45980 = 006F    C45982 = 0074
C45984 = 0090        C4597C = 010F 013F 006F 0074
```

`$C30306` consumes four adjacent/closing ranges from `$C4B390`; its first
four pairs are `(61,127)->(130,138)`, `(74,145)->(57,130)`, followed by the
closing record sourced with `$C4B392`.  The four `$C30668` entries are:

| trace index | D1 | D2 | D3 | D4 | D5 | D6 | D7 |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 111 | 0053 | FFE6 | 0028 | 0882 | FFA4 | 1B4A | 7454 |
| 194 | 0057 | FFA8 | 0018 | 0C02 | FF38 | 2B4A | 7610 |
| 275 | 0013 | 0016 | 0038 | 0442 | FFF4 | 9B4A | 74C7 |
| 349 | 0057 | FFB8 | 0008 | 0A42 | FF68 | 1B4A | 7454 |

Those are direct source-register images immediately before the exact
`$C30668-$C306B3` leaf.  They validate the existing native
`fa18_prepare_projection_pair_*` register construction.  The leaf's final
`BLTSIZE` store is at `$C306AE`, not its entry address.

## Complete Custom-register oracle

`build/run036_frame7000_instruction_trace/custom_writes.jsonl` covers the
same first bounded call in the full original replay.  Reconstructing channel
state at every `BLTSIZE` trigger identifies its four jobs at stream sequences
552, 568, 584, and 600:

| sequence | trigger | BLTCON0 | BLTCON1 | A | B | C = D | size |
| ---: | --- | --- | --- | --- | --- | --- | --- |
| 552 | `$C306AE` | `$1B4A` | `$0053` | `$00FFE6` | `$00728E` | `$007454` | `$0882` |
| 568 | `$C306AE` | `$2B4A` | `$0057` | `$00FFA8` | `$00728E` | `$007610` | `$0C02` |
| 584 | `$C306AE` | `$9B4A` | `$0013` | `$000016` | `$00728E` | `$0074C7` | `$0442` |
| 600 | `$C306AE` | `$1B4A` | `$0057` | `$00FFB8` | `$00728E` | `$007454` | `$0A42` |

All four have `$FFFF/$FFFF` A masks and `$0028` C/D modulos.  Their B
pointer is inherited from the preceding renderer stage; the complete stream
proves it is `$00728E`, rather than an absent/default channel.  Their source
control words have BLTCON1 bit 0 set: these are **line-mode** submissions,
not plain block copies.

## Native producer cross-check

The external stored pair list is decimal `(97,127) (130,138) (74,145)
(57,130)` (the leading `$0061` word is hexadecimal, not decimal 61).  Passing
that list with the trace-time scalar fields `display=144`, `vertical=145`,
`horizontal=127`, and renderer base `$6048` through the native far-list
dispatcher reproduces all four packets exactly.  The second edge specifically
proved the `$C30634-$C30638` rounding detail: `DIVS` returns quotient 93;
`ASR.W` yields 46 with carry set, then the source increments to 47, producing
`BLTSIZE=$0C02`.  The native bounded-limit helper now preserves that carry
rounding, including the source's DIVS-overflow register behavior.

The packet producer is now ready for the native page diagnostic.  That
diagnostic must still apply the complete inherited line/fill/lane state and
compare its settled Chip-page output, rather than calling a generic triangle
filler.

## Lane-stage consequence

After the four line jobs, the call performs the `$C303D2-$C30404` descending
exclusive-fill job, two `$C30466` lane copies, then `$C304B2`:

```text
C303EC: BLTAPT=BLTDPT=000076D8, BLTCPT=00007454, BLTBPT=0000728E,
         BLTAMOD=BLTBMOD=BLTDMOD=001D, BLTCON0=09F0,
         BLTCON1=000A, BLTSIZE=0486
C30466 #1: BLTAPT=000076D8, BLTBPT=BLTDPT=00014250,
           BLTCON0=0DFC, BLTCON1=0002, BLTSIZE=0486
C30466 #2: BLTAPT=000076D8, BLTBPT=BLTDPT=00016190,
           BLTCON0=0DFC, BLTCON1=0002, BLTSIZE=0486
C304B2:    BLTAPT=BLTBPT=BLTDPT=000076D8,
           BLTCON0=0D0C, BLTCON1=0002, BLTSIZE=0486
```

The settled input/final Chip comparison has 110 changed bytes in 16 runs,
all `$013FF2..$01442B`.  This is a useful end-to-end destination oracle for
the whole bounded call, but it cannot be assigned solely to a `$C30668` job:
the trace contains the subsequent lane-stage blits and the individual DMA
completion boundaries were not captured.

## Port boundary

Do not connect a generic triangle filler to `FA18FlightRendererPage` from
this evidence.  The next native implementation must model the actual OCS
line-mode and descending exclusive-fill semantics, followed by the observed
lane copies, then compare its complete run036 diagnostic page delta to the
110-byte oracle above.  `fa18_execute_ocs_block_blit` is useful only for the
later `$C30466/$C304B2` block jobs; it must reject the line/fill control words
rather than misrendering them.  The live owner that supplies these fields and
selects the active display page is still outside the normal `game.c` path.
