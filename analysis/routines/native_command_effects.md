# Native command message, space and audio effects

`port/command_effects.c/.h` supplies the remaining flight/context/indexed
command children through actual ordinary-state implementations. Install them
with `fa18_initialize_command_effects` and `fa18_install_command_effect_owners`;
the latter retains the caller's pose/span imports. The existing dispatcher
continues to execute eject/publication, direction, throttle and space-release
children directly. Missing original data or a host audio acknowledgement
service fails initialization. There is no CPU, memory bus or ROM dependency.

The earlier command enum names were misleading: `$C25704` posts cockpit
messages, and `$C17F8C` starts sound 6. Neither creates an entity. Historical
enum identities are retained for compatibility and annotated in the header.

| Original owner | Native behavior |
| --- | --- |
| `$C0833E` | Space press respects the low-nibble block gate, mode `$7D` and signed weapon-pause gate; updates target requests, emitted requests, command word or the space latch. The event is unchanged. |
| `$C25704` | Stores the message code, sets the low cockpit byte's posted bit, updates timing flags and clears the event low byte while preserving its high word. |
| `$C33186/$C3318E` | Context-gated status tone, kind 2, pitch 2 while fading and 4 otherwise; only a positive signed mute byte suppresses sound. The direct status entry returns event 2; a skipped context entry retains its input. |
| `$C17EF2/$C17B2C/$C17B08/$C4FFB4` | Patches sound 4's actual program values, resets position/delay, releases/replaces channel 3 and acknowledges it in the original three-call order. Instruction 8's loop value remains untouched; the final delay is instruction 9. |
| `$C0F4A6` | Clears each of the four slots and acknowledges each channel in order, including empty slots; returns event 12. |
| `$C17F8C` | Disabled sound sets fire state `$FA` and sound-6 mode 2. Enabled sound patches period/slide/ticks, advances the original shift-register RNG by 11 bits, initializes the voice and replaces channel 2 with three acknowledgements. A missing voice retains the input event. |

The signed divide inside the sweep is the original **software** helper
`$C52EC8`. Zero divisor returns zero, and the quotient wraps at 32 bits.
Native code uses a wider C intermediate to preserve that behavior without
undefined division overflow. Negative periods, divisors and fixed-point
shift wrap are covered by the original-instruction oracle.

Canonical shared fields retain their owners: indexed state owns separate
high/low cockpit bytes; flight state owns the space latch; audio owns fading
and sound-6 mode. The queue binds the space latch at initialization and
attaches the audio bytes at effects initialization. Its bounded byte-binding
API imports the current canonical value before rebinding, including a word's
high/low byte. No duplicate flags are synchronized around calls.

The audio owner requires actual imported voices, mutable program values,
channel masks, seed and gates. Acknowledgement is an explicit native host
service, with channel identity and original mask payload. The component
implements game-side sound starts and releases. Voice-program updates,
slides, output settings and fading now have a real native owner in
`audio_update.c`, using the independent `voice_program.c` core and the same
voice objects/program values. See [native audio updates](native_audio_update.md).
Samples, mixing/output, asset loading and full runtime composition remain
pending. The voice fields are shared rather than mirrored in another state.

Run `python tools/recomp/check_native_command_effects.py`. The validator seals
the original state and every reachable instruction byte, then executes the
complete original closures, including divide, RNG and all sound children.
No child is replaced with a contract. There are 12,288 calls over six entries,
covering **278/278** instruction boundaries. Full events, ordered channel/mask
acknowledgements and RAM at each acknowledgement match. Every Chip/Slow RAM
byte is compared except the fixture's original CPU ABI stack
`$C7FD00..$C7FF00`; native C has its own ABI and does not recreate those writes.
The validation host applies acknowledgement payloads to the reference custom
register to compare the resulting interrupt state. Neither the stack nor
that hardware adapter exists in the native library.

The existing queue regression passes 73,728 calls, all 28 boundaries, 138 raw
and 256 translated destinations, with the new space owner independently
mapped. The existing parent regression passes 16,384 calls and retains its
explicit controlled effect contracts; the new standalone proof establishes
the real effects separately. Native contract tests also compose actual
status/message/sweep/voice owners with aircraft actions and publication,
check the hook's word swaps, signed mute gates, queue aliases and failure
to initialize without required data/services.

GNU strict warnings, GNU symbol inspection, MSVC native contracts and all
eight command CTests pass. Native `fa18_port` builds and its unchanged guard
passes 430 files. The library links into that bounded runtime but its game
loop still does not call it. The playable reference remains emulated; the
complete emulation-free objective remains unfinished. See
`analysis/figures/native_command_effects_checkpoint.json` for exact proof scope.
