# Record depth-adjustment stage at `$C2DD4E`

Classification: **dataflow/behavioural adjustment stage**.  It is called from
`$C2D6F8` directly before the signed matrix-product stage `$C2DEE0` in every
sampled no-key, run001, run003, and attract update packet.

The exact entry `$C2DD4E-$C2DD71` saves `D0-D1/D4/A0`, tests bit 7 of the
caller record's first byte, derives `D4 = D7 >> 3`, and for values below `$384`
calls `$C2DE96` then `$C2E6DA` before entering raw interior arithmetic.

The called zero-case prefix `$C2DE96-$C2DEA1` is reconstructed in
`clear_zero_matrix_record_guard.asm`: if record word `$50` is zero it
explicitly clears it again and returns; a nonzero word enters raw code at
`$C2DEB0`.

The shared exact return `$C2DE7E-$C2DE95` restores those registers, adds `D3`
to `D0` and `D5` to `D2`, and when `$C459B4` is zero publishes `D3` to
`$C45946`.  This proves an adjustment handoff to the next matrix stage, but not
the physical meaning of the record or offsets.

Further exact interior slices establish that `D4/D5` are published at record
offsets `$22/$24`; word `$54` is read, changed by a delta involving `D4`, and
republished; and `$50` is copied to `$52` before comparison with `$7E`.
The accepted scaling path reduces `D4` by its arithmetic quarter, transforms
`D3/D5` with signed fixed-point shifts/multiplication, and conditionally scales
both through the word table at `$C2DEC2` indexed by record word `$66 >> 7`.
This still does not identify the fields as a particular physical quantity.
