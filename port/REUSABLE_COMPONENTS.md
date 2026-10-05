# Components for future ports

Keep reusable mechanisms separate from a game's original rules and data.
Extraction should follow a proven need in this port: the native game must
keep using the component, and its original-behavior proof must keep passing.
Game-specific adapters retain policy, asset interpretation and scheduling.

| Component | Reusable interface | Current limits |
| --- | --- | --- |
| `field_bytes.h` | Header-only byte views of caller-owned signed/unsigned 8/16/32-bit values and logical words. Whole-word fills call a logical owner's setter once. Shared by command publication and startup clears. | Requires stable canonical owners and valid shifts/pairs. Value accessors belong to each game's adapter; no original-address lookup, packed RAM, data import, record layout or scheduling is supplied. |
| `voice_program.c/.h` | Portable `PortVoice` state, named sound operations, waits/loops, slide updates and a program-ended callback. Builds with only the C standard library. | Uses unsigned 32-bit values and eight-byte program cursor units. It provides tick execution, not sample mixing or a device driver. |
| `voice_selection.c/.h` | Caller-owned sound tables and channel slots, ordered release/publication and acknowledgements. Shared by native command sounds and menu sounds. Builds with the C standard library and `voice_program.h`. | Accepts already resolved voices and fixed-point volumes. It has no asset loader, game sound IDs, scheduler or sample output. |
| `amiga/rgb4.c/.h` | Ordinary-buffer colour-map loading and CopIns/merged-list palette writes. Builds with only the C standard library. Both native callback and packed compatibility service use it. | Implements the existing host's RGB4 semantics and list data formats. Allocation, viewport construction, input scheduling and presentation remain caller-owned. |
| `amiga/viewport_list.c/.h` | Viewport record construction, palette records, MOVE/WAIT merging and an owned native list pair that binds to RGB4. Packed and native callers share the core. | Uses the accepted host's list format and viewport rules. Actual plane buffers, game setup, allocation and presentation remain caller-owned; there is no CPU/Copper execution. |
| `amiga/ofs.h`, implementation in `disk.c` | `amiga_ofs_*` reads, file lookup, metadata, bounded range reads and directory scanning from OFS images. | Read-only OFS; DOS locks, volume prefixes and OS service behavior remain adapters. Implementation also contains the F/A-18 wrappers. |
| `amiga/hunk.h`, implementation in `hunk.c` | `amiga_hunks_*` parses CODE/DATA/BSS, relocation records and pointer targets without executing original code. | Supports the loader's implemented HUNK types; native imports must still interpret each game's data. Implementation also contains the F/A-18 wrappers. |

The first new independent build target is `port_voice_program`. To use its
core elsewhere, compile `voice_program.c` and include `voice_program.h`.
An asset adapter supplies operation/value arrays and a caller-owned voice;
the host schedules ticks and receives the end callback. There are no F/A-18
globals, CPU registers, bus accesses, Kickstart requirements or SDL dependencies.

The independent `port_field_bytes` interface target now shares the actual
byte-of-value mechanism used by both command publication and startup clearing.
Include `field_bytes.h`; it needs no linked object. Reads/writes preserve the
other bits of a supplied integer, including signed words/longs, with explicit
errors for missing or ambiguous owners and invalid shifts. Ordered fills
retain preceding writes on failure. Each future game supplies its own field
views, clear extents and semantic record bindings. The F/A-18 queue regression
retains all 73,728 comparisons, and both complete startup leaves add 8,192
comparisons at 21 original boundaries with no RAM exclusions/child contracts.
See `../analysis/routines/native_startup_ranges.md` for ownership limits.

Logical word owners now let a game's resolved record reference participate
in the same bounded field view. Supply a `PortFieldWordValue` getter/setter
and its high/low byte views; `port_fill_field_words` writes the whole value
once without reading or exposing an intermediate identity. Primitive owners
still use ordered high/low writes with no observer between them. This adds
no concurrency guarantee. F/A-18's separate `viewed_record_word` adapter
maps its sixteen actual aircraft objects to the original 512-byte offsets;
that stride, record family and accepted domain stay game-specific. Its full
bootstrap-parent proof covers 16,384 calls at all 291 boundaries, with three
explicit pending child contracts. See `../analysis/routines/native_scene_bootstrap.md`.

The same field component now binds actual adjacent owners for the complete
F/A-18 template gate builder. Signed indices can modify earlier/later rows,
other axis buffers and supplied surrounding fields, with no duplicate packed
owner. Bootstrap and native game initialization share that one builder and
the existing terrain-consumer buffer format. Its 128-row geometry, source
error codes and count rules remain F/A-18 policy; the generic field mechanism
supplies none of them. Original Hunk-66 streams stay live caller-owned data.

The independent `port_voice_selection` target supplies release/selection for
those same `PortVoice` objects. Compile `voice_selection.c` with its header;
there is no link dependency on the tick executor. It clears a slot before
acknowledging, rereads the selected sound after that callback, then sets the
volume and publishes/acknowledges the voice. Empty sounds do no work; a voice
removed by the callback returns an explicit error after the release. F/A-18's
`audio_selection` adapter owns sound IDs, flags, volume conversion and channel
masks. Both command effects and the complete menu pair now use the core.
The original-instruction proof covers all 110 boundaries over 8,192 calls;
see `../analysis/routines/native_audio_selection.md` for scope and limits.

The F/A-18 adapter is `audio_update.c/.h`. It decodes the original sound-program
selectors, applies the game's signed period/master-volume rules, dispatches
the actual four channel descriptors and retains output/acknowledgement order.
Command effects share the core voice objects and mutable program values;
their older type names are aliases, not duplicated state. Another game must
supply its own adapter wherever its original behavior differs.

The independent `amiga_rgb4` target accepts caller-owned palette/list buffers
and an actual merged-list writer; it has no guest-address or machine dependency.
`input_palette.c/.h` supplies F/A-18's sixteen-word callback adapter and uses
the viewport's fixed ColorMap with its currently published DspIns list. The
packed compatibility adapter resolves guest buffers separately. Run
`python tools/amiga/check_rgb4.py` for the 16,384-case frozen-service comparison
and `python tools/recomp/check_native_input_callback.py` for both complete
callback comparisons. See `../analysis/routines/native_rgb4.md` for scope.

`amiga_viewport_list` now constructs the lists consumed by those services.
Its `AmigaNativeViewportLists` owns actual record/merged buffers and RGB4
descriptors; a caller supplies source geometry, plane identities/offsets and
palette data. It generates no scene or game timing. Run
`python tools/amiga/check_viewport_list.py` for 32,768 frozen-service
construction/merge comparisons. The native integration contract uses real
renderer buffer offsets and presents the constructed view; see
`../analysis/routines/native_viewport_list.md`.

The voice-program core returns specific invalid-argument/program results; it does not add
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

`outer_display.c/.h` now uses that same native page/pair/mode owner for the
complete outer display update. Its static/dynamic 32-word palette loop,
activity decrements and page toggle are F/A-18 policy. The shared RGB4 service
supports both this owner's 32-word loads and the input callback's 16-word
loads. Synchronization/presentation stay with the embedding runtime; see
`../analysis/routines/native_outer_display.md` for the source proof and limits.

`graphics_setup`/`graphics_storage` now compose those independent list/RGB4
mechanisms with this game's actual five-plane allocation and later shared
four-plane display. `display_palette_assets` imports the original ILBM CMAP
and Hunk palette data. This construction order, plane topology, table layout
and palette selection are game policy. Keep them in the F/A-18 adapter;
future ports can reuse the ordinary-buffer cores with their own data owners.
See `../analysis/routines/native_graphics_setup.md` for proof and remaining
startup/scheduling work.

`postflight_text` now binds the real mutable checksum descriptor and seeds the
same display owner used by those cores. Its checksum sentinels, callback
selection and unusual hex-field offsets are F/A-18 policy. `stage_callback.h`
consolidates this game's controller identities so the native owners can be
composed without conflicting types. The palette asset owner retains one raw
source-order bank for both its mode views and 32-word seed; no copied seed
needs synchronization. Full bootstrap and scheduling remain required; see
`../analysis/routines/native_postflight_text.md`.

`renderer_clear` and `post_input_display_stages` now share the existing plane,
command, viewport and controller owners instead of introducing new state to
synchronize. Their ten-stream topology, live fifth flag and countdown/callback
rules remain F/A-18 policy, so they are adapters rather than generic utilities.
Future ports can reuse the underlying ordinary-buffer graphics cores without
adopting these game rules. The complete original-instruction proof covers
62 boundaries over 16,384 calls; see
`../analysis/routines/native_post_input_display.md` for remaining dependencies.

`native_scene_records` now adds a third concrete caller of `field_bytes`.
Its views read/write the actual native command/geometry fields, with no
duplicate packed owner to synchronize. Record counts, prefix clear extents
and player setup remain F/A-18 policy; they belong in the adapter. Other
ports can use the typed byte mechanism with their own layouts and owners.
See `../analysis/routines/native_scene_player.md` for proof and limits.

`field_window.h` builds on `field_bytes` with a signed logical origin inside
immutable source data. Reads outside the source object require explicit
adjacent field owners, so another port can bind tables whose 16-bit indexing
wraps without adding a guest address space or hidden padding. F/A-18's scene
placement uses it for pose, grid and relocated descriptor assets; those Hunk
numbers, offsets and gameplay rules remain in `native_scene_placement`.

Planar rendering, projection and input modules remain candidates, with
game-specific dimensions, tables or state still present. They should be
extracted when another concrete caller establishes the common contract.
