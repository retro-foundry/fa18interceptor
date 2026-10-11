# Native record-dynamics parent

The active runner enters C25B66 from the existing C22 record-loop calls.
The entry now schedules game `advance_record_dynamics` with retained C phase/state.
All 554 source instruction cases are deleted from the shared CPU stepping body;
the other 754 cases are unchanged. Native zone exit, autopilot, input, selection
and matrix calls compose in this parent. Other children still yield to original
runtime entries. The thin C25B66 PC/stack adapter and guest memory remain.
The generator excludes this native owner rather than restoring its CPU body.

The frame-end handoff now yields before the interpreter fetches an instruction
at a retained C return. Previously the next original instruction consumed its
PC/stack and bypassed the owner. The existing child-dispatch assertions pass
without changes. Reentrant original calls can use their own frames while an
older C owner waits; matching return PC and SP guard resumption.

16,384 held-event comparisons of the C parent with controlled child contracts
match all registers, PC, SR and RAM and exercise all 554 source boundaries.
Six constructed real-runner frame pairs match source RAM/RGB/indices through
the autopilot return. Those frames mask interrupts and stop the parent early.
This does not prove the full live parent with original children: the raw
fixture fails case 2 at C60866, and selected-record/identity-matrix case 2
fails at C461EE. These are open composite matrix/state differences.

Both runners build on GNU/MSVC; twelve CTests and GNU profiling invariance
pass. The MSVC/GNU 800-frame demo matches RGB/index/RAM and complete profiles.
The three bounded recording probes complete and reach native children without
matrix CPU-entry dispatch. Compared with the preceding commit, non-fade output
changes begin at demo frame 591 (39,478 total pixels), carrier 446 (2,989),
and crash 263 (187,594). All three final RAM images differ. Copper fade is
excluded by the existing comparator. Parent instruction/event timing is
unmodeled; these are unfinished parity gates, not accepted differences.
No full replay suite was repeated.

Progress: **42.3547% of this family's CPU instruction cases removed**
(554/1308); bounded demo raw CPU work avoided **69.8706%**, delta
**-0.0073 pp**. Full raw minimum **38.4011%** remains cached. Accepted
CPU share unavailable; memory/chipset/boot cutover **0%**, deletion **0/4**.
The global inventory remains 25 exclusive C / 590 CPU rows / 84 deferred /
691 opcode bindings because the outer original calls still need the thin entry.
The raw meter does not count CPU-style `port_steps` as emulated instructions;
it therefore does not measure this CPU-body deletion. It must not be used as
an estimate of whole-plan completion.

Evidence/logs: `build/flight-owner/contract.log`, `recording-results.json`,
`frame/results.json`, `live.log`, `live-caller-inputs.log` and captures.
Next: replace the outer C22 record-loop CPU body and call this C parent directly;
retain the explicit parity/matrix/timing debt for final acceptance.
