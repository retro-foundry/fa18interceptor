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

The target currently reaches the first OpenLibrary wrapper and stops with its
caller, target, machine time and zero ROM read/fetch counters. It is a startup
development target; menus, gameplay and persistence are not accepted yet.

Current work prioritizes behavior-level compatibility and playable performance.
Exact OS timing is deferred in `../../analysis/routines/romfree_exact_followup.md`.
The ROM-backed `fa18_recomp` target remains a separate oracle. Reusable loading
and service code belongs in `../amiga`; this directory owns only this game's
asset identity, placement and startup configuration.
