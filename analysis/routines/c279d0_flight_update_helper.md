# Flight-update helper at `$C279D0`

Classification: **dataflow with renderer-descendant evidence**. The parent
update calls `$C279D0` at `$C0F0C2`. In the sealed attract cockpit capture its
executed path reads the projection component `$C45A78`, rejects values below
`-$800` through `$C27C44`, and later reaches `$C2FF48`, the bounded polygon
submission wrapper. This establishes a renderer-facing component gate, not a
physical-distance interpretation.

The exact entry prologue is in
`source_amiga/observed/initialize_c279d0_flight_helper.asm` (`$C279D0-$C279F9`).
The executed depth gate is in
`source_amiga/observed/gate_c279d0_projection_component.asm`
(`$C27A0C-$C27A31`). It loads `$C45A78`, branches to `$C27C44` when the signed
value is less than `-$800`, otherwise sets `$C457A2` and compares against
`-$200`. The capture does not execute the lower-range fall-through at
`$C27A32`; it remains unclassified rather than inferred.

Authority: `pcode/raw/attract_cockpit_1800_tenframe_trace/observed.asm.txt`,
the byte-exact baseline RAM comparison, and
`source_amiga/observed/run_parent_flight_update.asm` for the direct parent
call. `$C45A78` is also read by map and descriptor paths, so this slice alone
does not prove a shared spatial metric or an LOD threshold.

The contiguous observed loop `$C27AF4-$C27C4D` is now byte-exact in
`source_amiga/observed/project_c279d0_table_records.asm`. It reads records
through the selected table pointer, applies signed fixed-point products from
`$C45BD8`, rejects records outside its signed component bounds, derives two
bounded screen-pair values with `DIVS` by the transformed depth, fills the
`$C4B392-$C4B39D` pair buffer, and calls `$C2FF48`. This proves a local
projection-to-polygon-submission dataflow for the captured route. It does not
identify the source table's world object, establish camera-space axes, or
convert the `$C45A78` thresholds into physical units.

The separate direct-pair branch `$C27C62-$C27D0F` is exact source in
`source_amiga/observed/project_c279d0_direct_pair.asm`. After the same
fixed-point/depth rejection and screen-pair derivation, it dispatches to
`$C2F60A` only when local word `-$18(A6)` is two; otherwise it calls
`$C2F5F4`. This is a proved renderer-helper selection keyed by an internal
record-kind value. The value's object class and visual interpretation remain
unknown.
