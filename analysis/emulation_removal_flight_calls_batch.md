# Flight input and indexed-record call ownership

The active runner enters C25B66 through the machine frame/recomp hook.
Its original C25C70 and C25D7E calls now select existing game C owners:
`update_dynamics_record_input` -> `update_flight_input`, and
`update_dynamics_selected_record` -> `update_indexed_record`. Copied C arguments
survive event/IRQ service; temporary glue publishes the existing caller scratch
results. Child CPU entries C1B27E/C13D84 and nine exclusive helper entry adapters
are removed, including registry rows and prototypes. Known original calls and
actual C calls are checked in native_call_graph.json/static_recomp.py.

The 2,400/18,000-cycle atomic charges remain unchanged. This removes call-site
CPU dispatch dependencies, not the outer flight CPU/event boundary or shared
guest state. The helper C implementations were already called from these two
owners; their retired CPU entries were not additional live ON work removed.

| Recording | Frames | Parent-selection dispatches removed | Combined dispatches removed |
| --- | ---: | ---: | ---: |
| demo01 | 800 | 108 | 110 |
| qual_carrier_success | 800 | 0 | 0 |
| qual_fail_crashes | 800 | 104 | 104 |

All six pairs preserve RGB, palette indices and final RAM byte for byte, with
identical instruction, chipset, OS and port counts. The sole runner-statistic
change is the exact dispatch decrease. Both GNU/MSVC runners build; twelve
CTests and GNU profiling invisibility pass. The DMA flight oracle matches
554 instructions / 17,728 cases and the copied-argument/IRQ/reset fixture passes.
An 800-frame MSVC/GNU combined demo matches all drawing and final RAM.

Parent source components: all 309 sandbox calls match. Shadow has 205 compared
calls, all matched, plus 104 incomplete calls (32/10/62), exactly as before;
incomplete calls are not passes. Those proofs execute held original children;
live ON before/after captures separately prove the new runtime C calls.

There are 23 C-owned entries and 591 readable CPU registry rows. Reconstruction
count is not removal progress. Bounded CPU-work delta is 0.0000 percentage
points; full raw minimum 38.4011% is cached, with acceptance unavailable while
source parity fails. Memory/chipset/boot cutover is 0%, subsystem deletion 0/4.
Combined first non-fade differences remain demo/carrier/crash 565/446/263.
No full replay was repeated. Next: C2C392, the 626-instruction original flight
record-action child executing 27 times in the bounded demo. Reconstruct and
connect that complete owner to remove actual remaining generated CPU work.
