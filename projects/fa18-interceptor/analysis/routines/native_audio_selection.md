# Native sound selection and menu audio

`port/audio_selection.c/.h` implements the complete `$C17B96` menu sound pair
and its actual `$C17B2C/$C17B08/$C0F4A6/$C4FFB0` selection/release children.
It uses the existing `FA18CommandAudio` slots, voice objects, fading gate,
effect flags and imported channel masks. The second sound-enable byte now
has its own `sound_flags` field in that same owner. No CPU, bus or ROM is
used by these native functions.

The `$C0F812` text initializer's `$C17B96($32)` call starts menu audio. Its
historical reference hook name `PM_DELAY_TEXT` does not describe the actual
child and must not be implemented as a wait.

The menu owner returns immediately for any nonzero fading byte. Otherwise
bit seven of `sound_flags` takes priority: release all four channels, select
sounds 13 and 14 on channels zero and one at volume 63, then set fading to 2.
Without that bit, bit two of `effect_flags` selects sounds 35 and 36 at the
caller's volume without the preliminary release of all channels. The last
branch releases all four channels and leaves fading untouched. An empty
sound entry skips selection, but a successful pair branch still writes 2.
Volume conversion preserves the original unsigned 32-bit shift wrap.

`port/voice_selection.c/.h` is the independent mechanism used by both this
owner and the existing command status/sweep/release effects. It accepts
ordinary sound-table pointers, channel slots, a fixed-point volume and an
acknowledgement callback. Release clears a slot before acknowledging, even
when it is already empty. Selection checks the sound entry, releases the
channel, **rereads the entry**, stores volume, publishes it, and acknowledges
again. This keeps shared table changes visible rather than retaining a stale
voice. Slots can alias sound-table entries; removal after release reports a
missing voice and preserves preceding writes. Original code would dereference
an invalid address in that case. Invalid bounds or required services also
return an explicit error; no voice or program is fabricated.

The core builds with the C standard library and `voice_program.h` alone.
It does not select game sounds, import assets, schedule ticks, synthesize
samples or issue hardware register writes. F/A-18 keeps its sound IDs,
flag rules, volume shift and channel-mask adapter in `audio_selection`.
Existing command effects now use this adapter and the same portable core.
Their program patches, event results and three-acknowledgement ordering
remain proven against the original instructions.

Run `python tools/recomp/check_native_audio_selection.py`. Its two entries
execute 4,096 calls each, covering **110/110** original boundaries. All actual
selection, release and interrupt-acknowledgement instructions execute; no
child is replaced with a contract. Comparison includes every Chip/Slow RAM
byte except the original CPU ABI stack `$C7FD00..$C7FF00`, full RAM at each
acknowledgement, ordered channel/mask payloads and final interrupt/custom
register state. The production component has neither that stack nor that
machine adapter.

Fixtures exercise all 37 supplied sound indices, every channel, empty and
aliased voices, both sound-enable bytes, fading and signed/high-word volume
extremes. Supplied host effects change the sound table, fading, enable flags
and a subsequent channel mask **after** an acknowledgement in both runs.
The preceding boundary state is compared before applying those effects;
no captured output is copied into native state. There are 17,792 matched
acknowledgements in the menu run and 6,130 in the direct selector run.

The command-effects regression passes 12,288 calls at all 278 boundaries;
the audio-update regression passes 16,384 calls at all 105 boundaries.
Standalone GNU strict-warning tests, symbol inspection, native MSVC build
and four focused CTests pass. The integration contract selects actual native
voices and feeds those same objects to the native period/volume update.
The unchanged native guard passes 458 files. Exact hashes and proof scope
are in `analysis/figures/native_audio_selection_checkpoint.json`.

Original sound-table/program/sample loading, native sample output and full
startup/tick composition remain open. The bounded native main still does
not call the new graph; the playable reference still uses emulation. Next
complete the `$C0F812` publisher, actual hexadecimal formatter and `$C08F26`
bootstrap with shared native state and the original palette seed/loading
order. Do not count a palette-copy fragment as the complete publisher.
