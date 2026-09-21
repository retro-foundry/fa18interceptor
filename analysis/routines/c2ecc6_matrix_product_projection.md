# `$C2ECC6-$C2ED6B`: static matrix-product projection and dispatch

Authority: the canonical slow-RAM snapshot and the direct `$C2EC9C` call from
the runtime-backed fixed tuple routine. The observed call takes the rejection
tail before this range, so this contract is explicitly static/dataflow only.

The range scales `D0` by `$A0` and `D1` by `$5A` against `D2`, applies offsets
and clamps to `$000..$13F` / `$000..$B3`, converts the pair from those maxima,
checks it against `$C45984`, and stores it at `$C45958`. It then uses `D7` and
the word at `-40(A6)` to select one of `$C2F5F4`, `$C2F60A`, `$C2F66E`, or
`$C2F1C0`; success returns `D0=1`. The result, display, and child semantics
remain unassigned pending an accepted runtime call.
