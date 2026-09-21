# `blitter_draw_line_to_enabled_planes` at `$C2FA7E`

Classification: **behavioural, hardware-level**. Authority is
`build/attract_focus_600/trace.jsonl`, `custom_writes.jsonl`, the matching slow
RAM snapshot, and the raw P-code export.

The routine is called at `$C2FA7E` in the in-flight packet and returns at
`$C2FD20`. It derives a row-relative pointer from register inputs, constructs
the Amiga blitter line-mode control values, waits for `DMACONR` bit 6 to clear,
then conditionally starts the same line operation for bits 0-3 of the byte at
`$C456E7`.

For each enabled bit it writes:

| Register | Observed value source |
| --- | --- |
| `BLTCON0` | computed control word |
| `BLTCON1` | computed line-mode control word |
| `BLTCPT`, `BLTDPT` | the same computed buffer address |
| `BLTADAT` | `$8000` |
| `BLTBMOD` | computed error term |
| `BLTSIZE` | computed line trigger |

The code also establishes `BLTAMOD`, `BLTDMOD`, `BLTCMOD`, full A masks and
`BLTBDAT` before the enabled-plane sequence. Repeated writes from `$C2FBD4`,
`$C2FBD8`, `$C2FC3C`, `$C2FC40`, `$C2FCA4`, `$C2FCA8`, `$C2FD0A` and
`$C2FD0E` match these operations in the custom log.

The byte at `$C456E7` is therefore provisionally named
`active_line_plane_mask`; its wider structure, the input-register calling
convention, the coordinate system and the visual primitive's owner remain
unknown. Do not call it an aircraft, terrain, HUD or polygon routine yet.

## Byte-exact source boundary

The complete body through its return at `$C2FD20` is now represented by three
contiguous byte-exact source slices:

- `prepare_blitter_line_parameters.asm`, `$C2FA7E-$C2FB4D`: CPU coordinate,
  direction, Bresenham, and trigger preparation.
- `blitter_line_setup.asm`, `$C2FB4E-$C2FB79`: initial blitter idle wait and
  shared modulo/mask setup.
- `submit_blitter_line_to_active_planes.asm`, `$C2FB7A-$C2FD21`: conditional
  bitplane 0--3 submissions and per-trigger busy waits.

The first slice's `$C2FA70` branch is now byte-exact
`handle_equal_line_axis.asm` (`$C2FA70-$C2FA77`). It increments both axis
registers, clears the line span word, and rejoins at `$C2FA9C`.
