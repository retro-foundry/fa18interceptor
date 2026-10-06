# Native record autopilot in the playable runner

The frame loop reaches C25B66 through the existing runtime. Its original call
at C25C6A now selects `update_dynamics_record_action` ->
`advance_record_autopilot` in `port/game/flight_dynamics.c`. The native owner
contains all forty original action arms, including 361 cold instructions absent
from the old 626-instruction generated body. The audited source owner totals
987 instructions. C2C392 moves out of the deferred CPU-owner inventory; its
original generated body remains for reference and unresolved transfers. Runtime
normalization/steering children and unknown computed transfers remain original
CPU dependencies.

The retained `AutopilotFrame` holds the next C phase and eight response limits.
Known child calls yield through the production dispatcher; PC/SP guard their
return, while the C phase chooses subsequent behavior. Temporary glue mirrors
caller registers, flags and stack locals. Unknown or modified table targets use
the source's exact signed-byte index and transfer; there is no invented clamp.
The producer domain is still unresolved. Fixtures for every out-of-table byte
redirect only its original slot to a real source return arm: they prove index
and transfer selection, not arbitrary fault-target execution.

Component evidence: 49,600 held-source-child cases match every register, PC,
full SR and Chip/Slow RAM byte; 864/987 parent instructions were observed.
Separately, 12,400 cases using the actual C continuation and production runtime
children match the same state; 841/987 instructions were observed. Both cover
all 361 cold instructions. Neither claims complete branch coverage. The source
audit reproduces all 987 boundaries and checks the oracle's ownership array.

| Bounded recording | Native autopilot calls | CPU instructions fewer than before | Raw CPU work avoided before / after |
| --- | ---: | ---: | ---: |
| demo01, 800 frames | 27 | 2,076 | 69.8381% / 69.8779% |
| carrier, 800 frames | 0 | 0 | 48.0797% / 48.0797% |
| crash, 800 frames | 0 | 0 | 69.4983% / 69.4983% |

Live C2C392 CPU entry dispatch is zero. Demo instruction reduction is an
aggregate run result, including downstream timing/path effects; it is not an
exact isolated count for its 27 parent calls. Original child timing is retained;
parent instruction/event timing is explicitly unmodeled, with no fixed average
charge. Demo source parity improves from first non-fade difference 565 to 619
(32 pixels there) but still fails. Carrier/crash preserve previous RGB, indices
and final RAM byte for byte, with source differences still 446/263. Copper fade
is excluded throughout. A GNU/MSVC demo matches RGB, indices and final RAM.

Both runners build with both compilers. Twelve CTests, GNU profiling
invisibility, and the 554-instruction / 17,728-case parent DMA oracle pass.
The continuation fixture also checks copied/mutable argument lifetime,
IRQ and SP guards, completion, retirement and reset.

There are 24 C-owned entries, 591 readable CPU registry rows, 84 deferred
entries and 691 direct opcode bindings. These counts are reconstruction and
ownership evidence, not whole-plan percentages. The last full raw CPU minimum
38.4011% remains cached; no new full-suite delta or accepted CPU percentage is
claimed. Memory/chipset/boot cutover is 0%, subsystem deletion 0/4. The bounded
three-recording minimum also stays 48.0797%. No full replay suite was repeated.

Reproduce the source audit with
`python tools/recomp/audit_record_autopilot_source.py`. Build the component with
`python scripts/build_recomp.py --main tools/recomp/record_autopilot_oracle.c --output build/recomp/record_autopilot_oracle.exe --replace-source port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c --replace-source port/machine/bus.c=tools/recomp/flight_record_actions_bus_budget.c`,
then run `build/recomp/record_autopilot_oracle.exe 49600` for the held-child proof
or `build/recomp/record_autopilot_oracle.exe 12400 C2C392 --live` for continuation
integration. Next: remove remaining normalization/steering CPU child calls,
then C28E28 in the same live flight parent.
