# `$C308D8` segment-37 store helpers

Classification: **static-only dataflow**. The currently available packet does
not take the positive-counter route that calls these entries.

`source_amiga/observed/submit_segment37_store_helpers.asm` preserves three
contiguous helper entries at `$C308D8-$C30916` (64 bytes): a next-item setup,
a first-item store, and a table-fed store. The entries call `$C53F44` and write
derived values to the Custom register block addressed by `a0`.

Names describe the directly observed calling/dataflow relationship only; no
visual or gameplay interpretation is assigned.
