# `$C32A44-$C32ACF`: packed-value transform setup entries

Classification: **static formatter/render-submission dataflow**.

Three byte-exact source slices cover this contiguous set of entries while
preserving the pre-existing `$C32AA4-$C32AB3` renderer-offset entry:

- `prepare_postflight_packed_transform_value.asm` covers `$C32A44-$C32A95`.
  It sign-extends incoming `D2`, stores it at `$C45B1E`, calls the established
  `$C25A08` packed-BCD converter, prepares a backward destination within
  `$C457FA`, derives table/pointer values from `D0`, `D1`, and `D3`, and enters
  `$C32AD0` with `D4=1`.
- `initialize_postflight_transform_render_offsets.asm` covers
  `$C32A96-$C32AA3`; it loads `$C45986/$C45918` and joins `$C32ACC`.
- `initialize_postflight_transform_pair_offsets.asm` covers
  `$C32AB4-$C32ACF`; its entries either load those same offsets or clear them,
  then join `$C32B00` or `$C32ACC`.

At `$C32ACC`, `D0` becomes the formatter digit count (`D2`) and `D4=0`, then
the existing `$C32AD0` formatter follows.  These entries establish shared
formatting and renderer configuration only.  They do not identify the source
value, the resulting screen field, or a gameplay subsystem.
