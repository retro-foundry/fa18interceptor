# `$C1C860-$C1CA2D` context refresh packet

2026-10-02: complete live domain, CPU adapter and source timing are implemented
in `port/game/context_refresh.c` and `port/game/glue/glue_context_refresh.c`.
All 8,192 independent complete cases match every register, PC, full SR and all
RAM, including aliased save frames. See `native_c_scene_bootstrap.md` for
normal recorded and integration evidence. The older typed groundwork below
now uses record flagging (C1CA82 preserves D0), ASR.W #8 for alternate origins
and live request-bit rereads after children.

`context_refresh_packet.{c,h}` ports the structural parent packet executed in
`$C0F01C` immediately before the map conditional. It preserves:

- the signed `$C45A66 >= $F8000000` entry guard;
- request-bit classification, error reporting, individual bit consumption,
  and the three `$C1D10C` callback opportunities;
- selector publication from the selected active record or the alternate
  `$C45C3E/$C45C46` origin pair;
- ordered stage-A, stage-B, and stage-C callbacks with the source trace
  markers; and
- the guarded `$C2F66E` render-submit state, including its `$C456E6` and
  `$C45954` writes.

The record flagging, scene selector, downstream stages, error helper, and
render submission remain typed callbacks because their full owners are beyond
this local packet. The contract exercises all three selector callback bits,
the set stage-C route, and the prepared render submission.
