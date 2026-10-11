# run037 stable M-map runtime placement snapshot

Classification: **bounded mutable placement-cache snapshot during the visible
M-map**. It is not an immutable terrain mesh, a global coordinate export, or
a universal elevation invariant.

At the stable-map checkpoint used for the run037 renderer trace, scanning the
first 300 `$C4E9AA` 24-byte slots by their relocated `$C22000-$C22FFF`
descriptor pointer finds 111 qualifying placement records. All 111 have a
zero middle coordinate word. Their signed X range is `-1024..15872` and Z
range is `-768..10576`.

This larger cache snapshot independently agrees with the adjacent
template-to-placement trace, where 100 emitted records also had a zero middle
word. The values are mutable runtime placements, so their spread must not be
used as a map boundary or physical world-size estimate; the same static
template can produce different coordinates in different refresh contexts.

The complete [machine-readable snapshot](run037_m_map_runtime_placements.json),
[record inventory](run037_m_map_runtime_placements.md), and [X/Z plot](../plots/run037_m_map_runtime_placements_xz.svg)
retain the bounded evidence.

Authority: `build/run037_m_map_stable_13f_trace/slow.bin`, captured after
normal replay through frame 5,687 and immediately before the 13-frame stable
map trace.
