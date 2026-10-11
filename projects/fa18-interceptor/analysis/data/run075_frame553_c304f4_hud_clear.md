# run075 frame 553 `$C304F4` HUD transition blit

The canonical run075 replay reaches the direct blitter trigger `$C304F4` at
Engine frame 553. The entry registers are `D0=$0E14`, `D1=$0161A6`,
`D2=$0076EE`, `D3=1`, `D6=$0014`, `D7=$0E14`, `A0=$DFF000`,
`A2=$C4597E`, `A3=$C45970`, `A4=$0090`, and `A5=$C4BFE2`.

After one settling frame, 4,358 Chip bytes differ from the entry snapshot.
The changed region begins at Chip offset `$6E58` and is predominantly cleared
to zero, with the source snapshot containing the previous display-page data.
This is the buffer transition immediately before the large frame559 HUD text
delta; it is a blitter clear/copy boundary rather than a `$C2F688` pixel
submission. The native port should model it as a typed display-page operation
once the source and destination page roles are decoded.

The enclosing `$C304B2` setup is also captured in the same trace. It loads
`$C4596E` into the blitter size field (`$0E14`), sets `BLTCON0=$0D0C` and
`BLTCON1=$0002`, and derives the `$76EE` word from `$C45960` before writing the
custom-chip pointer fields. This establishes the operation shape, while the
original absolute pointer roles remain intentionally outside the C page model.
