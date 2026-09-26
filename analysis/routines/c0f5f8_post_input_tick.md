# `$C0F5F8`: post-input tick

Classification: **static complete routine with a bounded tail route**.

## Evidence

`source_amiga/observed/run_post_input_tick_prefix.asm` reconstructs
`$C0F5F8-$C0F7D1`, and `run_post_input_tick_tail.asm` reconstructs
`$C0F7D2-$C0F811`. Together they exactly cover the routine. The routine's
bytes are checked against `captures/baseline_menu/slow.bin` by
`python scripts/verify_reconstructions.py`.

A no-future-input trace from the training frame-600 snapshot enters `$C0F5F8`
and returns to `$C0EFEA` in 38 instructions. Canonical P-code is
`pcode/raw/training_600_c0f5f8/`. It takes the signed early guard at
`$C45898`, joining the tail at `$C0F7D2`.

## Observed route

The bounded route increments `$C457C1`, decrements `$C45AD6`, calls the
callback pointer at `$C1820C` (which resolved to `$C1075A` in this packet),
clears `$C457A3`, and returns.

The sealed run060 success activation supplies a second, distinct callback
fixture.  At entry to the directly traced `$C110A4` sequence writer, the
active stack return address is `$C0F808`; the byte-exact tail places
`JSR (A0)` at `$C0F804`.  Thus the live callback pointer dispatch at
`$C0F804` invoked `$C110A4` on this activation.  That routine sees the
countdown `$C45AD6=-1`, installs `$C10DAE`, and takes its observed mode-9
selector-writing route.  This proves the callback edge and countdown gate;
the same bounded run060 walk identifies `$C0F7FA` as the zero-to-negative
store.  It does not identify the earlier writer that selected this callback
and timing state.

## Static branches awaiting a matching packet

When the entry guards permit it, the routine calculates an offset from
`$C45AF2` and four longword fields in `$C45904-$C45914`, conditionally adds it
to `8($C1AB74)`, and selects callback addresses `$C0F920`, `$C0F946`,
`$C1104C`, or `$C11078` based on byte fields around `$C4582A`. These field
roles are descriptive only; their gameplay purpose and every non-tail route
remain unclaimed until bounded traces reach them.
