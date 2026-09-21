# `$C50158` four-record update iterator

Classification: **structural**.

The complete byte-exact routine iterates four entries beginning at `$C4FE28`.
For each non-null record reached through the entry's two-pointer chain, it
calls `$C50212`, then `$C501E0`, adds longwords `A3+$18/$1C` to `A3+$08/$0C`,
and decrements independent longword countdowns at `A3+$38/$3C`. A countdown
reaching zero clears its corresponding `+$18` or `+$1C` delta.

It is observed in menu, attract, human-flight, and run024 crash-result capture
packets. `source_amiga/observed/iterate_four_a3_record_updates.asm` preserves
the full `$C50158-$C501DF` range, including skipped zeroing arms. The pointer
chain's record type and all field/gameplay meanings remain unassigned.
