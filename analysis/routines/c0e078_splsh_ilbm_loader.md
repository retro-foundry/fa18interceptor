# `$C0E078` `pix/splsh` loader boundary

Classification: **bounded original startup-resource decoder**.

`$C0E2E8-$C0E2F6` pushes `df0:pix/splsh`, calls `$C0E078`, and stores the
returned runtime object in `$C1AADC`. The asset inventory proves that file is
a `FORM ILBM`, 320 by 200 pixels, five planes, ByteRun1 compressed, with a
32-entry CMAP; its body and RGB4 palette are original ADF data. The later
`$C1693A-$C1697E` consumer copies the five object plane fields into the
separate `$C1AAF4-$C1AB04` cache.

`ilbm_page_loader.{c,h}` ports this bounded asset-decode side into a distinct
native `FA18FivePlanePage`. It accepts only the established `splsh` geometry,
masking mode, ByteRun1 compression, and 32-colour CMAP; malformed or
incomplete source data fails. `game.c` loads it directly from the supplied
ADF at initialization. The page is intentionally not attached to the normal
presentation path: the evidence does not assign the `$C0E078` result as the
run075 flight page, nor does it yet recover its View/Copper selector.

The contract uses a synthetic source-shaped ILBM only to verify the decoder's
format boundary. It contains no emulator frame, Chip-RAM page, or recorded
pixel payload.
