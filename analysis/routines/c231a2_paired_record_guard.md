# `$C231A2`: paired 512-byte record-bank eligibility guard

Classification: **runtime-backed rejection-prefilter contract**. The
observed P-code path is memory-read-only and returns zero, but the full
success continuation at `$C231FE` and alternate `$C2321E` branch are not
covered here. This is not proof that the complete callee is pure.

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
4. Require `(byte[A2+$64] & $60) == $60`. Passing all observed guards
   falls through to `$C231FE`, whose behavior remains unassigned.

The shared rejection at `$C2321A` executes `MOVEQ #0,D0`, returning
`D0=0,Z=1`. Thus the three callers' `BEQ` branches select their skip/update
paths on this observed rejection. In
`build/no_key_c1c63e_capped/trace.jsonl`, the bank-10 invocation at rows
7314-7340 follows a negative `+$38` byte `$8E` to linked index 14, passes
its linked-record checks, then rejects because `A2+$64` has masked value
`$00` rather than `$60`. Its return to `$C22FD0` has `D0=0,Z=1`.

This explains a shared gate used by three record-bank update functions,
without naming the banks as aircraft, weapons, or AI entities. The captured
`observed_call_00c231a2` path has 27 distinct P-code starts in
`pcode/raw/no_key_c1c63e/` and no memory stores. Byte-exact entry source:
`source_amiga/observed/gate_c231a2_record_flags.asm`.
