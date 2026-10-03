# Complete game input-device callback and setup owners

Updated 2026-10-03. The eight complete owners add three translated and five
source-only callable entries: 480/624 translated plus 61 source-only entries,
541 registry rows and 355 source-timed entries. The seeded denominator does
not establish whole-game coverage. Stage D, Stage F and needed Stage E remain
open; the Copper-fade investigation remains deferred.

Authority is the original bytes in `captures/native/demo01/state.bin`, sealed
by `analysis/data/input_device_callbacks_source_scope.json`. Static branches
are followed to returns; calls remain child boundaries. The scope contains
274 unique instruction boundaries and no shared boundaries within the batch.
The five source-only entries have actual original calls, preserved in
`analysis/figures/native_input_device_callbacks_checkpoint.json`.

| Owner | Complete readable behavior |
| --- | --- |
| C1718E | Read JOY0DAT, split counters, correct wrapping deltas, halve negative vertical input when required, clamp input coordinates, advance the viewport byte countdown, preserve palette-load and view-pair publication order, copy the final 16-word palette and call the original fade child. |
| C17456 | Populate the callback descriptor, publish the original C1718E address and call the actual installation service. |
| C1748C | Remove that descriptor through the actual removal service. |
| C174A0 | Initialize the caller's port, allocate its signal, obtain its task and initialize its list; reload the argument after each child. |
| C16CD8 | Open the original timer request, retain the word result and original test/return flags. |
| C16B8C | Open the input request, prepare its port, populate the request and submit it, preserving original push/store order and reloading globals after children. |
| C17104 | Copy four caller word arguments into the original signed coordinate bounds. |
| C1712C | Split the initial hardware counters and call the actual callback installer. |

Readable behavior is in `port/game/input_device_callbacks.c`; the CPU adapter
is in `port/game/glue/glue_input_device_callbacks.c`. Fourteen actual child
sites retain original targets, return PCs and stack arguments. No service
return is invented. The adapter preserves full register halves, full SR,
stack/frame bytes, original local reloads and partial-write order. Its
mouse-read observation sets the original read PC for the existing source-first
input replay contract. The generated timing bridge owns exactly the sealed
274 PCs and executes no opcode handlers in production.

`check_input_device_callbacks.py` passes 196,608 complete CPU/SR/all-RAM cases,
with no exclusions: 8,192 controlled-child and 16,384 actual-original-child
cases per owner. Both layers visit every owned PC. Contracts compare all
registers, SR and RAM at child entry before returning changed outputs; they
mutate caller arguments, request/port globals, saved page index and palette
source to prove the original reloads around children. This proves the parent
contract and sampled original calls, without establishing independent OS or
hardware timing parity. Earlier mode-file service stops remain unresolved.

`check_input_device_callbacks_dispatch.py` passes 6,144 actual-dispatch cases:
256 per owner per ON/shadow/sandbox mode. Every mode covers every owner's PCs.
ON must enter and complete its native continuation; reference modes must
complete the exact hardware-free match or explicit hardware classification.
OFF, selection, non-call and source-write invalidation guards are retained.

The first dispatch fixture expected hardware classification for an uncaptured
mouse read, while unchanged production shadow captured the real input and
completed a match. Its raw report/hash and selected callback row remain in
the checkpoint. The test-only reference now mirrors production shadow's
capture policy using the unchanged registry and actual original read. Sandbox
retains its uncaptured hardware classification. The strict shared assertion
and production dispatch implementation are unchanged by this fixture fix.

With timing steps disabled in the temporary proof registry, normal callback C
matches all 36,236 recorded shadow calls. It has zero hardware, incomplete or
mismatched shadow calls. Sandbox retains 36,385 hardware classifications,
zero matches, incomplete calls or mismatches. The seven setup/removal owners
are cold on all three recordings. All six raw batch reports, their zero rows
and the generic C17456 rejection remain retained. The isolated normal callback
check passes independently. Hardware and cold rows are not counted as matches.

Local instruction/DMA proof passes 274 instructions / 8,768 cases, checking
registers, full SR, PC, cycles and all RAM. The independently proven instruction
union is 17,304 / 553,728, with no overlap this batch. Shared runtime CPU/bus/math
and instruction fixture setup are unchanged. All thirteen older generator
outputs are unchanged; the new NOP generator recipe is covered locally. No
fresh combined oracle was run this batch; the last fresh combined proof remains
16,384 / 524,288.

The full 541-row native gate passes 554,068 shadow and 413,271 sandbox matches,
zero mismatches, exact final RAM seals and identical poison frames. All 36,236
isolated live frames and seals match fresh source OFF. The group is exact
through frame 600; ALL retains its first difference at frame 416, 361 pixels.
GNU and MSVC Release builds pass. Checkpoint evidence includes per-owner
coverage, mode-specific classifications, raw recording rows and artifact hashes.

The next original-source inventory is
`analysis/data/input_display_setup_scope_inventory.json`: complete
C16D4C/C16FF4/C17066/C1787A/C1612C input/display setup and synchronization
owners, 388 unique / zero shared boundaries. That inventory implements none
of these owners and replaces no OS services. Continue game C before the native
backend and needed OS services.
