# Indexed record updater at `$C26428`

Classification: **behavioural packet with byte-exact static source**. In sealed
run003 frame 6,000, `$C1484C` calls `$C26428` and it returns to `$C14852` after
62 instructions with empty future playback. Canonical P-code is
`pcode/raw/run003_6000_c26428/` (62 RAM instruction starts, 506 operations,
no nested calls).

The dynamic route is contained in the byte-exact
`source_amiga/observed/update_indexed_shared_record_fields.asm` slice
(`$C2641E-$C2651B`). Field names retain the established structural record/table
interpretation; no broader object ownership is claimed.
