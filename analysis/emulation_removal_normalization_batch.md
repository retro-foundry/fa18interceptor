# Native autopilot vector normalization

The real runner's C25B66 -> native C2C392 path now calls
`normalize_record_vector` -> existing `magnitude3` in game C. Normalization
inputs, magnitude lookup leftovers and vector results return as C values.
Temporary glue publishes original caller flags and stack residue. This owner
no longer yields to C2574A or C1D974 CPU entries. Those adapters remain for
other callers; the fault child and unknown original transfers still use the
runtime. Guest memory and outer CPU/event timing remain dependencies.

12,400 production continuation cases match all registers, PC, full SR and
Chip/Slow RAM, with held events. They exercise 396 native normalization and
396 native magnitude calls with no corresponding CPU-entry dispatch. The
source oracle observes 841/987 parent boundaries, including all 361 cold
boundaries. Input scope is the autopilot's source scale 192 and fixture
vectors; this is not exhaustive proof of a general normalization API.

The six bounded real-runner frame pairs pass source RAM/RGB/index comparison.
Case 1125 exercises C22D88 -> C25B66 -> C2C392 -> C2574A -> C1D974 native
edges and stops at the original C25C70 return. Interrupts are masked and the
unrelated parent remainder is stopped; ordinary chipset frame events run.
This establishes connected frame integration, not sealed gameplay acceptance
or original instruction timing. Reproduce with the frame build/check commands
in `emulation_removal_steering_calls_batch.md`.

Both GNU/MSVC runners build; twelve CTests and GNU profiling invisibility pass.
Three 800-frame recording probes match previous RGB/indices/RAM and complete
emulation profiles. The MSVC/GNU demo matches. None of those recording prefixes
exercise normalization from this owner, so the measured delta is **0.0000 pp**.
No full replay was repeated. Full raw CPU minimum **38.4011%** remains cached;
bounded demo **69.8779%** remains unchanged. Accepted share is unavailable.
Memory/chipset/boot cutover **0%**, deletion **0/4**. Inventory remains
25 exclusive C / 590 CPU / 84 deferred / 691 opcode bindings. Parent timing
and the prior zone-exit non-fade crash regression remain unresolved.
