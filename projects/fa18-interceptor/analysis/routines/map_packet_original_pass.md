# `$C2AB34-$C2AFF9` original-data map-pass composition

`map_packet_original_pass.{c,h}` binds the existing map-pass runner to the
original Hunk 28/Hunk 68 resolver. For each control byte it copies the
caller-owned live record input, rebases its directory pointer from Hunk 68 to
the selected normal (`$C42CA8`) or wide (`$C42E6C`) source base, and supplies
the original selector-pair and packet-address resolvers.

Thus the native route preserves:

```text
original control stream → selector byte pair → rebased source directory
→ original static packet → pair transform → caller-owned `$C246A0` display
```

The parent still owns its live coordinate terms, depth/detail fields, display
matrix, page identity, and submission callbacks. This composition does not
introduce a frame trigger or captured runtime state.
