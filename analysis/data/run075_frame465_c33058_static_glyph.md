# run075 frame 465 `$C33058` static glyph submission

The run075 playback reaches `$C33058` at Engine frame 465, the first
post-scene frame after the frame-462 renderer boundary. The entry registers
are `D3=$FFFFC000`, `D4=$00C30032`, `D5=$0000041A`, `D6=$000001C2`, and
`D7=$000001C2`; the enclosing routine resolves the source pointer to
`$C3DAB7`. The seven source bytes consumed by `$C330FE` are:

```text
F0 90 90 F8 C8 C8 C8
```

The four destination longs are read from the display lane table at
`$C4567E`: `$18980`, `$16A40`, `$14B00`, and `$12BC0`. Adding `D5` gives a
common semantic page-relative offset `$041A`. `D6` selects set mode for the
outer and innermost lanes (planes 4 and 1) and clear mode for planes 3 and 2;
the mask word is `$C000`.

This is represented by `FA18StaticGlyphSubmission` and checked by
`fa18_glyph_contract` against the before and after display words captured
from `build/run075_frame464_c33058_trace`. The trace also confirms that the
current native function is the four-lane `$C33058` caller contract; the table
lookup and layout cursor update after the lane writes remain separate caller
work.

The next changed text boundary is Engine frame 467. It advances the source
entry and destination by two bytes, uses source bytes `C8 C8 C8 C8 D8 50 70`,
mask `$A000`, and the same plane mode mask `$09`. The same contract now checks
that second packet as well, preserving the observed per-frame cursor advance.

The following boundary is Engine frame 469. Its source bytes are
`F8 80 80 F0 C0 C0 F8`, the relative destination is `$041E`, and the mask is
`$1000`; the plane mode mask remains `$09`. The captured four-plane words are
also checked, confirming that the lane writer is reusable across successive
text positions while the caller advances the source and destination cursors.

Engine frame 471 retains the `$041E` destination and changes the source bytes
to `F8 88 80 C0 C0 C8 F8`, with mask `$8000` and mode mask `$09`. Its exact
four-plane result is checked as the next state in the same compositor sequence.

Engine frame 473 retains the destination again, resolves source bytes
`F8 20 20 30 30 30 30`, and supplies mask `$F000`. The live native gate now
executes this packet through the semantic page conversion and four-lane
compositor; the full replay verifier confirms its RGB444 result.

Engine frame 475 advances the destination to `$0420`, resolves source bytes
`F8 98 88 88 88 88 F8`, and supplies mask `$6000`. This packet is also routed
through the native compositor in the live gate.

Engine frame 477 keeps `$0420`, resolves source bytes `F0 90 90 F8 C8 C8 C8`,
and supplies mask `$D000`. The live gate applies this packet through the same
semantic four-plane operation.

Engine frame 479 advances the destination to `$0422`, resolves source bytes
`F0 90 10 78 18 98 F8`, and supplies mask `$B000`. This packet is also live
in the native gate.

Engine frame 481 advances the destination to `$0424`, resolves source bytes
`F8 80 80 F8 18 98 F8`, and supplies mask `$2000`. This packet is live in the
same native compositor path.

Engine frame 483 retains `$0424`, resolves source bytes `F8 88 88 98 98 98 F8`,
and supplies mask `$9000`. This packet is also executed by the native gate.

Engine frame 485 advances the destination to `$0426`, resolves source bytes
`F8 80 80 F0 C0 C0 C0`, and supplies mask `$7000`. This packet is live in the
same native compositor path.

Engine frame 487 retains `$0426`, resolves source bytes `F8 98 88 88 88 88 F8`,
and supplies mask `$E000`. This packet is live in the native gate as well.

Engine frame 489 advances the destination to `$0428`, resolves source bytes
`F0 90 90 F8 C8 C8 C8`, and supplies mask `$5000`. This packet is live in the
same native compositor path.

Engine frame 491 advances the destination to `$042A`, resolves source bytes
`F0 90 90 F8 C8 C8 F8`, and supplies mask `$3000`. This packet is live in the
same native compositor path.

Engine frame 493 retains `$042A`, resolves source bytes `F8 98 88 88 88 88 F8`,
and supplies mask `$A000`. This packet is live in the same native compositor
path.

Engine frame 495 advances the destination to `$042C`, resolves source bytes
`F8 88 80 D8 C8 C8 F8`, and supplies mask `$1000`. This packet is live in the
same native compositor path.
