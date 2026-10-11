# Complete context refresh and scene bootstrap

2026-10-02. Four complete original entries are now readable game C:
$C1C860 context refresh (92 instructions, including its early-return RTS),
$C08F26 bootstrap (80), $C0F920 reset callback (8) and $C0F992 follow-up
callback (23). The domain modules are `port/game/context_refresh.c` and
`scene_bootstrap.c`; all CPU, source-frame and timing machinery stays in glue.
Internal labels and shared return addresses are not additional functions.

## Context parent

The signed long guard compares against $F8000000. An admitted call saves
D0-D5/A0, writes its caller's frame-gate byte and samples the request mask.
Nonzero requests publish trace $49 and invoke the existing readable
record-flagging owner. **C1CA82 preserves D0**: the tested low nibble is the
original request byte, not a newly decoded classification result. Non-$B/$C/$F
kinds publish $27 through the returning fault hook and still continue.

The selected control record supplies signed word coordinates shifted by two.
The alternate route masks both origin longs with $1FFFFFFF, swaps their
halves and shifts the resulting low words by **eight**. Publish both condition
keys before changing preparation/check flags. Consume bit zero from the
original request sample; reread RAM for bits one, two and three after each
template child. Preserve the three distinct template-call return addresses
and partial flag writes. Source trace/frame-gate changes remain ordered.

Next run display sorting, placement-cache ordering and the selected condition
scan, then restore exactly the source-saved registers from RAM. Children retain
D6/D7 and the other address-register outputs. Preserve the real save stack and
caller A6 because the sorting child can read an aliased saved-register byte at
-$2C(A6). The guarded pixel-block submission uses word coordinates $72/$A0,
colour 15 with the prepared line style, or colour 6 otherwise. Clear the
prepared byte only after that child. The rejected entry guard changes only
the comparison flags and return state.

## Bootstrap and callback parents

Bootstrap retains all ten ordered children, including the final gate-builder,
flight-update and context-refresh calls at $C090AE/$C090B4/$C090BA. Its older
initialization slice ending at $C090AD was not the complete routine.
Direct initialization preserves all original constants, 41-long control-slot
clears and 8-long workspace-slot clears, untouched control padding, observer
setup, ordinal table, 48-byte zero-character fill and subsequent 28-byte space
fill. Child-entry register widths and flags are separately reproduced.

The reset wrapper calls bootstrap, clears its three sequence/mode bytes and
installs $C0FBE0. The follow-up wrapper frees voices, calls bootstrap, preserves
the mode-two remap and chooses the recorder-mode-three route to $C0FA04 or
the original menu-table child and $C0FCB4. Stage callbacks remain explicit
owners; no scene shortcut, fabricated renderer or convenience initialization
path is introduced.

## Independent readable-C proof

`python tools/recomp/check_scene_bootstrap.py --cases 8192` passes **8,192
complete original-byte calls per entry: 32,768 total**. Every register/high
word, PC, **full SR and all Chip/Slow RAM** match. There are no stack, RAM
or liveness exclusions and no patched source instruction bytes.

Fixtures cover all request-byte values, accepted/rejected guard boundaries,
both selector routes, low-word shift/carry bits, both condition scans, render
guards and colours, aliased/unaliased caller frames, bootstrap's clear extents
and overlapping text fills, mode remaps and recorder modes 0/1/2/3/4/$7F/$80/$FF.
The real original child bodies run on both sides; the domain does not execute
a memory-writing child twice to recover CPU outputs.

Normal recorded readable-C proof retains production caller masks. Context
refresh matches **3,743 shadow / 5,810 sandbox** completed calls; 347 shadow
calls remain incomplete. The reset and follow-up wrappers each match one
sandbox call; three wrapper shadow calls are incomplete. No hardware calls
or mismatches occur in these selected reports.

When all four are selected together, enclosing wrappers absorb bootstrap's
sandbox comparison; its two batch shadow calls remain incomplete. An
independent **C08F26-only** registry proves its own readable body with one
completed sandbox comparison in each recording, **three total**, while all
three shadow calls remain incomplete. These are not cold entries or accepted
incomplete calls.

`check_whole_call_glue.py` now performs that isolation automatically for a
called entry lacking a completed batch comparison. It retains separate raw
reports, checks every original mismatch and still requires a completed
comparison per entry. A cold/unproven entry still fails. This removes the
manual failed-batch/isolate-child cycle for subsequent related parent packs.

The older typed `port/context_refresh_packet` groundwork also now uses source
record flagging, the eight-bit origin shift, later-request rereads and a captured
condition route for the post-child trace. Its focused GNU -O2/-Wall/-Wextra
contract checks include child-consumed later requests and a child-changing
condition selector. It is not counted as a second registered function.

## Source timing and integration

Six already registered children now have source timing: C090C2, C090F2,
C0910C, C0915A, C0F4A6 and C11ACC. Together with the four new parents, all
**272 instructions / 8,704 DMA cases** match registers, PC, full SR, cycles,
RAM and bus timing. LINK/UNLK, MOVEM, partial widths, bit operations, memory
read/modify/write, PC-relative LEAs, child arguments and original boundaries
remain in the instruction bridge. The combined oracle passes **12,125
instructions / 388,000 DMA cases**, resetting the sealed machine per fixture.

The full **432-entry** gate passes **636,016 shadow / 930,150 sandbox**
completed comparisons, zero mismatches, all three sealed final RAM hashes
and identical poison frames. Parent absorption changes aggregate call totals.
There are now **223 timing-step entries**. GNU headless and MSVC Release pass.
All ten entries together match every isolated live source OFF RGB444 frame
and sealed final RAM on all **36,236 frames**: demo 20,833, successful carrier
landing 12,353 and qualification failure 3,050. This exercises the complete
bootstrap on all three recordings and the two wrappers on their recorded
routes; independent structural proof supplies the other branch coverage.
Build output after cleanup is **0.317 GiB**.
The 600-frame isolated demo probe is exact; ALL still first differs at
**frame 416 / 361 pixels**. Copper-fade work remains deferred and the normal
automated comparisons are unchanged.

Next complete the related C22C80 record-update and C29042 active-origin
parents, then their C1C63E update-stage owner (226 + 153 + 112 source
instructions). C08F26 still calls the original C1C63E body. The latter also
belongs to C0EFD4's update sequence; its C0F090/C0F132 addresses are internal
labels, not separate original routines to count.
