# Complete menu setup and input/message helpers (2026-10-03)

`port/game/menu_setup.c` recreates the complete C0FBE0 top-level menu owner,
C17B96 sound selector, C1082C countdown tail, C11BB0 cockpit message filter
and C24FA4 indexed queue helper. Normal CPU adapters and resumable source
timing are separate glue. Four entries are newly registered; C17B96 replaces
its older fixed-charge adapter and shares its domain with the typed audio API.
Coverage is 469/624 registered translated entries, with 262 source-timed entries.

## Original scope and behavior

`python tools/recomp/audit_menu_setup.py` seals 141 unique source boundaries
with no shared instructions or missing static paths. C0FBE0 retains its LINK -4
queue cursor, volume-target/fading gates and actual sound, clearing, reset,
delay and script children. The C0E78A busy wait and library call remain actual
source behavior. PEA C08490 preserves flags at the script child boundary.
The message queue is exactly 6, 100 through 109, then zero, followed by the
C0FCB4 callback publication.

C17B96 retains the early fading gate, bit-seven fixed 13/14 sound pair at
volume 63, bit-two argument-volume 35/36 pair, both free-voice routes and
the final fading state. Every child receives its original stack arguments.
C1082C publishes the auxiliary flag, countdown 3 and C0FB70 callback, then
unlinks the existing enclosing frame without creating another LINK frame.
C11BB0 preserves the bit-eight gate, original message publication, held flag,
argument-word mask and 4000/4800 message-state-bit clear. C24FA4 uses the
original signed word directory and row offsets, including word subtraction,
doubling and wrap, before storing the code and terminator in source order.
High register halves and partial byte/word writes remain observable CPU state.

## Independent readable-C proof

`python tools/recomp/check_menu_setup.py` runs complete original-byte execution
against independent normal C in two separate layers:

- Five entries with actual original children, 16,384 fixtures each: 81,920 cases.
- Five entries with controlled child-entry CPU/RAM contracts, 8,192 fixtures
  each: 40,960 cases. Children return changed registers and CCR in test builds.

Each layer independently covers every one of the 141 owned source boundaries.
All 122,880 fixtures compare every register and high half, PC, full SR and
all Chip/Slow RAM, including stack bytes, with no exclusions. Fixtures retain
the original enclosing frame for C1082C and exercise all CCR combinations,
sound/fading gates, cockpit codes and signed word argument boundaries.

The structural executable uses `tools/recomp/structural_write_log.c` to reset
completed fixture bookkeeping only while logging is inactive and ports are OFF.
Without this reset, repeated original busy-loop counter writes accumulated in
the global log until allocation failed around fixture 7,680. The production
runtime is included unchanged; each fixture still executes the complete actual
child and compares all RAM. Neither source delays nor comparison scope were
shortened. The original diagnostic remains an ignored build log.

Recorded normal-C proof disables selected timing steps and retains real caller
liveness. Independently isolated C0FBE0 and C17B96 each complete one sandbox
comparison in qualification failure: zero shadow / two sandbox comparisons
in total, with two retained incomplete shadow calls and no hardware or mismatch
classifications. C1082C/C11BB0/C24FA4 are cold across all recordings; their
generic `C1082C: no completed comparisons` rejection and raw zero reports
remain retained. The initial five-entry batch also rejects C17B96 because
the native C0FBE0 parent absorbs its child statistics. The standalone selector
proof establishes its sandbox evidence; it must not be classified as cold.
Cold structural proof and live nonregression remain distinct evidence.

## Timing and integration

`python tools/recomp/check_active_planes_step.py --group menu_setup --bus`
passes 141 instructions / 4,512 DMA cases, comparing CPU, PC, full SR, cycles
and RAM. Source PEA timing is emitted explicitly with unchanged flags. Shared
runtime CPU/bus/math and instruction-oracle fixture setup did not change.
The independently verified instruction union is 15,104 / 483,328 cases;
there was no fresh combined run for this batch. The last fresh combined run
remains the preceding menu-transition checkpoint, 14,963 / 478,816.

All 36,236 isolated live RGB444 frames and final RAM seals match fresh source
OFF output. The full 469-entry gate passes 554,025 shadow / 413,303 sandbox
calls, zero mismatches, exact final seals and identical poison frames. GNU and
MSVC Release builds pass; build artifacts occupy 0.575 GiB. The 600-frame
isolated probe is exact. ALL remains at first difference 416 / 361 pixels;
the user-deferred Copper/HUD work stays deferred. OFF/ON comparisons share
the machine model and do not establish independent Amiga timing parity.
Hashes and raw classifications are sealed in
`analysis/figures/native_menu_setup_checkpoint.json`.

## Remaining original owners

`python tools/recomp/audit_menu_source_only.py` seals a separate inventory,
`analysis/data/menu_cold_scope_inventory.json`: 191 unique / 14 shared
boundaries across ten source-only owners and one translated required helper.
The source-only entries are C0FE36/C1017E/C10272/C103E4/C09120/C29490/C2949A/
C09148/C10B90/C16406. C1643A is the translated 59-instruction file-loading
helper, not a source-only entry. This inventory implements none of them.
Their installed callbacks, shared tails, actual children and source file/OS
contracts require reconstruction; preserve the existing OS children while
game C is completed. The current lookup initially maps translated function
entries, so source-only activation requires explicit reconciliation and proof.
Track these owners separately from the seeded 624-entry denominator.
Continue Stage D game C, then Stage F plain native backend and only necessary
Stage E services. Whole-game completion must include the original indirect
callback/call graph beyond the seeded translation.
