# `$C1B038` nonzero-context key dispatch

Classification: **structural with byte-exact source**. This table handles a
context-2 special route, four paired raw-code routes, one single-code route,
and three input-latch pairs. Either code in each latch pair writes 1 to one of
`$C45878`, `$C45879`, or `$C4587A` and branches to `$C1C2B6`.

The raw-code pair names only describe the dispatch shape. Their press/release
roles and control semantics require isolated deterministic input traces.
