# Run041 descriptor detail selector

Classification: **same-invocation branch and source joins**. Reproduce with
`python scripts/collect_descriptor_stage_invocations.py` followed by
`python scripts/analyze_run041_detail_selector.py`. The latter checks each
runtime control word against the canonical Slow-RAM image and records every
comparison and selected record in `run041_detail_selector_decisions.json`.

`$C1EE14` loads its limit from `$C45B40` and shift from `$C45AB8`. At
`$C1EE58-$C1EE83`, each candidate word is masked with `$3FFF`, arithmetic
shifted by the live shift, and compared as a signed word with the limit.
An above-limit result selects the record at the current cursor (or skips two
bytes first when flag `$4000` is clear). Otherwise flag `$4000` skips four
bytes; an unflagged word follows its relative pointer. The terminal word
enters `$C1EE84` directly. This 44-byte selector slice is byte-exact in
`source_amiga/observed/select_c1ee14_threshold_record.asm`.

| Checkpoint | Control | Limit / shift | Decisive word | Selected record | Transform source |
| --- | --- | --- | --- | --- | --- |
| 5,000 | `$C35568` | 545 / 2 | `$4500` -> 320, within | `$C35596` `$A638` | `$C35BAA` |
| 5,750 | `$C35568` | 656 / 1 | `$4500` -> 640, within | `$C35596` `$A638` | `$C35BAA` |
| 6,000 | `$C35568` | 512 / 1 | `$4500` -> 640, above | `$C35592` `$25E4` | `$C35B56` |
| 6,250 | `$C35568` | 524 / 0 | `$42E0` -> 736, above | `$C3558C` `$056C` | `$C35ADE` via `$C1CFA6` |
| 5,000 | `$C355A0` | 523 / 2 | `$4500` -> 320, within | `$C355CE` `$A618` | `$C35BC2` |
| 5,750 | `$C355A0` | 611 / 1 | `$4500` -> 640, above | `$C355CA` `$25D6` | `$C35B80` |
| 6,000 | `$C355A0` | 419 / 1 | `$4500` -> 640, above | `$C355CA` `$25D6` | `$C35B80` |
| 6,250 | `$C355A0` | 455 / 0 | `$4200` -> 512, above | `$C355BE` `$44EE` | `$C35A98` via `$C1CFA6` |

At frame 6,250, `$C1CC86` makes short calls selecting the same records but
does not enter the transform. The later `$C1CFA6` calls do enter it and emit
lines. This is an observed caller-pass difference. One last 6,250 invocation
is capped by the 12-frame trace, so only its observed prefix is used.

The selector rule is proved; the ownership and meaning of `$C45B40` and
`$C45AB8` are not. Existing exact source shows `$C45B40` has at least two
writers: the fixed-point stage at `$C1D91A` and the alternate record loop at
`$C1CF36`. Identify which writer feeds each selected call before naming this
a distance or level-of-detail rule. The physical coordinate chain and reason
for the caller-pass change also remain open.
