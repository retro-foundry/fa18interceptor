# `$C2ECC6-$C2ED6B`: static matrix-product projection and dispatch

Authority: the canonical slow-RAM snapshot, the direct `$C2EC9C` call from the
runtime-backed fixed tuple routine, and the sealed run041 `$C35A98`
source-bounded interval. The earlier observed `$C2EC9C` call takes the
rejection tail, but the run041 interval reaches this range through `$C2EC90`.

The range scales `D0` by `$A0` and `D1` by `$5A` against `D2`, applies offsets
and clamps to `$000..$13F` / `$000..$B3`, converts the pair from those maxima,
checks it against `$C45984`, and stores it at `$C45958`. It then uses `D7` and
the word at `-40(A6)` to select one of `$C2F5F4`, `$C2F60A`, `$C2F66E`, or
`$C2F1C0` on its nonnegative path. The negative-`D7` tail instead increments
the mode through bounded checks and can return the projected `D0` word.

The run041 `$C35A98` interval proves one accepted projection: `$C33D34` calls
`$C2EC90`, whose validation reaches `$C2ECC6` with signed input words
`D0=-19`, `D1=1452`, and `D2=7990`. Integer division gives the unclamped
intermediate pair `(160,106)`; the reflected low words are **`(159,74)`** at
`$C45958`. The path takes the negative-`D7` tail, leaves `D7=-1`, and returns
to `$C33D3A`. The earlier `(319,90)` claim was incorrect. The `$C2EC90` mode
stub and adjacent static-only `$C2EC94` entry are byte-exact in
`source_amiga/observed/select_projection_validation_mode.asm`; this trace
does not prove the purpose of `$C45AB8`, which the second entry reads. This is
scenario-backed perspective projection and screen-pair publication within the close landmark renderer
interval. It does not assign the pair to a final pixel, prove the meaning of
the `D7` mode, or establish the semantics of the unobserved nonnegative child
dispatches. `scripts/extract_run041_projection_oracle.py` checks the accepted
call/return against the trace and writes the reusable fixture
`analysis/data/run041_projection_oracle.json`. The same interval also contains
the rejected `$C0DB3A -> $C2EC9C -> $C0DB40` call: signed `D0=2120` fails
the first bound comparison against signed `D2=-5536`; `D0` returns zero and
the instruction at `$C2EC84` writes longword `$FFFFFFFF` to `$C45958`.
