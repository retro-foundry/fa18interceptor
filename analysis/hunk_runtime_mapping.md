# Original Hunk to runtime mapping: segment 36 anchor

The private extraction of the original disk executable is SHA-256
`d7301e20958548f9b5f1ba744f8ff91fef476306c220eb886e7c90bac2802ffa`.
`scripts/parse_hunk.py` identifies 185 Hunk segments: 126 CODE, 32 DATA and 27
BSS. This supersedes the prior minimum-only inference from the WHDLoad patch.

An unchanged 12-byte instruction window at runtime `$C2FB4E` maps uniquely to
original CODE segment 36, file offset `$2920A`, segment offset `$6BE`. Its
runtime payload base is therefore `$C2F490`. `scripts/map_loaded_segment.py`
verifies every non-relocated byte of all 4,644 bytes in that segment and all 223
32-bit relocations.

| Original segment | Inferred runtime payload base | References from segment 36 | Status |
| ---: | ---: | ---: | --- |
| 33 | `$C0D720` | 2 | consistent relocation equation |
| 36 | `$C2F490` | 42 | verified full segment |
| 37 | `$C306C0` | 1 | consistent relocation equation |
| 62 | `$012988` | 2 | consistent relocation equation |
| 71 | `$C45630` | 168 | consistent relocation equation |
| 72 | `$C48390` | 8 | consistent relocation equation |

Segment 71 gives an exact local confirmation of the WHDLoad landmark in
`GAME.md`: segment 71 + decimal 357 is `$C45795`. Its observed value is `0` in
the menu snapshots and `1` in the two in-flight demo snapshots. The evidence
supports the provisional field name `text_vs_normal_state`; it does not prove a
complete mode enumeration or every non-zero meaning.

These bases are specific to the pinned A500 runtime and are payload addresses.
They do not substitute for original Hunk segment identity or imply that another
memory configuration loads at the same addresses.

## Relocation-closure result

`scripts/resolve_hunk_runtime.py` starts from the verified segment-36 payload
base and follows only relocation groups whose runtime equations agree. In the
frame-600 in-flight snapshot it resolves 120 original segments with no base
conflicts: 101 are byte-verified outside their declared relocation fields, 13
are runtime-mutated or otherwise not byte-verifiable, and six are BSS. The
full machine-readable map is `analysis/hunk_runtime_resolved.json`.

This converts the working address map from a flat `$C00000` dump into stable
original-segment identities. Runtime addresses must now be recorded as both a
CPU address and `segment_index + payload_offset` whenever the segment is in
this verified closure.
