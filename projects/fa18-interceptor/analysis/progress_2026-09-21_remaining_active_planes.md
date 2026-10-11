# 2026-09-21: remaining active-plane reconstruction

`source_amiga/observed/submit_remaining_active_plane_packets.asm` reconstructs
the 234-byte `$C2FDF4-$C2FEDD` continuation after the existing first active
cockpit-plane trigger. It preserves three further Custom-chip submissions at
`$C2FE3A`, `$C2FE90`, and `$C2FEDA`.

Acceptance evidence: `python scripts/verify_reconstructions.py` assembled all
139 non-overlapping source slices and matched 16,612 bytes against the
preserved baseline slow-RAM snapshot. A subsequent frame-4 breakpoint at
`$C2FEDE` returns to `$C0D742` in 2,439 no-future-input instructions, proving
the following `$C2FEDE-$C2FF45` routine separately; its raw P-code and
byte-exact source are retained in the corresponding cockpit-C2FEDE artifacts.
