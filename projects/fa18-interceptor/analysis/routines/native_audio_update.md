# Native voice programs and audio updates

`port/audio_update.c/.h` implements complete audio update `$C50158`, voice
program execution `$C50212`, output settings `$C501E0`, and master fade
`$C24FE8` with ordinary native objects. The state shares the actual voices,
slots and fade gate used by native command effects. Program values are
mutable caller-owned data; a command's tone/sweep patch is read directly
by the update loop, without another register/memory representation.

Following the user's request to build components reusable by future ports,
the game-independent executor and slide updates live in
`port/voice_program.c/.h`, under the independent `port_voice_program` library.
`PortVoice`, `PortVoiceProgram` and typed sound operations have no game,
CPU, bus, ROM, SDL or hardware dependency. `FA18CommandVoice` and
`FA18CommandSoundProgram` are aliases to those canonical types.
The source selector decoder, channel descriptor binding, period/volume
rules and master fade remain in the F/A-18 adapter. See
`port/REUSABLE_COMPONENTS.md` for the reuse boundary and existing file-format
components; no generic engine or alternative gameplay is introduced.

The importer supports every selector present in the five original programs:
period, volume, both slides, both loop-counter assignments, wait, and both
loop jumps. Selectors are resolved to named operations, never generic writes
to a CPU address space. Unknown selectors are asset errors. The source
programs are `$C50B78` (12 instructions), `$C50BD8` (5), `$C50C00` (6),
`$C50C30` (8) and `$C50C70` (12). No program bytes or sample data from a
recorded machine image are embedded in the native executable.

A nonzero delay decrements with 32-bit wrap. When it expires, the executor
runs from the imported byte cursor until a wait. A wait records the next
cursor and delay; zero delay clears the actual supplied slot then invokes
the end callback. Loop counters decrement when nonzero and fall through
when the result becomes zero; a zero counter always jumps. Positions and
jump values require aligned, bounded imported instructions. Missing data
returns an error after preserving preceding writes; there is no invented
termination or instruction-count limit.

The update loop resolves each descriptor's actual slot pointer, including
aliases, and retains source order across all four descriptors. For an active
voice it runs the program, writes period then volume, adds both slides and
expires their nonzero durations. **Program termination still outputs and
slides that original voice for the current tick**, even after clearing its
slot. Empty slots produce no output. Period is the signed high word clamped
to the source minimum 124; volume is its masked high word limited by the
signed high word of the master level. These rules belong to the game adapter.

Master fade shares the command audio owner's gate, current level and target.
The original quarter step is `$4000`. Downward clamping uses the signed
arithmetic condition immediately after subtraction, including overflow;
upward clamping compares the wrapped result with `$3F0000`. Overflow edge
cases are validated against the original instructions rather than modernized.

Run `python tools/recomp/check_native_audio_update.py`. The validator checks
the sealed original state and every reachable original instruction byte.
There are **16,384 calls and 105/105 source boundaries** across the four
entries. The original program/output/interrupt children execute fully,
with no contracted child substitutes. Comparisons include all Chip/Slow RAM
except the fixture's original CPU ABI stack `$C7FD00..$C7FF00`, full RAM at
every period/volume/acknowledgement boundary, ordered channel/payload data,
and final custom-register/interrupt state. The host validation callbacks
apply original output payloads to the reference machine; production code
contains none of that hardware adapter or CPU stack.

Fixtures cover all five programs, loop counts zero/one/two, arbitrary valid
entry positions, idle/expiring/wrapped delays, empty/aliased/permuted slots,
reordered output descriptors, slide expiration, signed output limits and
extreme fade values. The command-effects regression also passes its 12,288
calls and all 278 source boundaries after the shared-type change.

The portable core builds/tests alone with GNU strict warnings and no
`fa18_*`/CPU/bus/machine symbols. The full native audio contract also passes
strict GNU warnings and has no CPU/bus/machine symbols. MSVC builds and all
ten input/audio CTests pass. The unchanged native build guard passes 434 files,
including both linked libraries. The new integration contract starts a real
native status tone, executes its actual shared sound program to completion,
and checks final-tick output, aliases and required output-service failures.

The full game remains incomplete. These native components need original
asset import, tick scheduling and actual sample output. The bounded native
game does not call the new command/audio loop yet; the playable reference
still uses its machine model. Continue with sample playback, the original
input callback/host service and full native game-loop composition. The proof
checkpoint is `analysis/figures/native_audio_update_checkpoint.json`.
