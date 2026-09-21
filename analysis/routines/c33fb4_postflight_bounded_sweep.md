# `$C33FB4` postflight bounded sweep

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

The entry accepts `$C45988 + d0` only in `(0, $13F)`, stores its incoming
words in an 8-byte stack frame, performs an initial `$C2F60A` submission, then
runs two bounded sweeps. The first decrements a working word by 3 until it is
at most `$47`; the second starts from `$C4598C` and increments by 3 until it
reaches `$70`.

Each loop calls `$C2F5F4`, except on iterations 4 and 9 where it adjusts `d0`
by the saved word and calls `$C2F60A`. This proves the bounded call pattern;
the rendering/visual meaning of the helpers remains unproven.
