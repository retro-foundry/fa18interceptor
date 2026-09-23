# run037 `$C44970` template-to-polygon path

Classification: **bounded active static-template-to-renderer path**. This
establishes one descriptor target's route to transformed static input and
polygon submission; it is not ownership of a coastline pixel, complete mesh,
or terrain cell.

The run037 stable-map template-placement trace joins immutable source
`$C427AD` to descriptor `$C22890`, producing placement `(192, 0, -448)` and
descriptor `+8` target `$C44970`. `$C44970` is a verified 24-byte,
relocation-backed original Hunk-70 record.

A no-future-input trace begins exactly when `$C1CC70` reads that descriptor
field and writes it to `$C45A36`. Within its next 6,000 instructions, the
same execution reaches:

```text
$C1CC70  descriptor +8 field `$C44970` -> `$C45A36`
  -> `$C096CA` callback route
  -> `$C1EE14` control-stream stage
  -> `$C1F6F8`
  -> `$C1F4AC` with immutable source `$C3B720`
  -> four `$C2FF48` polygon submissions (context `$C3B6B0`)
  -> one `$C2FA7E` line emission (same context)
```

`$C3B720` and `$C3B6B0` are existing bounded static scene-component
authorities; the new result is the upstream join from one active
template/placement descriptor target to that renderer path. The trace ends at
its instruction cap rather than an enclosing routine return, so it proves the
observed forward sequence only. It does not prove that the `$C427AD` placement
alone owns every later submission, nor that this component corresponds to a
particular map feature.

Authority: sealed `captures/run037`; template inventory
`analysis/data/workspace_template_copies_run037_m_map_template_to_placement_3f.json`;
and `build/run037_c44970_descriptor_target_trace_v2/trace.jsonl`.
