# Native port moved to its own repository

On 2026-10-10 the user requested the playable port move into
[retro-foundry/fa18-interceptor-decomp](https://github.com/retro-foundry/fa18-interceptor-decomp).
The extraction starts at `ffcd1e6300724fcb20d186f67eeead4bd5c37f43`, with file
provenance, root build, native tests and current scope documented in that repo.

Native entry/composition move there; shared reconstructed game routines remain
here because the original CPU/machine comparison runners compile them.
The reusable flight trace moves from `port/native` to `port/recomp` unchanged,
and both comparison build paths use that new local location. The old native
CMake target is removed. The historical build command forwards to the standalone
sibling rather than compiling a second native runner in this framework.

Game media, sealed recordings, historical reports, protected native data tools,
user settings and comparison assets remain unchanged. The standalone import
contains no CPU/chipset framework or original media. Its Release/Debug builds
succeed, and its Release executable preserves the current runner's complete
intro, Free Flight and final-combat WAV, RAM, pixels, saved pilot and counters.
Standalone test completion and executable identities are recorded in its final
migration validation report. Full original/native sound acceptance stays open.
