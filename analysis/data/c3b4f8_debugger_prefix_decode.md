# `$C3B4F8` prefix: debugger-probe structural decode

Classification: **debugger-only parser dataflow**. The original run037 map
continuation does not naturally enter the walker with this pointer, so this
establishes record grammar only, not a scenario-level object or terrain claim.

With `$C45A36` debugger-written to `$C3B4F8` immediately before the byte-exact
`$C1F6F8` walker, the first record decodes as:

| Word | Observed walker use | Result |
| --- | --- | --- |
| `$42C0` | loaded at `$C1F71E` as `D0`; indexes `$C48390` | first of three copied workspace triples |
| `$4086` | loaded at `$C1F724` as `D7`; `$FC00`, `$0C00`, and `$2000` tests | direct three-triple route to `$C1FB8C` |
| `$C014` | loaded at `$C1F754` as first subsequent offset | second of three copied workspace triples |
| `$4480` | loaded with `$C014` at `$C1F754` | third of three copied workspace triples |

The direct route copies three six-byte records from `$C48390 + $42C0`,
`$C48390 + $C014`, and `$C48390 + $4480` into `$C4BF94`, then calls
`$C1FB8C`. Its observed zero result takes `$C1F780-$C1F78C`, reads the next
relative-control word after those three offsets, and sets the next control
cursor to `$C31BE4`. This proves `$C3B4F8` is syntactically a
three-triple control record prefix; it does not identify the triples, execute
the adjacent `$C3B4FE` family as code, or link it to the green mountain.

Authority: debugger-injected
`build/run037_c3b4f8_debugger_parser_probe/trace.jsonl`, trace indices 5--82,
and byte-exact walker source
[`initialize_record_walker_from_pointer.asm`](../../source_amiga/observed/initialize_record_walker_from_pointer.asm).
