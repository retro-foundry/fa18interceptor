# Gameport, text-resource setup and outer display synchronization

Updated 2026-10-03. Four complete source-only owners are activated:
C16D4C/C16FF4/C17066/C1787A. Coverage is 480/624 translated plus 65 source-only
entries: 545 registry rows and 359 source-timed entries. C1612C has complete
domain/CPU/step source but remains unregistered after a frozen-event reference
failure. Do not count it as an activated translated entry or a recorded
whole-call match. Original game-function coverage remains incomplete.

Authority is the original bytes in `captures/native/demo01/state.bin`, sealed
in `analysis/data/input_display_setup_source_scope.json`: five complete owners,
388 unique instruction boundaries, no shared boundaries and 49 child sites.
Calls remain distinct boundaries. Actual original calls prove all four added
source-only entries; C1612C is called by the original outer loop at C15DA8.

| Owner | Complete source behavior | Activation |
| --- | --- | --- |
| C16D4C | Create the gameport port/request, open gameport.device, retain original diagnostic/cleanup paths, set controller type and trigger, then submit the read request. | Registered |
| C16FF4 | Set the caller's controller-type byte, send/wait/receive the request and sign-extend its reloaded error byte. | Registered |
| C17066 | Build the original eight-byte event-trigger record in the caller frame and submit it. | Registered |
| C1787A | Load original text resources, allocate/duplicate descriptor slots, preserve callbacks, signed offset remapping and feature-bit writes. | Registered |
| C1612C | Wait, publish the selected view pair, load the view, execute the signed activity/palette loop or idle palette path, then reload and toggle the selected page. | Inactive; frozen-reference integration open |

Readable bodies are in `port/game/input_display_setup.c`; the adapter is in
`port/game/glue/glue_input_display_setup.c`. Source filenames include
`df0:text/textegn`, `textegn2`, `texttre`, `textger`, `textcpt` and `textwnd`.
Their meanings beyond the original resource/descriptor dataflow are not
invented. Original children, return PCs, stack arguments, frame bytes, register
halves, full SR, local/global reloads and partial-write order are preserved.
Production calls retain original services, including diagnostic exits.

Independent full-register/full-SR/all-RAM proof passes 73,728 complete cases,
without exclusions: 8,192 controlled-child cases per owner and 16,384 actual
original-child cases each for C16D4C/C17066. Controlled cases cover every one
of the 388 PCs, including changed request/port pointers, text descriptors,
palette sources, page indexes and activity counts across calls. Child entry
comparisons retain all CPU and RAM. Diagnostic-exit test contracts deliberately
return to exercise remaining literal parent instructions; they do not establish
real exit-service behavior. Production does not invent an exit result.

The real root fixture covers 51 of its 92 owned PCs and executes all 26/19
nested controller/trigger PCs. This is successful root-path evidence, not full
real error-path coverage or an independent standalone controller CPU proof.
C17066 real proof covers all its 19 owned PCs. Actual ON/shadow/sandbox
dispatch passes 1,536 fixtures for these two entries; ON must enter its native
continuation and reference modes must complete hardware-free comparisons.
Mode, selection, non-call and source-write invalidation guards remain strict.

Standalone original fixtures do not complete for C16FF4, C1787A or C1612C.
C1787A retains the original case-zero FC0FF0 stop. The other two exhaust a
test-only 100,000-instruction observation bound at AF3C90/C02776. The wrapper
includes production bus code unchanged and exits with failure; it never returns
a service result. These failures are retained and excluded from completed
real-service/dispatch counts. Independent OS/hardware timing is unproven.

Temporary C1612C registration passes all 36,236 native ON frames and RAM seals.
A separate 600-frame ON report proves 2,184 actual native calls. Both the full
and normal-C frozen-reference checks fail: the original graphics wait does not
return and the write log grows until runs fail. A live remaining failing process
was inspected with guest PPC FC5E90 and PC C02776, then stopped. That source
dependency is consistent with the separately bounded fixture failure.

C1612C was removed from production registration. Its normal body, CPU adapter,
80-PC timing bridge, 8,192 controlled cases, temporary registry/native replay,
raw failed gates and debugger witness remain explicit. No service return or
hardware input was fabricated, and no comparison assertion was weakened.
Its frozen-event/service integration must be resolved before activation;
temporary native success does not erase the failed reference requirement.

With the final four-entry registry, all six step-disabled recording reports
contain zero calls/matches/hardware/incomplete/mismatches. The generic tool
correctly rejects C16D4C for no completed comparisons. These are cold entries,
so recording integration is separate from their direct CPU/dispatch fixtures.

Local instruction/DMA proof passes 388 / 12,416, checking registers, full SR,
PC, cycles and all RAM, including the 80 inactive display PCs. The independently
proven instruction union is 17,692 / 566,144. There is no new overlap. Shared
runtime CPU/bus/math and instruction fixtures are unchanged; all fourteen older
generator outputs are unchanged. No fresh combined run was made this batch;
the last fresh combined proof remains 16,384 / 524,288.

Final GNU and MSVC Release builds pass. The 545-row native gate passes 554,068
shadow and 413,271 sandbox matches, zero mismatches, exact RAM seals and
identical poison frames. All 36,236 isolated live frames/seals for the final
four-entry group match source OFF; those entries are cold. The bounded group
is exact through frame 600, while ALL remains 416/361. Detailed per-owner
coverage, stops, cold rows, failed/successful temporary checks and hashes are
in `analysis/figures/native_input_display_setup_checkpoint.json`.

Next complete the five main-loop timer/control/message owners sealed in
`analysis/data/main_loop_services_scope_inventory.json`:
C2527C/C25312/C2548A/C1518C/C32CEE, 660 unique / zero shared boundaries.
That inventory implements none of them and replaces no OS service. Continue
game functions to the user's stopping point; Kickstart and standalone timing
work remain separate, and the Copper fade remains deferred.
