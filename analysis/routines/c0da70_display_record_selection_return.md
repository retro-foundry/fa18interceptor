# `$C0DA70-$C0DA9F`: display-record selector returns

Authority: the return-bounded attract cockpit trace whose `$C0D752` caller
returns to `$C0D7E0`, plus static bytes from the canonical slow-RAM snapshot.
The enclosing selector reaches these two returns after arranging record-write
leaves at `$C0DAA0-$C0DAED`.

The success return writes one to `$C456E6` and `$C456E8`, clears the longword
at `$C456EA`, sets byte `$C4589E`, returns `D0=0`, then tears down `A6`.
The rejection return clears only `$C4589E`, returns `D0=1`, and also tears down
`A6`. The ownership and visual meaning of these state fields remains
unassigned; the named source records only their observable write/return
contract.
