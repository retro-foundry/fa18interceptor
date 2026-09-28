# Original static data bridge for the map packet pass

`map_packet_static_data.{c,h}` binds the immutable source bytes consumed as
data by the already ported `$C2AB34-$C2AFF9` map-pass boundary.

- Hunk 28 is verified at `$C29F00-$C2B3B4`; it provides the signed selector
  byte pairs and the `$C2A0xx` control streams.
- Hunk 68 is verified at `$C42CA8-$C444F8`; it provides the packet directory
  and static pair payload.

The bridge converts a bounded runtime address to the corresponding original
Hunk byte pointer and remaining range. Requests below either base or at/after
the Hunk end fail. This makes the original static inputs available to a native
pass owner without retaining emulator memory or captured output.

Authority: `analysis/runtime_region_manifest.json`,
`analysis/routines/c2ad80_map_segment68_directory_lookup.md`, and
`analysis/routines/c2af00_map_static_pair_packet.md`.
