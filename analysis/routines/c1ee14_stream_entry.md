# `$C1EE14-$C1EF15`: stream-entry selector and descriptor publication

Classification: **bounded producer prefix**.

The entry derives its threshold from `$C45B40`, conditionally scales it with
`$C45A42`, then walks the caller-owned stream at `$C45A36`. The exact
`$C1EE58-$C1EE83` threshold loop selects a word relative to the caller's
record base. The following gates either return zero/one or publish:

- the selected descriptor at `record_base + (word & $0FFF)` to `$C45A32`; and
- the post-selection stream cursor to `$C45A36`.

The `$C45864`, `$C4579E`, `$C458DA`, `$C459B4`, and `$C4F6CA` branches are
kept as explicit caller-owned fields. The subsequent descriptor decode,
transforms, and `$C1F6F8` walker are outside this prefix.

`scene_stream_entry.{c,h}` composes that source boundary over the existing
threshold selector and records the source's return-zero, return-one, and
descriptor-ready routes. Its contract uses synthetic byte streams only; no
recorded RAM or display page is introduced.
