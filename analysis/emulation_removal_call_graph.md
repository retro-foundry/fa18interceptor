# Active-runner dependency graph

Traced for Phase 0 of `port/EMULATION_REMOVAL_PLAN.md` on 2026-10-06.
Both `port/recomp/CMakeLists.txt` and `scripts/build_recomp.py` compile these
owners into the actual `fa18_recomp` and `fa18_romfree` runners.

## Startup

- `recomp_main.c:main`, reference runner: `fa18_machine_load_state()` restores
  Chip/Slow RAM, CPU and device state from the sealed original recording.
  `fa18_recomp_init()` installs the translated entry map and OS adapters.
- The same `main`, compiled with `FA18_ROMFREE_MAIN`: `fa18_romfree_load()` in
  `romfree/profile.c` calls `amiga_ofs_open/read()` and `amiga_hunks_parse()`.
  `fa18_machine_init()` initializes guest state; `amiga_hunks_install()` places
  and relocates the original executable in guest RAM. `amiga_exec_bootstrap()`
  builds guest process/task storage. The profile restricts translations to
  original executable storage, installs host services through
  `fa18_os_host_compat_install()`, and calls `fa18_machine_prepare_run()`.
  This is ROM independence with a guest CPU/process handoff, not native boot.

## Every frame

`main`'s headless loop, or `run_window()`, applies recorded/live input and calls
`fa18_machine_run_frame()` followed by `fa18_loop_frame()`. Loop input events
are also delivered at the original update entry via `fa18_loop_iteration()`.

`fa18_machine_run_frame()` calls `m68k_execute()`. Musashi invokes
`fa18_machine_instruction_hook()` before fetching an instruction. The hook:

1. Finishes pending bus timing and calls `fa18_machine_service()` at an
   instruction boundary. This advances scanlines, Copper, bitplane display,
   CIA timers/TOD, blitter completion and interrupt acceptance.
2. Resumes active C timing bridges via `fa18_ports_resume_step()`.
3. Handles enabled OS services through `fa18_services_step()` and the neutral
   `amiga_services_step()` dispatcher. Host compatibility services still read
   and write guest data and return through guest registers/stack.
4. Looks up the current PC and enters `fa18_ports_enter()` or the registered
   source-only owner. Registry rows in `game/glue/ports.c` select domain C/glue
   in ON mode and generated source functions in OFF mode.
5. Returns to Musashi for an instruction when no native/generated entry handles
   the target. `fa18_recomp_resume()` additionally executes original opcode
   helpers between known labels, including source-only reference paths.

Generated `FA18_EXEC_USING` operations call the original Musashi opcode handler
or a static binding to that same handler. Generated calls use
`fa18_recomp_invoke()` / `fa18_recomp_call_dynamic()` to return to the registry.
Three OS RTE paths also call an opcode handler directly. All count as remaining
emulated instructions in the meter.

Registered stepped ports are hand-written operations, but many live paths
still select by PC, read original instruction/extension bytes and manipulate
the shared register file. Their domain counterparts are often used by the
comparison rather than the live timing bridge. Count their steps explicitly;
a CPU-work reduction does not demonstrate a direct native C call graph.

## State and output

Domain `game/memory.h` accesses call `fa18_bus_read/write*()` on the big-endian
guest image. The CPU memory callbacks reach the same image and charge
`machine/bus.c` timing. Game custom writes and blitter waits flow through
`game/hardware.h`, the existing hardware implementation, and the machine's
custom/blitter services. Copper/display/blitter DMA also accesses Chip RAM
directly. `main` / the window consumes the machine's completed RGB444 frame.

## Meter and first removal dependency

Phase 0 adds observation only: no dependency is removed. `--profile` retains
routine call keys and adds `_emulation` with executed instruction counts, bus
API accesses by origin, overlapping 4 KiB RAM-page accesses, chipset operations,
OS steps/entries and port calls/steps. Profiling starts after boot construction,
so loader/scratch-Copper work is excluded. Chipset counts cover the live replay.
Instruction counts include interpreted ROM and original-byte/OS opcode helpers;
ported instruction steps remain separately visible.

`tools/recomp/emulation_meter.py` executes the three sealed recordings and the
ADF keyboard-to-demo scenario on both active toolchains, comparing OFF/ON RGB
and the native recordings' final RAM seals. Optional full launcher checks cover
construction, both CPU modes, splash, credits and keyboard-to-demo.

The next structural removal is C child calls through the guest-PC dispatcher,
starting with a source-proven hardware-free parent/leaf pair actually exercised
by a recording. Existing global ON timing debt must stay visible: this meter's
baseline fails parity, so it cannot certify a new removal until the affected
path and eventual whole suite pass. RAM-page absence is scenario evidence only,
not proof of exclusive native ownership: direct DMA and host compatibility
memory accesses are outside the guest API page count.

Deletion gate: **0/4**. Both builds still link Musashi, generated translation,
guest RAM/bus and the chipset. Full mission-mode acceptance remains open.
