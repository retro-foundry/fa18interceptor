# run029 final-cockpit numeric-renderer trace

## Authority

`build/run029_cockpit_numeric_trace/` replays sealed run029 normally through
frame 14,298, then instruction-steps chipset frame 14,299 with no future
recorded input.  It contains 7,252 instruction rows and 138 Custom-register
writes.  The final video hash is unchanged from the pre-trace frame:
`0198e648472461a1ca47615e439275d64a075a03bd32cbbfa9327a0ce3a133f6`.

## Accepted facts

- `$C0D730 -> $C2FD8C` executes once.  This is the already established active
  four-plane cockpit packet.
- Its four `BLTSIZE` triggers occur at `$C2FDF0`, `$C2FE3A`, `$C2FE90`, and
  `$C2FEDA`, with `A2 = $C4567E`, the live active-plane table.
- `$C30668-$C306AE` executes twice.  Its two jobs consume successive prepared
  records at `$C4B39A` and `$C4B3A0`.
- The frame does not execute `$C2EE4A`, `$C2FA7E`, or `$C2FB7A`; therefore it
  is not evidence for the separate observed projected-segment path.

## Result for cockpit numbers

This stable redraw cannot identify the writer/record for the visible KTS or FT
glyphs.  It is evidence that those glyph regions reside in the same mutable
four-plane display system, but the unchanged output makes every blit in this
frame compatible with copying pre-existing content.  No RAM field is named.

The next useful oracle must deliberately bracket a frame where a readable
numeric glyph changes, then compare the before/after Chip-RAM rectangle and
trace the corresponding blit source.  That experiment should retain normal
full-frame playback until the changed frame; only a no-future-input interval
may be instruction-stepped.

## First changed-number boundary

Adjacent normal-playback screenshots establish a real output transition:

| Frame | HUD speed | HUD altitude |
| ---: | ---: | ---: |
| 993 | `161 KTS` | `145 FT` |
| 994 | `171 KTS` | `145 FT` |

The ordinary frame-993 and frame-994 snapshots have identical bytes in the
four Copper-list plane ranges.  The frame-994 stepped trace also programs the
same active-plane and prepared-job blitters but ends with no changed bytes in
those ranges.  This is a timing/buffer-observability discrepancy, not evidence
that the visible speed change lacks a renderer.  Do not attribute `$C4597C`
through `$C45984`, or any other simultaneously changing state word, to KTS on
this result alone.

The required next experiment is a capture hook at the video-refresh boundary
that exports the exact displayed bitplane backing store (or a validated raster
readback) alongside the corresponding Chip-RAM snapshot.  That will tell
whether the bridge's post-frame exported map lags or differs from the video
surface before writer-PC tracing resumes.
