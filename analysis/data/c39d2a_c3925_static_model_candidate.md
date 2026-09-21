# `$C39D2A → C3925C/C3925E`: static-model candidate

Classification: **trace-proven static vertex transform followed by renderer polygon contexts; long-component visual identity unresolved**.

At the Golden Gate checkpoint, `build/run031_frame12000_c39d2a_following_trace/trace.jsonl` begins at `$C1F100` with `A1=$C39D2A`. The same transform pass consumes 43 consecutive source triples in the `$C39Dxx` range. It then enters the renderer walker, selecting static contexts `$C3925C` and `$C3925E`; their face records reach `$C2469E` and `$C24CFE`.

The independent external-camera frame-7,500 final-polygon collector records three `$C3925C` and three `$C3925E` submissions. The pre-cull face collector observes five unique static face-record addresses in each context; [the complete pre-cull PNG sheet](../plots/c39d2a_c3925_preclip_complete_face_sheet.png) draws all ten selected faces. It is deliberately named a candidate: the Golden checkpoint trace proves the source-to-face contexts, while the external checkpoint supplies the transformed face geometry under a different camera/scenario.

The source set is substantially larger-scale than the `$C3515E` flight-object candidate. It was initially identified visually as an **aircraft-carrier deck/island**, but the evidence is not sufficient to distinguish that from a road or bridge segment: the observed faces recover a long narrow slab and a small raised/side structure, without a complete hull, roadway context, or traced placement/instancing logic. The label is therefore deliberately unresolved rather than an asserted carrier model.

[The static-coordinate topology candidate (SVG)](../plots/c39d2a_c3925_static_topology_candidate.svg) and [PNG sheet](../plots/c39d2a_c3925_static_topology_candidate.png) bind the ten observed face-offset lists directly to the 43 traced `$C39D2A` triples. They retain only those renderer-observed edges and make no extra deck, hull, island, or road connections.
