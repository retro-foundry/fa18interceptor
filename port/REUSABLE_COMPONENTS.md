# Components for future ports

Keep reusable mechanisms separate from a game's original rules and data.
Extraction should follow a proven need in this port: the native game must
keep using the component, and its original-behavior proof must keep passing.
Game-specific adapters retain policy, asset interpretation and scheduling.

| Component | Reusable interface | Current limits |
| --- | --- | --- |
| `voice_program.c/.h` | Portable `PortVoice` state, named sound operations, waits/loops, slide updates and a program-ended callback. Builds with only the C standard library. | Uses unsigned 32-bit values and eight-byte program cursor units. It provides tick execution, not sample mixing or a device driver. |
| `amiga/ofs.h`, implementation in `disk.c` | `amiga_ofs_*` reads, file lookup, metadata, bounded range reads and directory scanning from OFS images. | Read-only OFS; DOS locks, volume prefixes and OS service behavior remain adapters. Implementation also contains the F/A-18 wrappers. |
| `amiga/hunk.h`, implementation in `hunk.c` | `amiga_hunks_*` parses CODE/DATA/BSS, relocation records and pointer targets without executing original code. | Supports the loader's implemented HUNK types; native imports must still interpret each game's data. Implementation also contains the F/A-18 wrappers. |

The first new independent build target is `port_voice_program`. To use its
core elsewhere, compile `voice_program.c` and include `voice_program.h`.
An asset adapter supplies operation/value arrays and a caller-owned voice;
the host schedules ticks and receives the end callback. There are no F/A-18
globals, CPU registers, bus accesses, Kickstart requirements or SDL dependencies.

The F/A-18 adapter is `audio_update.c/.h`. It decodes the original sound-program
selectors, applies the game's signed period/master-volume rules, dispatches
the actual four channel descriptors and retains output/acknowledgement order.
Command effects share the core voice objects and mutable program values;
their older type names are aliases, not duplicated state. Another game must
supply its own adapter wherever its original behavior differs.

The core returns specific invalid-argument/program results; it does not add
an implicit terminator or substitute sound. Program-ended clears the supplied
slot before the callback. Loop zero means unconditional jump, delays and
slides wrap at 32 bits, and jumps/cursors must resolve to imported instructions.
These are explicit contracts a future port can compare with its own source.

Standalone validation:

```sh
gcc -std=c11 -O2 -UNDEBUG -Wall -Wextra -Werror \
    port/voice_program.c port/voice_program_contract_test.c -o voice_program_test
./voice_program_test
```

The F/A-18 original-instruction proof is
`python tools/recomp/check_native_audio_update.py`: 16,384 calls cover every
one of 105 source boundaries, with the original program, output and fade
children executed fully. This establishes this game's adapter fidelity;
another port still needs its own original-behavior fixtures. See
`../analysis/routines/native_audio_update.md` for the precise comparison scope.

`viewport_transition.c/.h` now owns one shared implementation used by both
the complete native input callback and the bounded native viewport wrapper.
Its interfaces accept imported palettes and actual publication/load owners,
but its mode delays and load/publish order are F/A-18 rules. Keep those rules
in this game's adapter when a future port establishes a common mechanism.
The callback also shares mouse/throttle words directly with command input;
there is no additional emulated state to synchronize. See
`../analysis/routines/native_input_callback.md` for validation and limits.

Planar rendering, projection and input modules remain candidates, with
game-specific dimensions, tables or state still present. They should be
extracted when another concrete caller establishes the common contract.
