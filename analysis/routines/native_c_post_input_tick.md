# Complete post-input tick parent

2026-10-02. `port/game/post_input_tick.c` implements all 118 instructions
of $C0F5F8-$C0F810 as readable game semantics: offset accounting, phase
selection, counters, stage dispatch and final command clear. Its CPU adapter
and source timing bridge are separate files under `port/game/glue/`.

## Source contract

The signed attempts/mode guards may skip the prefix; the tail always runs.
The offset path requires a nonzero phase, inactive recorder, negative sequence
step and nonzero primary offset. Each long subtraction wraps independently.
Optional secondary/extra differences subtract from the stored local; the
signed stored result must be in [0, hexadecimal $4650). Valid offsets add to
the target's +8 long and mark it changed. Invalid offsets publish $3F and
invoke the original returning fault hook. Both routes clear the primary
offset before phase selection.

For a nonnegative player phase, sequence phases $FF, 1 and 2 install the
original $C0F920, $C0F946 and $C1104C callbacks, subject to their exact
guards and partial writes. For a negative player phase, sequence phase 3
clears the sequence flag even if the signed step blocks its remaining writes;
otherwise it copies the configured countdown and installs $C11078.
The phase-two flag clear likewise occurs before its signed-step guard.

The tail decrements only a nonnegative step byte, increments the clock byte,
decrements the countdown word and calls the installed stage once. The call
is at **$C0F806**, with return **$C0F808**. The parent clears $C457A3 after
the child returns, preserving every other change made by that child.

Stage consumers remain required independent owners. In particular $C08F26
and the cold $C1104C callback are not claimed as newly translated game C.
The existing runtime can execute original bytes between generated entries;
the shared whole-call bridge now uses that path for installed callbacks with
no generated entry. No handwritten glue invokes opcode handlers or patches
original instruction bytes.

## CPU contract and independent proof

The adapter preserves the source's LINK -4 frame for children and unwinds
through the current A6. Semantic observations publish wrapped offset values,
the optional D1 difference, frame-local writes and phase MOVEQ results.
The final word decrement preserves D0's high word and supplies the child's
exact N/Z/V/C/X input. After the child, CLR.B publishes the final logical
flags while retaining its X. Production caller liveness remains all sixteen
registers and all eight data high words at $C0EFEA; exit flags are dead.

`python tools/recomp/check_post_input_tick.py --cases 16384` independently
executes the original parent and real source callbacks. Every register,
high word, PC, **full SR and all Chip/Slow RAM** match in all 16,384 cases.
There are no stack exclusions or relaxed return masks. Each side starts from
the same sealed machine and CPU state; source instruction bytes are unchanged.

Cases cover every phase-selection route, blocked partial-write routes,
positive/negative/zero entry gates, stopped and wrapping step bytes, byte
clock wrap, signed countdown edges, configured reloads, optional offset
terms, signed long wrap and the 4,650/5,000/17,999/18,000 boundaries. Original
callbacks may change the countdown, flags, callback pointer and scene state;
the domain does not flush a stale state snapshot over those changes.

`python tools/recomp/check_whole_call_glue.py C0F5F8` separately disables
only this parent's timing bridge in a temporary registry. Normal readable
C matches **15,929 shadow and 16,001 sandbox** completed calls across three
sealed recordings, with zero mismatches. Shadow retains four hardware calls
and 79 incomplete calls; sandbox retains ten incomplete calls. These are
not accepted as completed comparisons.

## Source timing

`python tools/recomp/check_active_planes_step.py --group post_input_tick --bus`
passes all **118 instructions / 3,776 DMA cases**, including every incoming
CCR combination, bus accesses, PC, cycles, registers, full SR and RAM.
The bridge retains source branches, PC-relative LEAs, LINK/UNLK, memory-long
read/modify/write and the indirect child boundary.

The fresh 600-frame demo probe matches source OFF with this entry isolated.
Combined ALL still first differs at **frame 416 by 361 pixels**. The deferred
Copper fade remains a separate checkpoint; its timing has not been declared
resolved or excluded from normal proof gates.

## Integrated checkpoint

The full **428-entry** gate passes **644,155 shadow / 944,089 sandbox**
completed comparisons, zero mismatches, all three sealed final RAM hashes
and identical poison frames. Its aggregate hardware/incomplete classifications
are 16,000/33,252 shadow and 17,010/413,673 sandbox. Ported parents absorb
previously counted child calls, so call totals are not a monotonic coverage
measure.

The full isolated live $C0F5F8 replay matches every source OFF RGB444 frame
and sealed final RAM on all **36,236 frames**: demo01 20,833, carrier success
12,353 and qualification failure 3,050. This is an exercised entry in all
three recordings. Source OFF is generated freshly on the same machine model;
this is not an independent UAE timing claim.

The fresh combined DMA oracle passes **11,853 instructions / 379,296 cases**
after restoring sealed machine state before each fixture and opcode read.
There are now **213 timing-step entries**. GNU headless and MSVC Release
builds pass. The next source-owned batch is the complete C1C860 context-refresh
parent and C08F26 bootstrap, followed by its C0F920/C0F992 callback wrappers.
C08F26's full source has 80 instructions, including the three final parent
calls after C090AE; its older initialization slice ending at C090AD is not
the complete routine.
