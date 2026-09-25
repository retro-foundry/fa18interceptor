# Run060 qualification-text onset boundary

Authority: native deterministic replay frames in `build/run060_every5`, made
from sealed `captures/run060` with the canonical boot-restore protocol.  This
is a screen-output measurement, not an instruction trace or a qualification
predicate claim.

Run the measurement without altering the capture:

```powershell
python scripts/audit_colored_text_onset.py --frames build/run060_every5 `
  --from-frame 9000 --to-frame 9550
```

The script counts pixels whose RGB values satisfy `red > 150`, `green > 100`,
and `blue < 100`.  In this interval, that narrow range is absent from frames
9,000 through 9,285.  It first appears in the five-frame native sample at
frame 9,290 (28 pixels), then grows character-by-character: 154 at 9,295,
664 by 9,320, 1,780 by 9,390, and 3,658 by 9,525.  The sample remains at
3,658 through frame 9,545, which is the known fully drawn result frame.

Therefore the visible result text begins in the bounded interval
`(9,285, 9,290]` in Engine9000 GUI-frame numbering.  This refines the prior
full-message anchor but does **not** identify the result transition, test,
status writer, payload producer, or any causal input.  A direct-core trace
during this active character-draw interval is not a visual oracle, so future
causal work must begin with a native checkpoint before this boundary.
