# Candidate-record scan at `$C26EBE`

Classification: **dataflow candidate scan**.  `$C2600E` calls this helper after
forming a relative `D2/D3/D4` triple, then branches on its returned `D0`.  The
entry is observed in attract, no-key, run001, and run003 update packets.

The exact `$C26EBE-$C26EFB` entry clears `$C4589F`, uses `$C459B6` as an offset
into the record region rooted at `$C46184`, snapshots words at relative `$00`
and `$02` plus byte `$5E`, and masks record byte `$04` down to its low six bits.
It extracts the high nibble of byte `$62` and returns immediately for class
`$20`; otherwise later raw code advances candidate offsets by `$200` and bounds
them against `$1E00`.

This proves record selection and class filtering, but not a collision/object
identity interpretation for the scan or its relative triple.
