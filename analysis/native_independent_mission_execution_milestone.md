# Independent mission-three update-call comparison — 2026-10-09

The complete successful mission-three gameplay sequence now matches the original
in independently started native Release and Debug. All 4,967 original flight
observations match all 79,472 complete aircraft record cores and the named
camera/control/target fields. Both native builds earn grade one and return to
the original menu callback without a reset or queued input. Their complete
traces, final RAM, counters, earned saves and comparisons agree.

Strict drawing remains incomplete: 267/4,967 complete page sets match; the first
difference is still original loop 22,286, planes 4/6/7. Clock/HUD diagnostics
remain explicit and unaccepted. This milestone establishes gameplay-state
parity for this successful flight, not exact complete-flight visual parity,
all-mission completion, audio fidelity or visible performance.

## Why the previous comparison diverged

The original reference recorder counts dispatches at C0EFD4 before its first
instruction, `LINK A6,#-2`, executes. A known deadline resumption is suppressed
using `fa18_recomp_stop_pc/sp`. If an interrupt's generated instructions replace
that marker before returning to C0EFD4, the recorder counts another observation
of the same pending update.

The first such boundary is loop 22,502. The instruction trace records an event
at C0EFD4 in PAL frame 20,052, entry into FC0CE2 with the interrupt stack, and
return to the original stack in frame 20,053. Only one LINK executes. The entry
probe additionally records the actual C15DA2 JSR, a stop before LINK, then a
dispatch whose previous PC is FC0EC0 and whose stop marker belongs to C501D4.
Loop 24,008 has the same proven ownership issue after an interrupt return.

Across the complete recording there are 27,110 legacy observations but exactly
27,108 explicit update calls and 27,108 executed LINK instructions. The two
extra observations belong to calls 22,501 and 24,006. Unchanged game ticks or
matching aircraft state are never used to infer call ownership.

## Evidence and runtime boundaries

`tools/native/original_update_entry_probe.c` is an external reference-only
adapter replacing `port/recomp/recomp_ports.c` in a diagnostic build. It forwards
every dispatch argument and return value and reads CPU/machine storage directly.
It does not issue guest bus accesses, deliver controls, alter registers or write
game state. The existing bus instruction/event trace observes LINK execution.

The complete probe must reproduce the successful original recording's trace
byte for byte, its full final RAM and all 1,426 consumed key edges. All three
fingerprints match the already independently replayed, unmodified original.
The physical recording and sealed qualification prefix are preserved.

`check_original_update_entries.py` validates the JSR caller, return address,
pre-LINK suspension and resumed stack, and checks every explicit call against
the ordered LINK trace. It maps every original observation to that real call.
Four rejection probes catch a wrong caller, missing executed LINK, changed
resume stack and incorrect suspension marker. The mapping retains all 27,110
observations, including both duplicates.

`check_recorded_original_mission_trace.py --source-updates ...` independently
revalidates that evidence. It translates only the recorded iteration column of
the consumed-key input into actual call identities. All edges, their order and
source PAL timestamps remain intact. Native receives those ordinary controls
through the existing canonical replay path; original RAM never initializes it.
The enlisted pilot earns qualification before the mission. No native or source
clock, counter, aircraft state, physics or pixels are changed.

The native flight trace covers actual calls 22,142–27,106: 4,965 updates. Each of
the 4,967 source observations is compared to its specified native update, with
no search for matching state and no skipped observation. The two final original
menu observations fall outside native's flight-only trace; actual native final
RAM, stage, menu counters, grade and saved pilot are checked separately. Full
pilot records retain date/name/history differences at offsets 5,10,11,30–32.

Release and Debug both pass the complete gameplay gate and its NPC-core,
control and full-page rejection checks. The older failed-flight comparison
still passes all 5,216 cores and camera/control state over 326 boundaries. The
canonical native executable remains unchanged:
`55b2fd45a853fddf76d3aa35ad3632b45d796c06f3036c42f49ab829c59edc23`.

This removes a recorder-accounting dependency from the comparison, not a
gameplay dependency from the runtime. Emulator/ROM execution is reference
evidence only. Native continues to report no CPU or chipset emulation.

## Reproduction and remaining scope

```powershell
python tools/native/check_original_update_entries.py --source-evidence build/native-flight/original-mission-three-patrol-runway --out build/native-flight/patrol-entry-review
python tools/native/check_recorded_original_mission_trace.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --out build/native-flight/original-mission-three-execution-release
python tools/native/check_recorded_original_mission_trace.py --runner build/native-cmake/native/Debug/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --source-updates build/native-flight/patrol-entry-review --out build/native-flight/original-mission-three-execution-debug
```

Use `--assess-existing` to verify retained capture hashes and rerun the gates.
The default fixed-offset comparison remains available and retains its failure;
the original recorder's established behavior and recordings are unchanged.
Complete source/native traces and RAM are compressed, and passing prefix copies
were removed. The diagnostic instruction trace is bounded to 32 MiB; standard
512 MiB native captures and the 4 GiB pruner remain enabled. Fingerprints and
results are retained in `figures/native_independent_mission_execution_checkpoint.json`.

The next full-flight work is the strict mission-three drawing assessment and
independent successful flights for the other missions. Recorded audio/filter
fidelity and visible gameplay performance remain open. Internal state cleanup
is deferred outside this goal, and the uninterrupted campaign remains waived
under the user's latest instructions. Original plain-Escape behavior is intact.
