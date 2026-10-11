# Native planar display adapter

This is a port display-boundary contract, not a reconstruction of an original
routine. The observed Copper display pages are four contiguous 8,000-byte
planes, 40 bytes per row and 200 rows. In the attract cockpit snapshot, the
page starts are `$012BC0`, `$014B00`, `$016A40`, and `$018980`; see
`../cockpit_bitplane_assets.md`. Plane one supplies the low bit and plane four
the high bit of the visible four-bit index.

`FA18PlanarPage` in `port/display.h` holds those four planes without a
Chip-RAM address or Copper state. `fa18_decode_planar_page` converts its
MSB-first bytes to `FA18IndexedFrameBuffer`, the port's one-byte-per-pixel
native render target. `FA18Palette` holds RGB4 colour words as palette state,
and `fa18_apply_palette` converts indices to the RGB444 presentation format.

The contract test checks the plane-bit order, byte/row boundary, invalid index
rejection, and observed map palette words `$0D92` and `$0036`. It does not
claim that the captured planes are immutable artwork, identify their producer,
or implement Copper timing/page selection.
