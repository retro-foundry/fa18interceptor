# run029 active cockpit bitplanes

## Authority

The normal replay snapshots at frames 993 and 994 are the authority for the
visible `161 KTS` to `171 KTS` change.  Copper-source rows in
`build/run029_cockpit_numeric_change_0994/custom_writes.jsonl` execute from
`$057834-$057848`, identifying the live Copper list that supplies the display
window immediately before the five-plane section.

At `$057858`, both snapshots contain:

| Plane | Copper pointer | Range (8,000 bytes) |
| ---: | ---: | --- |
| 1 | `$04DB30` | `$04DB30-$04FA6F` |
| 2 | `$04FA70` | `$04FA70-$0519AF` |
| 3 | `$0519B0` | `$0519B0-$0538EF` |
| 4 | `$0538F0` | `$0538F0-$05582F` |
| 5 | `$055830` | `$055830-$05776F` |

`BPLCON0=$5200` at `$057884` enables that five-plane section after the Copper
wait at `$057880`.  This is a run029 runtime list, distinct from the
four-plane attract list at `$00A400` documented elsewhere.

Across ordinary snapshots, the renderer-table publisher changes `$C456B6`
from `$C4567E` at frame 992 to `$C4566E` at frame 993. A return-bounded normal
frame-993 trace proves `$C2F558` performs that publication; `$C4566E` contains
the four-plane subset `$0538F0,$0519B0,$04FA70,$04DB30`. This connects the
normal renderer's table selection to the Copper-visible family, while leaving
the individual glyph producer unassigned. See
`analysis/routines/c2f558_outer_pointer_selector.md`.

## Changed-number evidence

Across normal frames 993 to 994, the five displayed planes change by:

| Plane | Changed bytes |
| ---: | ---: |
| 1 | 2,760 |
| 2 | 3,870 |
| 3 | 438 |
| 4 | 352 |
| 5 | 0 |

Thus the visible numeric transition is backed by real changes in the active
run029 Chip-RAM planes.  The broad per-frame redraw means this result alone
does not isolate the glyph rectangle or identify its source record.  It does
reject the earlier inactive-buffer explanation and establishes the exact
buffers that the next renderer trace must watch.

`scripts/analyze_cockpit_bitplanes.py` now also compares all active planes as
one composite and reports four-connected changed-pixel components.  For this
boundary it finds one redraw component spanning `$x=0..319`, `$y=1..144`, plus
small isolated components.  The 7-pixel component at `$x=106..108`,
`$y=92..96` and adjacent 6-pixel component at `$x=102..103`, `$y=92..96`
overlap the left speed-readout glyph region in the composite inspection image.
They are useful coordinates for a future source/destination trace, but are
not by themselves an attribution of a particular digit: the giant component
still reaches the same display rows.

## Renderer handoff at the changed-number frame

The no-input stepped interval for chipset frame 994 records
`$C0D730 -> $C2FD8C`.  Its four `BLTSIZE` triggers use these live destination
values:

| Trigger PC | Blitter destination | Corresponding active plane |
| ---: | ---: | ---: |
| `$C2FDF0` | `$053918` | plane 4 base `$0538F0` plus `$28` |
| `$C2FE3A` | `$0519D8` | plane 3 base `$0519B0` plus `$28` |
| `$C2FE90` | `$04FA98` | plane 2 base `$04FA70` plus `$28` |
| `$C2FEDA` | `$04DB58` | plane 1 base `$04DB30` plus `$28` |

The same interval executes `$C30668-$C306AE` twice; its prepared jobs use
`$04DB58` and `$04DB7F`.  A CPU watch of `$04DB30` through normal replay does
not hit, consistent with these data changes being blitter DMA rather than CPU
stores.  This does not identify the numeric source record, but it proves the
changed HUD is produced through the active-plane blitter pipeline rather than
the inactive attract buffers.
