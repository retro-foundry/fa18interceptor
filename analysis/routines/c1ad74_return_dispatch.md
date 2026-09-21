# Raw-key `$44` Return prefix at `$C1AD74`

Classification: **capped command prefix**, bound to the documented Return
weapon-selection event in sealed `run002`.

## Runtime evidence

- State: replay through frame 5,171 before `K 13 0 16 1` (Return).
- The isolated event reaches `$C1AD74` at replay frame 2 with raw byte `$44`.
- `$C1AE34` selects `$C1BB7A`; the measured route tests `$C458A6` and calls
  `$C33186`.
- The trace was stopped at the 1,000-instruction cap without reaching
  `$C0F45C`. P-code is retained at `pcode/raw/run002_return_key_dispatch/`
  (172 observed RAM starts / 946 operations).

This maps the recorded key to its dispatch entry but is not a completed
function or scenario contract. The first callee has subsequently been bounded
as `$C33186 -> $C1BBC0`; see `analysis/routines/c33186_return_inner_packet.md`.
Do not infer the Return key's game effect from this packet alone.
