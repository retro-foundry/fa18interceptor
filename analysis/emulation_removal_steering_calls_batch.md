# Direct autopilot steering calls

The active runner reaches C25B66 -> native C2C392 at original C25C6A. Six
steering selections now call game C functions directly and return C values:
C2CA26, C2CA92, C2CAA0, C2CB86, C2CB82 and C2CBBC. They no longer yield to
their CPU entry adapters from this owner. Original normalization/fault and
unknown action transfers still use the runtime. Steering adapters remain for
other original callers and unresolved original transfers; none are counted
as newly exclusive C entries. The source audit validates the new C call edges.

The production continuation matches all registers, PC, full SR and Chip/Slow
RAM for 49,600 source-derived cases with held events. It exercises 17,118 direct
steering calls and observes 864/987 parent instruction boundaries, including
all 361 cold boundaries. This proves tested state behavior, not complete branch
coverage or event timing. Logs: `build/steering-calls/live-oracle.log`.

Six focused real-runner frame pairs start at original C22D88's JSR C25B66 and
stop at the source autopilot return C25C70. All six native steering edges occur;
steering CPU-entry dispatch is absent. RAM, RGB and indices match ports-OFF
source execution in every pair. The test masks interrupts and stops unrelated
flight work; ordinary chipset events continue to the frame boundary. These
are constructed source-input fixtures, not sealed gameplay coverage.

Three 800-frame sealed recording probes have identical RGB/index/RAM and entire
emulation profiles to `4f136fa8`. An MSVC/GNU 800-frame demo matches. Extended
3000-frame demo and 2600-frame GNU ADF probes do not exercise these steering
arms. Both compilers/runners build, twelve CTests and GNU profiling checks pass.
No full replay suite was repeated.

Progress remains scoped: bounded demo raw CPU work avoided **69.8779%**, batch
delta **0.0000 pp**; full raw minimum **38.4011%** is cached. Accepted CPU share
is unavailable because source parity is open. Native memory/chipset/boot **0%**,
deletable subsystems **0/4**. Inventory unchanged: 25 exclusive C entries,
590 readable CPU rows, 84 deferred entries, 691 opcode bindings. Parent timing
and the prior zone-exit crash regression (185 non-fade pixels) remain open.

Reproduce the frame probe:

```powershell
python scripts/build_recomp.py --main tools/recomp/record_autopilot_frame_main.c --output build/recomp/record_autopilot_frame.exe --replace-source port/machine/machine.c=tools/recomp/record_autopilot_frame_machine.c --replace-source port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c --replace-source port/machine/bus.c=tools/recomp/flight_record_actions_bus_budget.c
python tools/recomp/record_autopilot_frame_check.py
```

The continuation build uses `record_autopilot_oracle.c` instead of the frame
main and omits the machine replacement. Run that executable with
`49600 C2C392 --live`. Both probes share the source-derived input fixture.
