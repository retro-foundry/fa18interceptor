# Native preallocation - 2026-10-09

The playable runner reserves gameplay storage before entering its frame loop.
Project heap calls and CRT stream creation/closure are rejected throughout
title, menus, flight, restart and result transitions. SDL uses a fixed arena;
PCM uses a fixed ring rather than SDL's growing audio packet queue. Exhaustion
fails explicitly. Original update order, randomness and sample sequencing stay
unchanged. Remaining addressed state is still ordinary preallocated PC memory;
its conversion into named structures remains a separate task.

The connected caller is `port/native/main.c`: startup loads the frontend,
replays, window, renderer, texture and audio before locking heap operations.
`native_frontend_tick`, input delivery, `native_audio_render`, PCM publication,
conversion and presentation then run under that guard. CMake force-includes
`port/amiga/runtime_memory.h` into every active native runtime/entry source.
The guard rejects malloc/calloc/realloc/free and fopen/fclose; the runner
checks violations and fails the run. NULL frees are harmless. The reference
emulator and reusable library's ordinary allocation policy are unchanged.

| Owner | Storage and lifetime |
| --- | --- |
| Frontend | Two fixed 512 KiB data banks, aircraft/terrain/render workspaces, input ring and indexed pixels; allocated once before gameplay. |
| Assets | Executable tables, splash, cockpit ILBMs, recorder data and all sample bytes loaded at frontend startup; no flight-time asset loader. |
| Replays | Parsed and grown at startup; only indices advance in gameplay. |
| PCM output | 131,072 stereo signed-16 frames, 512 KiB, allocated before device start; producer and callback reuse it under the device lock. |
| SDL storage | 32 MiB aligned arena touched at startup, installed before any SDL allocation; bounded first-fit reuse with adjacent free-block coalescing and thread serialization. |
| Pilot log | Immutable original 78 bytes retained in the frontend; 64 existing fixed handle slots use direct OS I/O and retain original DOS open/read/write/seek/close decisions. |
| Diagnostics | WAV, flight trace and frame timing have owned 4 KiB stdio buffers initialized before gameplay; bounded RAM/sidecar captures use direct OS I/O. |

The audio callback only consumes copied PCM. It never calls game code or
advances the original C500D8 sample handler. FIFO ordering, signed samples,
stereo, buffer handoffs and PCM generation are preserved. Empty device output
is silence, matching SDL's former queue. A full ring rejects a whole write
without dropping/replacing queued samples, increasing capacity or allocating.
At 48 kHz its capacity is approximately 2.73 seconds; it is a host-storage limit,
not an added game delay or requested backlog. Headless mixing needs no ring.

Pilot refresh/save still follow C162E4/C1643A through the existing domain
owners. A startup without a saved pilot does not create an empty config file.
MODE_OLDFILE reads the original bytes until an actual write materializes the
overlay; existing saved files are read on each original open. Create/truncate,
immediate saves, source disk immutability and cold reload are preserved.

`--memory-report PATH` writes allocator evidence after shutdown. SDL may still
request blocks while handling new event/render paths, but those requests use
the already reserved arena. The report distinguishes these pool requests from
project heap violations. OS, graphics/audio drivers and internal CRT operations
are outside the allocator interfaces measured here; their private allocations
and scheduler timing are not claimed deterministic. The original RNG is not
reseeded or replaced to manufacture deterministic missions.

The final visible Free Flight replay presents all 6,500 frames with a real
audio device and preserves the entire headless WAV/data/pixels/counters. Its
SDL arena peaks at 74,944 bytes; seven small SDL requests use reserved storage,
and project heap/stream violations and pool failures are zero. Known render
and initial event caches are warmed before the guard without presenting an
extra frame or consuming pending input. This does not eliminate every SDL
pool request and is not a new all-mission performance acceptance.

Validation uses the active Release/Debug runtime, including deliberate
allocator exhaustion, failed realloc preservation, alignment, coalescing,
PCM ring wrap, overflow, underrun silence and rejected heap/stream calls.
Ordinary-key intro (3,100 frames), Free Flight (6,500) and final combat (24,200)
preserve entire WAV, both data banks, final pixels, saved pilot and every counter
against the preceding accepted executable. This proves preservation of native
PCM; original complete recording/filter fidelity remains open. The earned cold
tour passes qualification and all six missions with exact expected pilot saves.
That tour exercised the fixed file/audio/SDL backends and heap guard before the
added CRT stream guard and render warmup; its exact executable identity is
retained rather than attributed to the final binary.
The independent qualification/mission-three recording preserves every previous
trace field/page hash, final RAM, counter and earned save under the guard.
Four bounded before/after capture bodies (observations 22,441..22,444) also
reproduce their full fields, cores and pages with the final stream guard;
their previously recorded original drawing differences remain unchanged.

Machine-readable output and validation identities are retained in
`figures/native_preallocation_checkpoint.json`; passing raw captures remain
temporary. Broader whole-flight drawing, original filtered audio and the earlier
visible performance acceptance remain open. No CPU/chipset/glue dependency is
added to the native runner, and the complete-C-port goal remains active.
