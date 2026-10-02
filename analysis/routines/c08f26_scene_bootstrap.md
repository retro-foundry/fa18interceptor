# `$C08F26-$C090AD`: cold-boot scene bootstrap

The heading describes the earlier initialization slice. The **complete**
routine ends at C090C0, with 80 original instructions including the final
C1C40C/C1C63E/C1C860 parent calls after C090AE. It is now implemented by
`port/game/scene_bootstrap.c`, with a normal CPU adapter and source timing.
All 8,192 independent full-register/full-SR/all-RAM cases and three recorded
sandbox comparisons of the standalone readable body pass. See
`native_c_scene_bootstrap.md`; nested parent comparisons are kept distinct.

Classification: **scenario-backed direct initialization contract**.

The cold-boot-to-menu trace executes this complete body in chipset frame 7769.
It calls its two earlier setup helpers, clears the first 164 bytes of all 16
`$C46184 + index*$200` records, clears 32 bytes of all 16 `$C48184 +
index*$20` work entries, invokes `$C09620`, and then calls `$C09266`.

The source range is preserved in
`source_amiga/observed/initialize_scene_bootstrap.asm`. Its direct stores and
two clear extents are reconstructed from
`build/cold_boot_menu_init_root_creation_instruction_trace/trace.jsonl`:
the record clear begins at row 2037, the `$C09266` call begins at row 4307,
and the trace advances exactly 8,511 instructions through frame 7769.

The callees `$C090C2`, `$C090F2`, `$C2FD22`, `$C0910C`, `$C0915A`, and
`$C09266` retain their existing bounded/native ownership. This source slice
does not establish the menu-to-scene scheduler or authorize an init-only call
from `game.c`; rendering still requires the source-driven active record,
projection matrix, page selection, and viewport schedule.
