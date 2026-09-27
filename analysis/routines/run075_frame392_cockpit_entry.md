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
the alternate page. The `$C07F00` object's exact Copper/palette ownership is
still unassigned, so this proves the page-staging presentation path without
claiming a decoded Copper list.

The adjacent `$C1AA9C` 32-word RGB4 palette bank remains byte-identical for
global frames 201--392. That excludes a mutation of this observed palette bank
as the frame-392 trigger; it does not exclude a distinct Copper-controlled
palette source until `$C07F00` is decoded.

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

## Next port boundary

Trace the display-state/Copper path selecting or revealing the five-plane
page at the frame-391/392 boundary, then model the first cockpit or scene
records as C structs. Do not add the flight model or advance the native frame
gate until the frame-392 pixels and their input/state cause are accounted for.
