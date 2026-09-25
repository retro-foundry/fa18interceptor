# Demonstration-flight `$C3B5A6` to `$C3B5B6` component path

Classification: **event-faithful bounded descriptor-to-local-geometry-to-polygon path**.

The sealed no-input Demonstration Flight checkpoint at original frame 3,600
repeats a scene component four times in its following 30 hardware frames.  At
the first occurrence, `$C1CC70` reads descriptor `$C22870` field `$C3B5A6`.
The forward no-future-input trace then makes this complete observed sequence:

```text
$C22870 descriptor field `$C3B5A6`
  -> `$C1EF10` publishes cursor `$C3B5B4`
  -> `$C1F6F8` walker loads `A5=$C3B5B4`
  -> `$C1F4AC` transforms five local triples `$C3B628-$C3B645`
  -> next walker begins at `$C3B646`
  -> two `$C2FF48` polygon submissions with `A5=$C3B5B6`
```

The census records `$C3B62E` because it samples the first per-triple source
read at `$C1F4B0`, after the matrix entry has already loaded the first triple
from `$C3B628`.  It is therefore evidence for the five-triple input run
`$C3B628-$C3B645`, not a claim that `$C3B62E` begins the component.

The 30-frame ordinary refresh has four such input-transform occurrences and
eight `$C3B5B6` polygon submissions.  `$C3B5B6` also recurs in separated
M-map captures, where its semantic role is explicitly unknown.  The common
polygon context does not establish that the demo component is a building,
landmark, or unique world instance; no flight placement record is joined in
this evidence.

Authority: `build/attract_demo_scene_refresh_3600_30f_trace/trace.jsonl`,
`analysis/data/attract_demo_scene_refresh_3600_renderer_census.json`, and the
selected no-future-input trace
`build/attract_demo_c3b5a6_selected_trace/trace.jsonl`.  M-map recurrence is
documented separately in [`c3b5b6_m_map_reusable_component_repeat.md`](c3b5b6_m_map_reusable_component_repeat.md).
