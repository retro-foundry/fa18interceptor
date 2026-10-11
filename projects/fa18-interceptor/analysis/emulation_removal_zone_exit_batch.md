# Native zone-exit call in the flight parent

The active runner's frame/recomp path reaches C25B66. Its call at C25BA6 now
selects `update_dynamics_record_zone_exit` -> `advance_record_zone_exit` in
existing game modules. A retained C frame selects fault/placement continuation
phases; the original two children remain in the runtime. C28E28's CPU entry,
registration, ownership guard and sole wrapper file are retired. Its original
generated reference remains. The existing held-child component entry remains
available as `glue_complete_zone_exit`.

The production continuation passes 16,384 original-source cases: every register,
PC, full SR and Chip/Slow RAM byte matches, with all 79 source instructions
observed. Both runners build under GNU/MSVC, twelve CTests and GNU profiling
invisibility pass. GNU/MSVC demo RGB/index/RAM match for 800 frames.

| Recording, 800 frames | Native calls | CPU entry calls | Total CPU instruction change |
| --- | ---: | ---: | ---: |
| demo01 | 2 | 0 | 0 |
| carrier | 0 | 0 | 0 |
| crash | 2 | 0 | 0 |

Demo/carrier RGB, indices and final RAM match the preceding build exactly.
Crash final RAM and total CPU instructions also match, but 185 non-fade pixels
change across the run, first at frame 556 (14 pixels there). These changes add
to source mismatches; they are not Copper fade. Source first differences remain
619/446/263. Component and continuation correctness are demonstrated; rendering
acceptance is open. Parent instruction/event timing remains unmodeled. The
instruction-adapter path was already metered as port work, so this batch removes
a CPU-entry/stepping dependency without raising the CPU-work percentage.

Counts: 25 C-owned entries, 590 readable CPU registry rows, 84 deferred entries,
691 direct opcode bindings. Raw bounded demo work avoided remains 69.8779%;
batch delta 0.0000 pp. Full raw minimum 38.4011% remains cached. Accepted CPU
percentage is unavailable; memory/chipset/boot cutover is 0%, deletion 0/4.
No full replay suite was repeated.

Build the live proof using `scripts/build_recomp.py --main tools/recomp/flight_geometry_oracle.c --output build/recomp/flight_geometry_oracle.exe --replace-source port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c --replace-source port/machine/bus.c=tools/recomp/flight_geometry_bus_budget.c`,
then run `build/recomp/flight_geometry_oracle.exe 16384 C28E28 --live`.
Remaining registered geometry dispatch proofs cover the other three owners;
the two component oracles use the preserved complete native fixture entry.
