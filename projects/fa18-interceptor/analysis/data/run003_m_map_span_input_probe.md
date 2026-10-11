# Run003 M-map area-blit input probe

The `$C304F4` map-page operations are **descending area blits**, not line-mode
operations.  On OCS, `BLTCON1` bit zero selects line mode; the captured value
`$0002` instead selects descending address progression.  The 44 display-page
submissions are therefore the renderer's filled/masked primitive path.

`scripts/collect_run003_m_map_span_inputs.py` pauses immediately before each
`$C304F4` `BLTSIZE` trigger and captures the live blitter state plus Chip RAM.
For the first display-page operation, the exact pre-submit state is:

| field | value |
| --- | --- |
| `BLTCON0/BLTCON1` | `$0DFC` / `$0002` |
| `BLTSIZE` | `$23C7` |
| C/B/A/D pointers | `$0062A0` / `$0512DC` / `$0078B4` / `$0512DC` |
| A/B/C/D modulo | `$0028` / `$001B` / `$001B` / `$001B` |

The A and C inputs are in the mutable `$006000-$007FFF` renderer workspace;
the display destination is in the pending map page.  Thus the fill masks cannot
honestly be exported as a static coastline polygon list.  The probe gives the
next decoding step exact inputs for replaying the hardware operation and then
converting only the resulting operation spans to SVG paths.

The probe is deliberately ignored because it contains a 512 KiB Chip-RAM
snapshot per submission.  It is reproducible from the sealed run003 checkpoint:

```powershell
python scripts/collect_run003_m_map_span_inputs.py `
  --output build/run003_m_map_span_inputs
```

This is renderer-operation evidence, not a bitmap-to-vector conversion.
