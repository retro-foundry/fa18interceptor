# `$C2AF00` map static-pair packet transform

Classification: **scenario-backed static-packet-to-polygon-workspace path**.

During preparation of the pending `M`-map page, direct entries from
`$C2AEFC` to `$C2AF00` begin with `A3` in verified original Hunk 68,
`$C42CA8-$C444F7`; all captured packet and pair reads are within that same
6,224-byte segment. It has zero non-relocation byte differences from the
original executable in the resolved runtime mapping. The routine reads its source through `A3`, applies
the live fixed-point display matrix rooted at `$C45BDA`, and writes only
mutable `$C4BFxx` polygon-input workspace. `$C246A0` consumes that workspace;
its projected results reach `$C2FF48` on observed packets.

The bounded map trace contains 26 such direct packet entries and 55 completed
transform batches. The transform loop reads 353 exact signed two-word source
pairs at `$C2AF9C/$C2AF9E`; all 55 batches reach the `$C2AFE2 -> $C246A0`
display-stage call. The direct header is only captured on 26 batches because
the remaining batches resume through an enclosing packet path. The packet
source/header fields and every consumed pair
are retained in the [static-packet inventory](../data/run003_m_map_polygon_static_packets.md).
The accompanying [raw-coordinate sheet](../plots/run003_m_map_static_pair_packets.png)
is a visual inspection aid only; it preserves read order without inferring
closed faces or screen/world alignment.

This establishes that `$C4BFxx` workspace records are derived from immutable
map-mode packet input. It does **not** decode the packet's full grammar, prove
that every pair is terrain/coastline rather than an overlay, place the pairs
in global flight coordinates, or establish that the bounded 26 packets are
the complete map.

Authority: sealed `captures/run003`,
`build/run003_m_map_appearance_trace/{trace.jsonl,slow.bin}`, and:

```text
python scripts/inventory_map_polygon_static_packets.py \
  --trace build/run003_m_map_appearance_trace/trace.jsonl \
  --slow build/run003_m_map_appearance_trace/slow.bin \
  --output analysis/data/run003_m_map_polygon_static_packets.json
```
