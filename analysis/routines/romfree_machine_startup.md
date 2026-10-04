# Explicit machine initialization

`port/machine/startup.h` adds an initializer independent of UAE restoration.
`fa18_machine_init` clears Chip/Slow/ROM/expansion-ROM banks, resets the CPU,
timeline and persistent blitter state, installs the caller's CPU/device/beam
profile and enables the strict ROM guard before any CPU memory access. Reset
vectors are read from cleared RAM only. Physical CPU reset cycles are excluded
from the program-handoff timeline. The embedding OS profile must supply
evidence-backed state; this API does not invent or initialize AmigaOS.

Assets and OS structures are installed separately. `fa18_machine_prepare_run`
then predicts DMA from installed guest memory before execution. Preparing after
execution starts fails. Invalid CPU, stack, beam or overlapping profile inputs
fail before changing the machine. DMA prediction allocation failure returns an
explicit initialization error. The ROM-backed restoration API is retained.

CMake now builds the shared game/machine implementation as `fa18_runtime`.
The reference executable and startup test link that same library. SDL window
integration belongs to the reference executable, not the runtime library. The
future `fa18_romfree` executable will use this shared runtime and the independent
ADF/Hunk loader; that executable and its complete OS initializer remain absent.

## Validation

`python tools/amiga/check_machine_startup.py` builds and runs the GNU constructor
test without reading ROMs, savestates or game assets. The same test passes MSVC
Release CTest, with assertions enabled. It checks cleared banks, exact complete
CPU handoff, user/supervisor stacks, repeated initialization, live interpreter
frames, preflight failure atomicity, overlapping input rejection, and clearing
persistent blitter B data between sessions. Candidate executions have zero ROM
reads, instruction fetches and unsupported services. These are synthetic
machine fixtures, not a ROM-free game launch.

GNU and MSVC Release reference builds pass. After the CMake runtime extraction,
the MSVC full service-versus-original-ROM comparison matches all 36,236 sealed
recording frames, every RGB444 byte, full final RAM, CPU cycles, final PC and
blitter counters. Output hashes are unchanged from `romfree_foundations.md`.
The protected native-build check passes 406 files.

Local proof logs are `build/amiga/machine_startup_test.log`,
`build/recomp/machine_startup_{gnu_build,msvc_build,msvc_ctest}.log`, and
`build/recomp/machine_startup_service_recordings.log`. Full replay metadata
is in `build/amiga/machine-startup-recordings/full.json`. The reusable recording
checker accepts `--output` to preserve separate runner/configuration evidence.

Remaining work includes a source-backed game/OS startup profile, Exec scheduling,
interrupts/exceptions, graphics and devices, DOS persistence, original game
startup/exit, clean-launch recordings and whole-game zero-ROM acceptance.
