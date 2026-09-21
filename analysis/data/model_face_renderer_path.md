# Model-face renderer path

This is a renderer contract, not a claim that an immutable source-model table has already been extracted.

## External-view trace anchor

`build/run031_frame7500_c3925a_stream_trace_20k/selected_stream_trace.jsonl` begins at the live `$C1F6F8` walker entry for the byte-stable `$C3925A` controller. It selects the `$C3925C` static substream, then executes this observed path for its first finalized polygon:

```text
$C3925A controller
  -> $C3925C substream / $C39376 face-offset record
  -> $C1F942 indirect dispatch ($C2005C)
  -> $C2005C reads 3/4 signed offsets from A2
  -> selected triples copied from $C48390 into $C4BF90
  -> $C1FB82 orientation predicate
  -> $C2469E face clip/project stage
  -> $C24CFE finalized polygon preparation
  -> $C2FF48 / $C301F6 bounded screen-pair submitter
  -> $C2FA7E line-emission/blitter path
```

The trace reaches `$C2FF48` at instruction 763 with `A5=$C3925C`; the same controller path later reaches it twice more with `$C3925C` and three times with `$C3925E`. Those six polygons are the combined candidate shown in [the orthographic controller-family plot](../plots/external_aircraft_c3925a_controller_family_orthographic.svg).

## What each stage proves

| Stage | Proved input/output | Meaning level |
| --- | --- | --- |
| `$C3925C/$C39376` | Static face-record words select vertex offsets and a dispatch target. | dataflow |
| `$C2005C-$C200F5` | Copies three or four triples selected from `$C48390` into `$C4BF90`; calls the orientation predicate and `$C2469E`. | behavioral |
| `$C1FB82` | Signed fixed-point three-vector orientation/sign predicate; its result gates the face route. | behavioral |
| `$C2469E-$C24D60` | Builds projected face data and passes a finalized polygon tuple onward. | dataflow |
| `$C2FF48-$C301F6` | Consumes `$C4B390` screen pairs and reaches the known `$C2FA7E` blitter line emitter. | behavioral |

## Crucial boundary

`$C48390` contains the triples actually selected for faces, but it is a mutable projection workspace. It is therefore the correct place to recover **face topology and current rendered geometry**, not a safe local-model export. The remaining model-extraction task is to catch the first writer of the specific `$C48390` slots selected by `$C39376` before the `$C3925A` controller runs, and then identify that writer's immutable source range.
