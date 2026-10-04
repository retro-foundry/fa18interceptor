# Original ADF launcher

Build GNU headless with `python scripts/build_recomp.py --romfree`, or build
the CMake `fa18_romfree` target in Release for SDL window support.

```
fa18_romfree --adf path/to/original.adf --save-dir local/saves --window
fa18_romfree --adf path/to/original.adf --frames 300
```

The ADF supplies executable bytes and relocations. The compiled Interceptor
profile supplies checked Hunk addresses and explicit process/library ABI
identifiers. No Kickstart ROM, UAE state, captured RAM or SDK is read at runtime.
The save directory defaults to `local/saves`.

Window presentation defaults to host pacing at 50 Hz with monitor vsync off.
Use `--window --vsync on` to select monitor synchronization (which can reduce
tearing but adds display waits). This changes presentation only: it does not
increase the original game's 3D update rate or change gameplay timing.

The target now loads the splash and credits, accepts keyboard input, reaches
the main menu and renders the demo from a clean ADF launch. GNU and MSVC Release
pass the isolated launch smoke test, with zero ROM reads, ROM instruction
fetches and unsupported services. Run `python tools/amiga/check_romfree_launcher.py`
to reproduce it. This is functional smoke coverage, not acceptance of every mode.

Host compatibility supplies libraries, reserved-region allocation, DOS files
and directory enumeration, writable save overlays, View/Copper construction,
keyboard requests and timer queries/waits. Existing C interrupt and graphics
wait services run guest callbacks on the chipset timeline. New services use a
coarse execution charge. The original game startup and gameplay code still run.

Save/load through the game UI, all mission modes, restart and game teardown
still need coverage and any remaining services. Host file round trips already
pass portable component tests. Unknown operations continue to fail with their
caller, service, target and machine time. Faster flight simulation and smoother
presentation remain performance work; no measured speedup is claimed here.

Current work prioritizes behavior-level compatibility and playable performance.
Exact OS timing is deferred in `../../analysis/routines/romfree_exact_followup.md`.
The ROM-backed `fa18_recomp` target remains a separate oracle. Reusable loading
and service code belongs in `../amiga`; this directory owns only this game's
asset identity, placement and startup configuration.
