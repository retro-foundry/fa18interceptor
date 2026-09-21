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

The initial comparison was against the older attract-style plane set at
`$012BC0` onward.  That set is inactive in run029.  The Copper-source rows in
the trace identify the active run029 list at `$057858`; it points at a separate
five-plane display set.  Those active planes change by 2,760, 3,870, 438, and
352 bytes respectively across the 993-to-994 boundary.  See
`analysis/run029_active_cockpit_bitplanes.md`.

The stepped trace still programs the earlier four-plane packet and ends before
the active five-plane presentation is reflected in its exported RAM.  It must
not be used to attribute `$C4597C` through `$C45984`, or any other
simultaneously changing state word, to KTS.  The next trace must follow the
producer of the active `$057858` Copper list and its five backing buffers.
