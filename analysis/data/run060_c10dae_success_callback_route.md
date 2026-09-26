# Run060 `$C10DAE` success callback route

Classification: **sealed-replay callback route**. This is a direct execution
comparison with the documented run062 `$C10DAE -> $C11788` failure-side route;
it is not yet evidence of the earlier writer or landing predicate.

From sealed native checkpoint
`build/run060_frame09285_checkpoint/frame_09285_state.bin`, a no-future-input
instruction trace enters `$C10DAE` on its first frame and reaches shared
continuation `$C11048` after 22 instructions. The retained trace is
`build/run060_c10dae_success_trace/trace.jsonl`.

| Step | Success execution |
| --- | --- |
| `$C10DBA` | loads word `$C46184 = $11C8` |
| `$C10DC0` | tests bit 9; it is **clear** |
| `$C10DC4` | takes `BEQ.W $C10E96` |
| `$C10E96-$C10F08` | follows the saved-selector dispatch with `$C45798=$EF` |
| `$C1100A` | installs callback `$C11958` in `$C1820C` |
| `$C11014` | branches to shared continuation `$C11048` |

The sealed run062 callback trace instead observes bit 9 set and the
nonzero-gate route that installs `$C11788`. Therefore bit 9 of the leading
`$C46184` word is a direct discriminator for these sampled postflight callback
routes. This does **not** establish which outcome owns either value: the bit
could be a result, a postflight state marker, or another prerequisite written
earlier. The producer and the causal landing/failure predicate remain open.
