# `$C0F5F8`: post-input tick

Source review on 2026-10-02 corrected the typed port's offset limit:
$C0F69A uses hexadecimal $4650 (18,000), whereas the earlier C used decimal
4650. The contract check now covers 5,000 and 17,999 as valid and 18,000 as
invalid. This is a source-backed groundwork correction; the complete normal
CPU adapter and live source-timing registration are now implemented in
`port/game/post_input_tick.c` and `port/game/glue/glue_post_input_tick*.c`.
Independent proof covers 16,384 complete all-register/full-SR/all-RAM cases
without exclusions, plus 31,930 normal recorded whole-call comparisons.
See `analysis/routines/native_c_post_input_tick.md` for current integration
evidence; the older static/archived route evidence below remains historical.

Classification: **static complete routine with a bounded tail route**.

## Evidence

`source_amiga/observed/run_post_input_tick_prefix.asm` reconstructs
`$C0F5F8-$C0F7D1`, and `run_post_input_tick_tail.asm` reconstructs
`$C0F7D2-$C0F811`. Together they exactly cover the routine. The routine's
bytes are checked against `captures/uae/baseline_menu/slow.bin` by
`python scripts/verify_reconstructions.py`.

A no-future-input trace from the training frame-600 snapshot enters `$C0F5F8`
and returns to `$C0EFEA` in 38 instructions. Canonical P-code is
`pcode/raw/training_600_c0f5f8/`. It takes the signed early guard at
`$C45898`, joining the tail at `$C0F7D2`.

## Observed route

The bounded route increments `$C457C1`, decrements `$C45AD6`, calls the
callback pointer at `$C1820C` (which resolved to `$C1075A` in this packet),
clears `$C457A3`, and returns.

The bounded menu adapter still represents this route as
`fa18_menu_post_input_tick`. The wider source routine is now separately
represented by `fa18_run_post_input_tick` in `port/post_input_tick.{c,h}`:
it performs the complete local prefix and tail, including callback selection,
callback dispatch, and command-pending clear. The `$C06C02` invalid-offset
call and callback bodies are required caller-owned hooks; missing owners are
reported explicitly, never replaced by native convenience behavior.
`post_input_tick_contract_test` checks the early-tail, phase-one,
phase-three, valid-offset, and invalid-offset routes.

The sealed run060 success activation supplies a second, distinct callback
fixture.  At entry to the directly traced `$C110A4` sequence writer, the
active stack return address is `$C0F808`; the byte-exact tail places
`JSR (A0)` at `$C0F806` (the earlier report's `$C0F804` was a transcription
error). Thus the live callback pointer dispatch at `$C0F806` invoked
`$C110A4` on this activation. That routine sees the
countdown `$C45AD6=-1`, installs `$C10DAE`, and takes its observed mode-9
selector-writing route.  This proves the callback edge and countdown gate;
the same bounded run060 walk identifies `$C0F7FA` as the zero-to-negative
store.  It does not identify the earlier writer that selected this callback
and timing state.

An aligned suffix replay from the sealed global-frame-9,200 checkpoint closes
that local selection chain at global frame 9,263.  The complete 80-instruction
`$C0F5F8` invocation observes `$C45798=$FF`, `$C4582A=3`, and signed
`$C4582C=$FF`.  Its phase-three branch clears `$C4582A`, copies the zero word
at `$C458C0` to `$C45AD6`, and installs `$C11078` in `$C1820C`.  The shared
tail immediately decrements that zero to `$FFFF`, dispatches `$C11078`, and
the callback installs `$C110A4` with a delay of two.  The bounded fixture is
`build/run060_frame09200_posttick_to_c11078_trace/`.  These are scheduler
state values and branch facts, not a decoded qualification predicate.

## Static branches awaiting a matching packet

When the entry guards permit it, the routine calculates an offset from
`$C45AF2` and four longword fields in `$C45904-$C45914`, conditionally adds it
to `8($C1AB74)`, and selects callback addresses `$C0F920`, `$C0F946`,
`$C1104C`, or `$C11078` based on byte fields around `$C4582A`. These field
roles are descriptive only; their gameplay purpose and every non-tail route
remain unclaimed until bounded traces reach them.
