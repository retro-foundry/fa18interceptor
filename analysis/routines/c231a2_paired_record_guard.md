# `$C231A2`: paired 512-byte record-bank eligibility guard

Classification: **runtime-backed paired-record route selector**. The
ordinary filter path is memory-read-only and returns zero; its distinct
nonzero route consumes `$C457BC` before returning one.

Three callers at `$C22F44`, `$C22FA8`, and `$C2300C` prepare adjacent
512-byte record banks: current `A1` is bank 9, 11, or 13, while `A2` is
respectively bank 8, 10, or 12. Each caller bypasses the callee when
`bit 6` at `A1+1` is set, then branches on the callee's returned Z flag.
The common callee filters `A2` as follows:

1. Reject to `$C2321A` if `word[A2] & $8700` is nonzero or bit 1 at
   `A2+$20` is set.
2. If byte `$C457BC` is nonzero, branch to `$C2321E` instead; that branch
   is **not** assigned the ordinary rejection result. Otherwise require
   bit 6 at `A2+1`.
3. If signed byte `A2+$38` is negative, use its low seven bits as an index
   into `$C46184 + index*$200`. Require linked-record bit 6 at `+1`, bit 1
   clear at `+$20`, and bit 0 clear at `+1`.
4. Require `(byte[A2+$64] & $60) == $60` and bit 0 at `A2+1` set. The high
   nibble of `A2+$63` then selects the result: `$20` or `$30` route to
   `$C2321E`; every other value falls through to `$C2321A`.

`$C2321A` executes `MOVEQ #0,D0`, returning `D0=0,Z=1`. This includes both
ordinary filter failures and the fully passed ordinary path whose `$63` high
nibble is neither `$20` nor `$30`. `$C2321E` clears `$C457BC`, executes
`MOVEQ #1,D0`, and returns `D0=1,Z=0`; it is reached either from a nonzero
`$C457BC` at the early state test or from the two terminal `$63` classes.

Thus the three callers' `BEQ` branches skip their special continuation on the
zero route. A nonzero result instead falls through to `$C2377E` and then the
shared `$C23A7E` update stage. A focused run060 trace forces `$C457BC=1` at
the bank-9 call, observes `$C2321E` clear it and return one, observes the
`BEQ` at `$C22F6C` not taken, and reaches `$C2377E` before `$C23A7E`. In that
specific state `$C2377E` clears bit 6 at `A1+1`; that helper's broader role is
not assigned. In
`build/no_key_c1c63e_capped/trace.jsonl`, the bank-10 invocation at rows
7314-7340 follows a negative `+$38` byte `$8E` to linked index 14, passes
its linked-record checks, then rejects because `A2+$64` has masked value
`$00` rather than `$60`. Its return to `$C22FD0` has `D0=0,Z=1`.

This explains a shared gate used by three record-bank update functions,
without naming the banks as aircraft, weapons, or AI entities. The captured
`observed_call_00c231a2` path has 27 distinct P-code starts in
`pcode/raw/no_key_c1c63e/`; the run060 forced-route trace adds the bounded
one-byte clear at `$C457BC`. Byte-exact source is split between
`source_amiga/observed/gate_c231a2_record_flags.asm` and
`source_amiga/observed/return_c231a2_route_result.asm`.
