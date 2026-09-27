# Native `$C42A02` scene-record table adapter

Verified Hunk 67 maps at `$C429D0`; its immutable `$C42A02` table starts at
offset `$32` and `$C42A54` starts at `$84`. The intervening `$52` bytes are
five 16-byte, eight-word table-A entries. `$C0924A`, `$C1B6C2`, and `$C1BE60`
all index this family in 16-byte steps.

`port/scene_record_table.c` loads the table directly from Hunk 67 and decodes
table-A entries as signed big-endian words. `FA18Game` owns the loaded table
alongside its other executable-backed assets. This proves storage and entry
format only; no table word is promoted to a scene or gameplay name.
