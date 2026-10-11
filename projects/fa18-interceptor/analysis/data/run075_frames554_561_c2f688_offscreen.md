# run075 frames 554–561 `$C2F688` calls

The canonical run075 playback reaches `$C2F688` at Engine frame 554 with
`D0.w=140`, `D1.w=105`, `A3=$C28710`, `A4=$C2F786`, and `D3=$2430`. The
settled Chip snapshot changes three bytes in the traced memory window, but the
host oracle images for frames 553 and 554 are pixel-identical. A second
bounded trace armed at frame 555 reaches the same renderer entry at frame 557;
the adjacent visible images remain unchanged as well.

This classifies these calls as internal or off-screen display work for the
current chunky frame gate. The entry is not promoted to a visible native HUD
operation from register names alone. The existing `$C2F688` semantic contract
still covers the proved primary and two-row handlers; this replay window adds
no new visible pixel contract.
