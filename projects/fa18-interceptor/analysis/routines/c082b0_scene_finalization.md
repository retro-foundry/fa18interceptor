# `$C082B0`: scene-finalization direct stores

Classification: **static direct state contract**, with a run075 scene-caller
edge.

## Source and contract

Static disassembly of `$C082B0-$C08322` shows no calls. It writes `$0090` to
`$C45984`, then writes byte `$03` to fourteen direct control fields:
`$C45837/$36/$39/$3A/$3E/$3F/$40/$41/$3B/$3D/$43/$44/$45/$3C`.

It tests `$C457C0`. Only when that byte is zero does it clear word `$C458D8`
and longword `$C45918`; a nonzero value preserves them. `$C0FAA4` calls this
routine after `$C11312` in the run075 frame-370 initialization trace.

## Native contract

`port/scene_finalization.c` preserves these direct stores in
`FA18SceneFinalizationState`; the individual control-field names remain
structural. `scene_finalization_contract_test` verifies both guard outcomes,
and `scene_entry_contract_test` supplies this as `$C0FAA4`'s fourth ordered
helper. Consumers and semantic names remain open.
