# `$C0D7E0-$C0D871`: display-record pair selector

Authority: the return-bounded attract cockpit capture which returns from
`$C2E758` to `$C0D7E0`, plus canonical slow-RAM bytes. This prefix is directly
upstream of the already reconstructed `$C0DA70` success/rejection returns and
the record-write leaves.

The prefix reads four workspace words at `$C4E854 + {0,2,4,6}`. Its nested
zero/nonzero tests either select one of six pairs from offsets `{8,10,12,14}`
into `D0/D1`, or branch to one of the rejection/alternate setup paths. The
selected pair and later state fields have no assigned display or gameplay
meaning; this is a verified dataflow and branch-selection contract only.
