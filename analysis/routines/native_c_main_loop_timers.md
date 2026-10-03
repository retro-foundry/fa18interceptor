# Complete main-loop timers, readout and setup bounds

Updated 2026-10-03. C2527C/C25312 are newly registered; C2548A's older fixed-charge
adapter is replaced with its complete CPU/SR domain adapter and source timing.
The registry has 481/624 translated entries plus 66 original source-only entries:
547 rows and 362 source-timed entries (296 translated plus 66 source-only).
C1612C remains inactive with its prior frozen graphics-wait failure retained.
Original game-function porting remains incomplete.

Authority is the original sealed state in `captures/native/demo01/state.bin`,
audited in `analysis/data/main_loop_timers_source_scope.json`. Three complete
owners contain 157 unique instruction boundaries, no shared boundaries and six
actual child sites. Original call sites establish all three callable entries.
C2527C's earlier RTS at C2527A belongs to its source flow and is included in
both ownership and the registry's explicit start address.

| Owner | Complete behavior |
| --- | --- |
| C2527C | Iterate original table offsets and 20-byte bounds records, retain the -1 sentinel and negative-offset exit, normalize the width/height vector through C2574A, then publish six transformed endpoint words using original coefficients and word wrapping. |
| C25312 | Sample the original clock child, accumulate word/long elapsed counters, retain flag resets/notifications and countdown children, update the signed level byte, then poll the original clock against the indexed threshold with its original backward branch. |
| C2548A | Preserve signed validity/limit tests and DIVU word quotient/overflow rules, update signed min/max only on the division path, publish the readout and reload both sample longs. |

Readable behavior is in `port/game/main_loop_timers.c`; CPU outputs and actual
service calls are in `port/game/glue/glue_main_loop_timers.c`. The public
`update_readout()` uses the same domain, so the old implementation is removed.
The original 9999 sentinel skips min/max updates. The earlier implementation
updated them on that path; complete source fixtures exposed the difference.
All source coefficients, signed comparisons, partial register writes, full SR,
global reloads, child outputs and memory order are retained. No clock advance,
timer fee, guessed child result or synthetic service completion is introduced.

Complete structural proof passes 73,728 calls without register/RAM exclusions:
8,192 controlled-child cases and 16,384 actual-original-child cases per owner.
The controlled layer covers all 157 instruction boundaries and compares every
child-entry CPU/SR/RAM snapshot. Contracts mutate clock fractions and timer
inputs, scramble caller registers, change the bounds-record pointer and exercise
the original polling backward branch. The fixture's synthetic clock updates
are test contracts only; production always calls the original clock child.

Real-child proof covers all 52 bounds and 19 readout PCs. The timer fixture
retains the original negative poll guard and covers 38/86 timer PCs, including
its actual clock and countdown children. This is partial real timer coverage;
its elapsed/poll branches are covered by the separate controlled layer. No
completed real-service polling-loop proof is claimed.

A zero readout divisor raises the original divide exception at C254AE with
continuation C254B0. The CPU adapter retains the original exception frame and
runtime handler. A forced zero-divisor fixture exhausts a test-only 100,000
instruction observation limit at FC30C2, exit 3. It is retained as an incomplete
original ROM-service proof and is excluded from successful call counts. The
wrapper includes production bus code unchanged and never returns a service
value. The standalone domain requires an exception backend for this input;
without one, it fails explicitly. OS exception-handler completion is unproven.

Actual ON/shadow/sandbox dispatch passes 2,304 fixtures (256 per owner/mode),
requiring native ON continuations and strict reference classification. Bounds
and readout fixtures are hardware-free completed comparisons. Timer fixtures
actually touch hardware and require exact hardware classification instead.
Non-call, OFF, selection and source-write invalidation guards also pass.

All six step-disabled recording reports are retained. C2527C has zero calls.
C25312 has 3,937 hardware-bearing plus 146 incomplete shadow calls and 4,230
hardware-bearing sandbox calls, with no recorded match claimed. C2548A has
504 shadow and 505 sandbox matches; one shadow call is incomplete. All entries
have zero mismatches. The generic checker correctly rejects C2527C for no
completed recording comparisons; that assertion is unchanged.

Local instruction/DMA proof passes 157 instructions / 5,024 cases, comparing
registers, full SR, PC, cycles and all RAM. This includes division exception
instruction frames, but does not establish completion of the ROM handler.
The independently proven union is 17,849 / 571,168, with no new overlap.
New multiply/divide, MOVEM store, EXG and arithmetic-direction recipes are
family-local. All fifteen older generator outputs are byte-identical. Shared
CPU/bus/math and instruction fixtures remain unchanged. No fresh combined run
was made; the last fresh combined proof remains 16,384 / 524,288.

GNU and MSVC Release builds pass. The full 547-row gate passes 552,046 shadow
and 413,271 sandbox matches, zero mismatches, exact seals and identical poison
frames. Parent absorption changes the comparison totals. Every one of the
36,236 isolated live frames and final RAM seals matches source OFF. The group
is exact through frame 600; ALL remains 416/361. Evidence, raw report totals,
per-owner coverage and hashes are in
`analysis/figures/native_main_loop_timers_checkpoint.json`.

Next complete C1518C/C32CEE, sealed in
`analysis/data/main_loop_control_messages_scope_inventory.json`: 503 unique
instruction boundaries, no shared boundaries. These owners are not implemented
by that inventory. Continue complete game functions to the user's stopping
point; Kickstart services and timing work remain deferred afterward.
