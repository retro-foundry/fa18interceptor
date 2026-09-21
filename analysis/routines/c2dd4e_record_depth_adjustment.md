# Record depth-adjustment stage at `$C2DD4E`

Classification: **dataflow/behavioural adjustment stage**.  It is called from
`$C2D6F8` directly before the signed matrix-product stage `$C2DEE0` in every
sampled no-key, run001, run003, and attract update packet.

The exact entry `$C2DD4E-$C2DD71` saves `D0-D1/D4/A0`, tests bit 7 of the
caller record's first byte, derives `D4 = D7 >> 3`, and for values below `$384`
calls `$C2DE96` then `$C2E6DA` before entering raw interior arithmetic.

The shared exact return `$C2DE7E-$C2DE95` restores those registers, adds `D3`
to `D0` and `D5` to `D2`, and when `$C459B4` is zero publishes `D3` to
`$C45946`.  This proves an adjustment handoff to the next matrix stage, but not
the physical meaning of the record or offsets.
