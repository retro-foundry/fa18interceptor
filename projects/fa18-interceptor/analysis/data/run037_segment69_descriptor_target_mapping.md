# Verified Hunk-69 run037 descriptor targets

Classification: **verified immutable descriptor-target subset**. This is neither a complete terrain mesh nor a whole-hunk code/data reclassification.

Original CODE Hunk 69 is byte-exactly verified at `$C44500-$C44877` (888 bytes), including all 16 self-relocation operands. The rows below are its fields reached by the run037 terrain-template placement catalog.

| target | Hunk offset | placements | static template sources | descriptors | observed `$C1CC70` write |
| --- | --- | ---: | --- | --- | --- |
| $C44500 | $0000 | 1 | $C427D5 | $C22764 | no |
| $C4455A | $005A | 1 | $C427E1 | $C22778 | no |
| $C445A2 | $00A2 | 1 | $C42665 | $C22318 | no |
| $C445DC | $00DC | 3 | $C42647, $C42B6D, $C42B73 | $C2232C | yes |
| $C44612 | $0112 | 2 | $C4267D, $C42B79 | $C227F0 | no |
| $C4466E | $016E | 1 | $C4269B | $C22354 | yes |
| $C44732 | $0232 | 1 | $C42659 | $C22804 | yes |
| $C4477C | $027C | 1 | $C426A1 | $C22368 | no |
| $C447C6 | $02C6 | 1 | $C4265F | $C22818 | no |
| $C44812 | $0312 | 1 | $C426BF | $C2237C | no |
| $C4483C | $033C | 1 | $C426C5 | $C22390 | no |

Every row is a descriptor +8 field present in the run037 template-placement catalog. A field is a renderer-control candidate; it is not by itself a vertex list, terrain-cell payload, or proof that a particular control stream was rasterized.
