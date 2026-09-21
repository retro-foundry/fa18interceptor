# Renderer-observed model identification catalogue

Each sheet is a tight-fit orthographic X-Y / X-Z / Y-Z plot. A coloured closed outline is one polygon observed at `$C2FF48`; no nearest-neighbour links or unseen faces have been invented. "Candidate" means the source-to-renderer path is traced, while the human-readable object name is provisional unless noted.

[Open the side-by-side gallery](../plots/model_identification_gallery.png) to compare every current candidate and unresolved family at once. Regenerate it with `python scripts/render_model_identification_gallery.py` after adding a sheet.

| Candidate / family | Renderer evidence | Current identification | Sheet |
| --- | --- | --- | --- |
| `$C3515E` to `$C34C06-$C34C48` | 5 pre-clip faces / 26 edges; 22 of their 36 vertex slots trace from immutable inputs, remaining 14 slots predate replay | partial source-to-face tapered aircraft-detail candidate; user-facing name remains provisional | [evidence](c34c_aircraft_detail_face_family.md) Â· [orthographic + isometric](../plots/c3515e_c34c_aircraft_detail_isometric_sheet.png) |
| `$C34A9A/$C34A9C` | 42 unique pre-cull faces, 156 polygon edges; uses separate `$C48390` workspace, including an X-axis-only frame-12000 state | aircraft-like renderer family; immutable source linkage pending | [complete pre-cull sheet](../plots/external_aircraft_c34a9_preclip_complete_face_sheet.png) |
| `$C39D2A` to `$C3925C/$C3925E` | 10 unique pre-cull faces, 45 polygon edges; static transform and both face contexts traced | **user-identified aircraft-carrier deck/island** candidate | [complete pre-cull sheet](../plots/c39d2a_c3925_preclip_complete_face_sheet.png) |
| `$C35932` / `$C3B720` batches | 25 polygons, 86 polygon edges, 36 line segments; bounded transform-batch capture | broadest partial Golden Gate bridge composite | [assembled sheet](../plots/golden_gate_assembled_transform_batches_model_sheet.png) |
| `$C355D8/$C355D6` | 12 unique pre-clip faces, 42 polygon edges, 39 line segments | bridge face-family component, including faces not finally submitted | [pre-clip faces + lines](../plots/golden_gate_c355_preclip_complete_combined_sheet.png) |
| Golden Gate batch scene | 51 polygons, 184 polygon edges, 52 line segments; same batches with `$C34A9A` retained | bridge context plus the separate aircraft family | [scene-context sheet](../plots/golden_gate_transform_batch_scene_context.png) |
| `$C35932` to `$C355D8` | 15 polygons, 53 edges; alternate transform to final submission traced | bridge face-family component | [sheet](../plots/golden_gate_c35932_c355d8_model_sheet.png) |
| `$C362A2` to `$C36298` | 16 final polygons / 6 unique pre-clip records; alternate transform to final submission traced | later bridge checkpoint, unnamed bridge-like candidate | [silhouette](../plots/later_bridge_c36298_model_sheet.png) · [pre-clip faces](../plots/later_bridge_c36298_preclip_face_sheet.png) |
| `$C3B4FE` | 14 polygons, 49 edges at the later bridge checkpoint | renderer family; source/model boundary not yet tied | [sheet](../plots/bridge_checkpoint_c3b4fe_model_sheet.png) |
| `$C3B50A` | 7 polygons, 28 edges at the later bridge checkpoint | renderer family; source/model boundary not yet tied | [sheet](../plots/bridge_checkpoint_c3b50a_model_sheet.png) |
| `$C38F98` | 11 unique pre-cull faces, 36 polygon edges at the external checkpoint | unidentified tall/thin external-scene face family | [pre-cull sheet](../plots/external_scene_c38f98_preclip_face_sheet.png) |
| `$C45BEA` | sampled mutable transform/display context used while `$C34C06-$C34C48` reaches `$C2469E` | **not a polygon source**; changes the pose/display of the linked aircraft-detail faces | [evidence](c45bea_runtime_display_record.md) |
| `$C3A94C/$C3A94E` | 6 unique pre-cull quad faces / 24 polygon edges; distinct live control stream and face-record family | unnamed compact polyhedral component; upstream immutable vertices untraced | [evidence](c3a94c_compact_polyhedron_candidate.md) Â· [pre-cull sheet](../plots/frame12600_c3a94e_preclip_face_sheet.png) |

The catalogue deliberately keeps the last entries separate from the static-model candidates. `$C45BEA` is especially important: `$C0D74A` selects it as an alternate display-record base and the matrix-update route writes that cache. The aircraft-like plot previously associated with it is actually the linked `$C34C` face family. `$C38F98` and `$C3A94E` remain drawable renderer families whose upstream static source/ownership is not yet established.

Pre-cull-sheet qualification: `$C2005C` supplies selected faces before their orientation and clipping gates. The sheets retain one observed transformed sample for each distinct static face-record address, which recovers faces absent from `$C2FF48` without inventing topology. `$C212B0` line sheets remain supplementary because their endpoint lists may describe scene detail as well as model outlines.
