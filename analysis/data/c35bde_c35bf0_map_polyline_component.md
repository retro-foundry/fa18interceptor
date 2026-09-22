# `$C35BDE -> $C35BF0`: map-mode three-vertex polyline component

Classification: **scenario-backed immutable three-triple input with decoded
static offset-pair line topology**.

The map-preparation trace enters `$C1F4AC` with `A1=$C35BDE` and transforms
three source triples to `$C48390`; the following control walk enters with
`A1=$C35BF0`. The exact source triples are:

```text
slot 0  (11136, 0, -9088)
slot 1  ( 6656, 0, -3840)
slot 2  (-13568,0,  2944)
```

The static `$C35BF0` control word dispatches `$C212B0`, the reconstructed
offset-pair segment submitter. Its data beginning at `$C35BF2` is:

```text
selector $0008
pair     $0000, $0006
pair     $0006, $800C  ; negative second word terminates the list
```

`$C212B0` uses each offset against `$C48390`, whose three-word slots have a
six-byte stride. The two renderer-selected segments are therefore exactly
`slot 0 -> slot 1` and `slot 1 -> slot 2`. Both reach `$C2FA7E` with static
`A5=$C35BD4`, yielding logical map endpoints `(148,123)->(152,118)` and
`(152,118)->(173,112)`.

```text
$C35BDE-$C35BEF immutable triples
  -> $C1F4AC/$C1F524 transformed slots 0..2
  -> $C35BF0 / $C212B0 static offset pairs
  -> $C35BD4 line record -> $C2FA7E map lines
```

This establishes a complete local three-vertex polyline component for this
bounded map scenario. It does not locate the component globally, establish a
coastline ownership match, or identify the rest of the terrain mesh.

Authority: sealed `captures/run003`; ignored
`build/run003_m_map_appearance_trace/{trace.jsonl,slow.bin}`; and byte-exact
[`$C212B0` source](../../source_amiga/observed/submit_offset_pair_segments.asm).
