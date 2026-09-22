# Candidate scene-table Hunks

Candidate table evidence is not a code-to-data reclassification.

| Segment | Original kind | Runtime | Bytes | Segment-16 pointer sites | Cross-scene incoming relocations |
| ---: | --- | --- | ---: | ---: | --- |
| 41 | CODE | $C34A50 | 2,832 | 0 | none |
| 42 | CODE | $C35568 | 3,224 | 21 | none |
| 43 | CODE | $C36208 | 2,068 | 15 | none |
| 44 | CODE | $C36A28 | 2,024 | 6 | none |
| 45 | CODE | $C08718 | 724 | 28 | none |
| 46 | CODE | $C37218 | 1,120 | 12 | 47:13, 48:10, 49:20, 50:16 |
| 47 | CODE | $C37680 | 772 | 9 | none |
| 48 | CODE | $C0DB50 | 608 | 10 | none |
| 49 | CODE | $C37990 | 1,512 | 26 | none |
| 50 | CODE | $C37F80 | 1,124 | 12 | none |

The segment-16 counts are relocation-backed pointer sites in the exact `$C223A8-$C227EB` inline-data table. Adjacent three-pointer records can target several Hunks, so per-target site counts must not be interpreted as record counts. Cross-scene links expose static storage families, but do not establish whether a target is instructions, tables, mixed content, or a named model. Consumer tracing is required before extraction or reclassification.
