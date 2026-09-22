# Origin-selector mutation probe

Classification: **controlled, reversible selector-causality experiment**. The experiment proves that the live two-word origin input affects terrain-template selection and placement output in the normal workspace refresh. It does not identify the original immutable world-cell table, establish global axes, or prove an LOD scheme.

## Method and authority

Both runs restore the same sealed `run033` state immediately before replay frame 404, replay normally to the same `$C1C860` breakpoint at chipset frame 18, then single-step to the same return PC `$C0F048` with later replay input deliberately suppressed. The control has no state edits. The mutation writes only the following values while paused at that breakpoint:

| address | control value | mutation value | width |
| --- | --- | ---: | ---: |
| `$C45C3E` | `$10800000` | `$11800000` | long |
| `$C45C46` | `$10C00000` | `$11800000` | long |
| `$C45785` | `$01` | `$01` | byte |

The last field is retained at its already-observed nonzero value so both runs take the `$C1C8B8` origin-pair branch. Complete trace authority is retained in `build/run033_origin_control_trace/` and `build/run033_origin_mutation_trace/`. The generated joins are [control inventory](workspace_template_copies_origin_control.md) and [mutation inventory](workspace_template_copies_origin_mutation.md).

## Result

| run | copied static entries | later builder reads | unique sources | output X range | output Z range | zero middle words |
| --- | ---: | ---: | ---: | --- | --- | --- |
| control | 106 | 91 | 106 | -704 to 88 | -352 to 512 | 91 / 91 |
| mutation | 60 | 35 | 60 | -736 to 514 | -348 to 112 | 35 / 35 |

Every mutation source also occurs in the control; the control has 46 additional sources. Therefore changing only the origin pair changes the selected template subset rather than introducing an unrelated data family.

For 33 static sources that reach output records in both runs, the resulting placement differs. Examples:

| static source | control output | mutation output |
| --- | --- | --- |
| `$C42647` | `(0, 0, 512)` | `(-512, 0, 0)` |
| `$C4264D` | `(76, 0, 428)` | `(-436, 0, -84)` |
| `$C4265F` | `(-64, 0, 448)` | `(-576, 0, -64)` |
| `$C42683` | `(-148, 0, 416)` | `(-660, 0, -96)` |

This proves a live origin pair causally drives the static-template paging and the generated placement cache. It supports a paged/tiled terrain representation as an inference, but no source table that maps world coordinates to all pages has yet been traced. The all-zero middle outputs remain scenario evidence for a flat placement plane, not a universal terrain-height invariant.

Visual diagnostics: [control X/Z plot](../plots/workspace_template_placements_xz_origin_control.svg) and [mutation X/Z plot](../plots/workspace_template_placements_xz_origin_mutation.svg).

## Independent axis probes

The same breakpoint procedure was repeated with only one origin component
incremented at a time. The selector reduces each high-word component to a
live bin term, so the control is `(16,16)` and each individual perturbation
changes exactly one term to `17`.

| input change | copied sources | later builder reads | joined one-to-one common sources | dominant output delta |
| --- | ---: | ---: | ---: | --- |
| `$C45C3E`: `$10800000 -> $11800000` | 60 | 53 | 51 | 47 × `(-512, 0)`; 4 × `(0, 0)` |
| `$C45C46`: `$10C00000 -> $11800000` | 95 | 79 | 79 | 75 × `(0, -512)`; 4 × `(0, 0)` |
| both components | 60 | 35 | 33 | 29 × `(-512, -512)`; 4 × `(0, 0)` |

The independent deltas are separable and compose in the combined probe. This
proves a two-axis terrain-template paging lattice with a **512-unit pitch in
the generated placement cache**. The unchanged minority is expected to need
separate source/descriptor analysis; it does not invalidate the dominant,
controlled effect. It remains incorrect to call the cache pitch an absolute
world-unit scale or to infer an all-page source table until the static
coordinate-to-stream control data is traced.

The corresponding diagnostics are [X-only](../plots/workspace_template_placements_xz_origin_x_plus.svg) and [Z-only](../plots/workspace_template_placements_xz_origin_z_plus.svg).
