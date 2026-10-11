# Live matrix call dispatch removal

Runner -> machine frame -> C25B66 flight step -> source JSR C25D9E now schedules
copied C arguments for `update_dynamics_record_matrix` -> `update_record_matrix`.
The callee is selected at the parent callsite, rather than by a C2D408 CPU entry
lookup. That entry adapter and registry row are removed. The compatibility
boundary still publishes caller values and preserves the existing 9,500-cycle
atomic timing charge. Interrupt/stack guards postpone an already-selected C
call; they do not choose game behavior. This is a connected call-site removal,
not a CPU-independent flight parent or frame.

| Recording | Frames | Isolated parent dispatches removed | Combined dispatches removed |
| --- | ---: | ---: | ---: |
| demo01 | 800 | 70 | 68 |
| qual_carrier_success | 800 | 15 | 15 |
| qual_fail_crashes | 800 | 76 | 75 |

All six before/after pairs match RGB, palette indices and final RAM byte for
byte; all other runner statistics and instruction/device counts match. Held
parent shadow/sandbox checks have no mismatches. These use the original child
reference and are component checks; the live ON captures prove integration.
The new boundary fixture checks argument lifetime, IRQ PC/stack guards,
retirement and reset; the parent DMA oracle matches 554 instructions / 17,728
cases. Both GNU/MSVC runners build, twelve CTests pass, GNU profiling preserves
output/state, and an 800-frame MSVC/GNU combined demo matches.

The existing combined source mismatch remains demo 565 (683 pixels), carrier
446 (3), crash 263 (4), under the Copper-fade exclusion policy. No full replay
was repeated. Bounded CPU-work delta is 0.0000 percentage points; cached full
raw minimum remains 38.4011%, with no accepted percentage while parity fails.
Native memory/chipset/boot cutover is 0%, subsystem deletion gate 0/4.
There are now twelve C-owned entries and 602 readable CPU registry rows;
614 reconstructed entries is not plan completion. See the JSON for hashes and
per-recording evidence. Continue outward into native flight ownership and its
ordered event schedule, then the native frame and remaining plan phases.
