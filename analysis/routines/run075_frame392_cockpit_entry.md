# run075 frame 392 cockpit-entry boundary

This note records the first visible change after the verified menu/demo clear
sequence. It is limited to runs 60 and above and uses the canonical run075
replay oracle.

## Pixel boundary

Frames 287 through 391 are identical to the all-black frame 286 page. Frame
392 changes 361 of the 64,000 RGB444 pixels. The changed-pixel bounding box is
native coordinates `x=7..318, y=101..199`. The pixels are sparse and dark;
this is the first visible cockpit or flight-scene construction boundary.

The native port must remain locked before this boundary until the calls that
produce these pixels are identified. A static frame fixture would not explain
the replay-driven transition.

## Runtime evidence

The replay reaches `$C2FD22` at Engine frame 291. The complete entry
register record is retained in `build/run075_c2fd22_trace/report.json`.

The first instructions load five display-plane pointers from `$C456BE`, then
clear four active plane streams with a `DBRA` loop. The routine therefore
clears the planar visual buffers; it is not the source of the frame-392
cockpit pixels. The existing native `fa18_renderer_clear_planar_words`
contract models this operation as a semantic display-buffer clear.

The trace was started from the canonical restored state with the recorded
run075 input events applied at their original frame boundaries. The bounded
instruction trace advances debugger frames while stepping, so it is used for
callee behavior and register evidence only; frame 392 pixel ownership still
requires a normal replay trace around the first visible change.

## Normal-frame renderer boundary

The corrected normal-replay PC profiles distinguish the two adjacent host
frames. They restore the canonical state after replay frame 200, deliver
recorded input by global frame label, and profile exactly one subsequent
frame:

```text
python scripts/profile_window.py --restore captures/run075/initial_state.bin \
  --playback captures/run075/playback.e9k --first-frame 391 --last-frame 391 \
  --frame-offset 200 --output build/run075_profile_391_offset
python scripts/profile_window.py --restore captures/run075/initial_state.bin \
  --playback captures/run075/playback.e9k --first-frame 392 --last-frame 392 \
  --frame-offset 200 --output build/run075_profile_392_offset
```

The frame-392 profile newly samples `$C27B32-$C27C9A` and `$C2F68A`, while
the frame-391-only profile instead includes `$C249…/$C24D…` and
`$C302…/$C306…` paths. The `$C27B…` range is the observed `$C279D0`
projection-to-polygon traversal: it transforms selected records, fills the
`$C4B392-$C4B39D` screen-pair buffer, and submits through `$C2FF48`.
`$C2F68A` is within the four-plane pixel primitive.

A return-bounded trace of that first `$C279D0` entry, with its breakpoint
armed at local frame 192 (global frame 392), reaches two `$C2FF48` entries
and five `$C2F688` entries before returning to `$C0F0C8` after 5,706
instructions:

```text
python scripts/trace_from_breakpoint.py \
  --restore captures/run075/initial_state.bin --playback captures/run075/playback.e9k \
  --playback-frame-offset 200 --address 0xC279D0 --arm-frame 192 \
  --return-pc 0xC0F0C8 --frames 200 --max-instructions 10000 \
  --ignore-future-input --output build/run075_frame392_c279d0_first
```

At that entry `$C456B6=$C4567E`, whose four plane bases are
`$018980,$016A40,$014B00,$012BC0`; it is not the separately fetched
five-plane `$04DB30,$04FA70,$0519B0,$0538F0,$055830` family. The normal DMA
captures for frames 391 and 392 fetch that latter five-plane family.

The selector's normal state provides the immediate staging timeline. Its
`$C456B6` byte changes from `$C4567E` to `$C4566E` at global frame 380, then
back to `$C4567E` at frame 389. `$C4566E` holds the four render-page bases
`$0538F0,$0519B0,$04FA70,$04DB30`, so the frame-380 selection can direct the
four-plane primitive to the page later fetched as five planes. A
frame-by-frame sample of the complete 40,000-byte display family records its
last mutation at frame 389 and none in frames 390--392. Thus frame 392 reveals
a page prepared earlier; the `$C279D0` work newly observed in frame 392 is
directed at the other page. The page-selection/palette/Copper condition that
makes the already prepared page visible remains open. In particular, neither
the staged record stream nor the first visible scene may be named terrain from
this evidence.

The frame-389 return-bounded `$C1612C -> $C15DB2` trace supplies the next
display-state edge. It first calls `WaitBOVP`, copies table entry zero
`$C074D8` into `$C1821C` and `$C07F00` into `$C18232`, invokes `LoadView`,
then changes `$C4566C` from zero to one in its tail. `$C18232` is
`$C1822A + 8`, the proved graphics `ViewPort` display-instruction field.
Therefore this outer-loop child publishes a selected display instruction and
waits for the graphics display boundary before the next loop begins rendering
the alternate page.

The frame-389 final Slow-RAM snapshot now identifies `$C07F00` structurally as
the first `struct CopList` referenced by that `ViewPort`'s `DspIns` field. Its
Kickstart-1.3 layout has `Next=$C555F8`, `_ViewPort=$C1822A`, and
`CopLStart=$0577B0`; `$C555F8` in turn has `Next=$C55680` and
`CopLStart=$057880`, while `$C55680` ends the chain with
`CopLStart=$057888`. The merged Chip-RAM stream loads BPL1--BPL5 as
`$04DB30,$04FA70,$0519B0,$0538F0,$055830`. Its wait at `$2A01` is followed by
`BPLCON0=$5200`, and the `$F201` wait restores `$0200`; therefore the selected
display instruction presents that five-plane family over the Copper interval
from vertical position `$2A` until `$F2`. This is a structural Copper/display
join, not a claim about scene identity or the still-unported native scheduler.

The adjacent `$C1AA9C` 32-word RGB4 palette bank remains byte-identical for
global frames 201--392. That excludes a mutation of this observed palette bank
as the frame-392 trigger. The `$C07F00` decode proves its plane pointers and
BPLCON0 interval, but does not yet identify a distinct Copper-controlled
palette source.

## Copper palette and page exactness

A normal full-frame replay through global frame 392 now resolves that remaining
palette source. `build/run075_global392_normal/normal_custom_writes.jsonl`
records the published Copper prefix at `$0577B4`: at vertical position 40 it
writes `COLOR00=$000`, `COLOR01=$100`, `COLOR02=$111`, and the remaining
observed colour registers (including `COLOR09=$620`). These are dynamic words
in the `$0577B0` list, not the unchanged `$C1AA9C` RGB4 bank.

`python scripts/compare_run075_frame392_copper_page.py --chip
build/run075_global392_normal/chip.bin --report
build/run075_global392_copper_page_decode.json` deplanarizes the five proved
buffers and those Copper palette moves. It has zero mismatches against all
64,000 RGB444 pixels in oracle frame 392: 63,639 are `$000`, 193 are `$100`,
and 168 are `$111`. The prior frame-389 return snapshot retains zero for
`COLOR01/COLOR02`; the identical five-plane decode is all black and differs by
exactly those 361 pixels. This proves that the visible boundary is a dynamic
Copper-palette update on an already prepared page. It remains a captured
display oracle, not native page data or a complete scene producer.

An ordinary-replay CPU-write watchpoint over `$0577B0-$0577BF`, armed at
global frame 392, stops on its first access at `$0577B6`. The interrupted
instruction is `MOVE.W (A1),(A0)+` at `$FCFF32`, with `A0=$0577B6` and
`A1=$C085F0`; its call context retains the Copper-list base `$0577B0` and
Custom `COLOR00` base `$DFF180`. `$C085F0` begins the live RGB4 words
`$000,$100,$111,...,$620`, matching the published Copper values. The
watchpoint reports the leaf copying routine, not a game-level owner, so it
does not identify a scene or schedule. It does establish the source-to-list
palette-load boundary independently of the frame oracle.

The saved stack and the byte-exact `$C1718E` callback identify the owner:
`$C53EC0` loads `A0/A1/D0` then invokes graphics.library `-$C0`, which the
Kickstart 1.3 ABI names `LoadRGB4(ViewPort, colours, count)`. At this frame
`$C458A0=8`; `$C1718E` computes `$C08510 + (15 - 8) * 32 = $C085F0` and
passes `count=16`. Thus this is a 16-word Hunk-21 RGB4 mode table applied to
`COLOR00..15`, not a 32-word palette write; higher Copper colours retain their
separate state. A run075 frame-201..392 CPU write watch finds the Hunk source
unchanged.

`fa18_update_copper_palette_moves` models the bounded list-write operation
over caller-owned mutable Copper streams. `fa18_load_viewport_mode_palette`
now reproduces the exact Hunk-21 `$C08510` table stride and
`fa18_load_viewport_mode_palette_into_copper` applies only the first 16
registers. Neither routine supplies a palette, page, frame gate, or scene
producer; the contracts use synthetic values and retain the source values only
as replay evidence.

The source-bounded `$C1718E` mode tail is now also represented by
`fa18_advance_viewport_mode`. It preserves the signed-byte countdown and
one-step current/target transition; on an eligible step it loads the selected
16-colour table, publishes the proved `(1 - $C4566C)` pointer-table pair,
performs the source's second identical lower-16-colour load, and republishes
that pair. When the current
mode reaches target, it performs the exact 16-word copy
to a required caller-owned destination and stores state `3`. The equal-mode
nonzero-state branch reloads the palette without republishing pointers. The
callback schedule, pointer-table contents, destination ownership, and page
producer remain separate native owners rather than inferred state.

`scripts/sample_run075_viewport_mode.py` samples the direct state and Copper
words once per ordinary replay frame. In run075, `$C0FA04`'s expired path
returns from `$C0FAA4` and then writes `$C458A1=15` and `$C458A0=0` at global
frame 370. The sampled state then permits one mode increment per three observed
updates, making the live current mode eight at frame 392. A live `$C1718E`
breakpoint counter records exactly one entry in each run075 replay frame 201--420;
at frame 413 its entry state is current 14/target 15/countdown 0. Its terminal
write is independently CPU-watchpointed at `$C1731A`: `MOVE.B #3,$C458A4`
changes the byte from zero to three. The idle `$C1617E` outer-loop child then
consumes that shared state before the following callback entry, which observes
state 2. This is a traced
scheduler cadence, not permission to replace it with a frame-number condition.
`post_input_followup`
models only those direct `$C0FA04` stores after a caller-owned scene-initializer
callback, preserving their observed order. Its paired `$C0FA4C` direct gate
clears its separate auxiliary byte after countdown expiry and advances only
when the same typed current/target bytes match; no replay-frame condition is
used.

`outer_loop_child` ports the matching idle branch of `$C1617E-$C16283`: it
decrements the same state byte, conditionally performs its caller-owned
`WaitBOVP`/32-word `LoadRGB4` sequence, and retains the word-sized outer-index
toggle. The nonzero activity-counter loop remains a separate unported branch.
`viewport_palette` now also models the shared 32-word `$C45660` buffer: the
initial `$C0F812` copy seeds all words from Hunk-21 `$C08510`, and the terminal
mode copy replaces only words 0--15. This is the exact source of the buffer
provided to the outer-child 32-word palette operation; no captured palette is
embedded.

`five_plane_page` is the native owner for one proved `$5200` five-plane page:
it owns five separate 8,000-byte buffers, exports the lower four to the
established planar-pixel adapters, and exposes a count-preserving `LoadRGB4`
palette callback. Its display state is presented through the existing Copper
page decoder contract with private native pointer identities. The
`viewport_transition_page` contract proves the terminal 16-word mode copy and
the following 32-word outer-child load reach that same page palette. It does
not claim that the native runtime yet produces the page's scene pixels.

## Prepared-page producer sample

Arming `$C279D0` at local frame 182 first reaches it at local frame 184
(global frame 384), inside the bounded render-page preparation interval. This
return-bounded invocation uses `$C456B6=$C4566E`, the `$04DB30` render-page
pointer family, rather than the `$012BC0` family used by the frame-392
invocation. It reaches two `$C2FF48` submissions and five `$C2F688` pixel
entries before returning to `$C0F0C8` after 4,593 instructions. The direct
pixel entries are `(140,105)`, `(52,106)`, `(297,106)`, `(231,109)`, and
`(101,101)`; all lie in the first visible frame's `y=101..199` bounds.

```text
python scripts/trace_from_breakpoint.py \
  --restore captures/run075/initial_state.bin --playback captures/run075/playback.e9k \
  --playback-frame-offset 200 --address 0xC279D0 --arm-frame 182 \
  --return-pc 0xC0F0C8 --frames 185 --max-instructions 10000 \
  --ignore-future-input --output build/run075_frame382_c279d0_render_page
```

This directly identifies `$C279D0` as one producer of the page later shown at
frame 392, even though its frame-392 invocation has already moved to the
alternate page. The polygon contexts and source records remain unclassified:
these coordinates are not sufficient to call the staged image terrain,
cockpit art, or a complete scene.

The frame-384 entry carries `A5=$C3B5B6`. That same static context is observed
in an independent demonstration-flight descriptor-to-polygon path and in
separated M-map captures, where it is explicitly retained as a repeated
component of unknown world semantics. The current `$C279D0` traversal does
not join that context to its mutable `$C28128` record stream, so it is
corroborating renderer context—not a basis for naming the first page terrain.

## Next port boundary

Trace the display-state/Copper path selecting or revealing the five-plane
page at the frame-391/392 boundary, then model the first cockpit or scene
records as C structs. Do not add the flight model or advance the native frame
gate until the frame-392 pixels and their input/state cause are accounted for.
