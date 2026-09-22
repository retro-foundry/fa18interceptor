# `$C38E2C/$C38E54`: external-scene matrix-input packets

These Hunk-51 packets are immutable matrix inputs observed in the run031
external-camera replay.  They must be kept separate from the neighbouring
`$C38F98` face-control family until a renderer record proves a shared slot
contract.

## `$C38E2C-$C38E4F`: five direct triples

The selected transform trace enters `$C1F100` with `A1=$C38E2C` and
`A3=$C48390`.  It reads the first triple, applies the `$C45BC6/$C45BD8`
matrix paths, and writes the transformed result into the mutable workspace.
At the following `$C1F6F8` record-walker entry, `A1=$C38E50` and
`A3=$C483AE`.  This proves five consecutive six-byte inputs at
`$C38E2C-$C38E4F` and five direct workspace slots
`$C48390-$C483AB`; `$C38E50`/`$C483AE` are the next pointers, not source
vertices.

Across the 7,600-frame replay search this packet is selected 58 times at the
matrix entry.  The traced pass subsequently has controller state
`A5=$C39260`, then enters the static `$C384CC` control stream.  That is
dataflow evidence for an external-scene transform packet, not sufficient
evidence to name an object.

## `$C38E54`: adjacent independently selected packet

`$C38E54` is selected 134 times by the same matrix-entry inventory.  A focused
trace begins with `A1=$C38E54`, `A3=$C48390`, and later reaches `$C1F6F8` with
`A1=$C38E60`, `A3=$C48396`, again under `$C39260`.  Its exact coordinate-table
extent and downstream face selection are deliberately left unresolved.

## Exclusion from `$C38F98` export

The `$C38F98` collector observes eleven static face records, but the focused
`$C38E2C` trace does not reach that face context before its bounded trace ends.
Neither packet is therefore assigned as `$C38F98` geometry.  Treat the packets
as trace-proven static transform inputs and `$C38F98-$C3900E` as static
face/control data; continue tracing the shared renderer control path before
combining them into a model export.

Authority: `build/run031_initial_c1f100_matrix_input_inventory/matrix_transform_entries.json`,
`build/run031_initial_c38e2c_transform_trace_v3/trace.jsonl`, and
`build/run031_initial_c38e54_transform_trace/trace.jsonl`.
