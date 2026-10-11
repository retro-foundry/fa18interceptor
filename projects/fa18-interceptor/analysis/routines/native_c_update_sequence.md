# Complete game-update, pending-input and display owners

Verified 2026-10-02. C0EFD4, C0F3C4 and C0D730 add three complete readable
owners: **438/624 registered entries, 229 with source timing**. C0F090/C0F132
remain internal labels. The full game-source objective is still incomplete.

## Source and behavior

| Owner | Original range | Instructions | Readable implementation |
| --- | --- | ---: | --- |
| C0EFD4 | C0EFD4–C0F3C3 | 210 | `port/game/update_sequence.c` |
| C0F3C4 | C0F3C4–C0F4A5 | 53 | `port/game/pending_input.c` |
| C0D730 | C0D730–C0D749 | 7 | `submit_update_display_buffers` |

C0EFD4 preserves all 68 original child call sites, the saved entry tick,
stage markers, inactive route, signed flight threshold, map override, indexed
record selection, activity scheduling, context readouts, periodic work and tail.
Child-sensitive globals are reread at their source boundaries; periodic work
uses the saved tick while the final increment reads the live counter.

C0F3C4 preserves its six-byte frame, mouse-button state/mirror writes, saved
recorder mode, both command-word drain loops, full raw-key child values, byte
key argument narrowed to a pushed long, and mode-specific pending-word copies
and clears. There is no artificial production loop limit or new key encoding.

C0D730's bit-$2000 alternative calls C0DA38, which **unlinks the enclosing
update frame**. The enclosing owner must stop before the map/readout/tail work.
The normal CPU adapter recognizes that actual PC/SP exit and performs no extra
RTS. Timing continuations capture the enclosing exit and discard unwound calls.
C0D748's RTS is present in the source and instruction oracle, but C0DA38's
frame exit bypasses it in complete-call fixtures.

The older typed flight slice now uses the original signed `$F8000000` boundary,
rather than testing only INT32_MIN. Its threshold contract passes. The older
split pipeline is explicitly documented as bounded groundwork: cached slice
states and its inactive route do not constitute the complete owner above.

## Independent readable C and cold paths

`python tools/recomp/check_update_sequence.py` passes **8,192 complete calls
per entry, 24,576 total**, using real original child bodies with chipset work
held. Every register/high word, PC, full SR and all Chip/Slow RAM match, with
no RAM/stack exclusions or source-instruction patches.

A separate, explicitly controlled **child-contract oracle** passes 1,024
cases per entry, 3,072 total. It checks every child entry, return boundary,
register, full SR and all RAM before applying the declared child effects.
It covers all 210 update and 53 input instruction boundaries and both display
branches; only the bypassed C0D748 is absent. These contracts are test-only
and are not claimed as real original child execution. Fixtures include signed
thresholds, all scheduling masks, signed/aliased record indices, byte activity
limits, mode changes after capture, two-pass word drains, key press/release,
and children changing the live tick. Direct RAM snapshots replace repeated
bytewise hashing and remain bounded in memory.

An additional **256 original/native timing calls** through the cold display
frame exit match full CPU/RAM and leave **zero saved continuations**.

Normal recorded readable-C proof disables the three timing bridges and keeps
production caller masks:

| Owner | Completed shadow | Completed sandbox | Shadow incomplete | Sandbox hardware |
| --- | ---: | ---: | ---: | ---: |
| C0EFD4 | 11,374 | 0 | 4,638 | 16,658 |
| C0F3C4 | 49 | 0 | 10 | 16,222 |
| C0D730 | 1,660 | 5,798 | 2,354 | 0 |

There are zero mismatches. Parent absorption accounts for the small standalone
input count; it still has completed independent body comparisons. Hardware and
incomplete calls remain classified, not counted as successes.

These owners exposed two proof-scaffold requirements. Cold original-byte
dispatches must resume to the captured entry return/SP before classification.
Display callers need the existing source-first DMACONR inputs; input callers
also need source-first JOY0DAT/JOY1DAT/POTINP inputs. Replay checks each source
PC, address and read order, rejects extra/unconsumed inputs, and supplies no
source output or memory write to C. Unsupported hardware and interrupts retain
their classifications. The whole-call registry keeps these input contracts
when disabling timing bridges.

## Timing and integration

All **270 instructions / 8,640 DMA cases** pass the local instruction oracle.
The fresh combined oracle passes **13,115 instructions / 419,680 DMA cases**,
matching CPU, full SR, PC, cycles and RAM under DMA contention, with sealed-state reset
per case. Handwritten game glue invokes no opcode handlers.

The full 438-entry gate passes **554,286 shadow / 394,909 sandbox** completed
comparisons, zero mismatches, all three sealed RAM hashes and identical poison
frames. Full-set hardware/incomplete totals are retained in the checkpoint.
Aggregate counts change when parent calls absorb children and cold source
dispatches become correctly classified.

The isolated three-entry live group matches fresh source OFF RGB444 output
on all **36,236 frames**, with every final RAM seal exact. GNU headless and
MSVC Release builds pass. The 600-frame probe is exact for this group; **ALL
remains 416/361**, and the user-deferred Copper fade is still open. Temporary
RGB streams are removed; build/ is approximately **0.364 GiB**.

Machine-readable evidence: `analysis/figures/native_update_sequence_checkpoint.json`.
Next inspect the complete external-event, keyboard-source/raw-poll and changed
button owners C16EAE, C16BF2, C16C56 and C13D34 as a related readable batch.
