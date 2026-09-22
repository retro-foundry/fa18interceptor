# Disk graphics assets

The original ADF contains these immutable `FORM ILBM` resources. They are distinct from the mutable Chip-RAM display targets documented in `cockpit_bitplane_assets.md` and the pointer-bearing runtime data records in `runtime_display_pointer_state.md`.

| ADF path | Size | Geometry | Planes | Identifier Hunk | Runtime string | SHA-256 |
| --- | ---: | --- | ---: | --- | --- | --- |
| `pix/frnt5` | 988 | 288×12 | 5 | `DATA 1+$8D` | `$C07FC5` | `7cf5300d75a10f0f7727506a91f2eb46968a625fdbbc9f13e13c88b26a52d012` |
| `pix/inst5` | 7,230 | 320×55 | 5 | `DATA 1+$7F` | `$C07FB7` | `870329e081934311588fe90d28897a82c775a39e22f6f4525f163502a008889c` |
| `pix/splsh` | 23,224 | 320×200 | 5 | `DATA 1+$71` | `$C07FA9` | `d64e541d3e91808dd10a9e934216f1f28601f7e2c7c7a8db8b9d4845a47e1c70` |

## Runtime palette evidence

- `pix/frnt5` RGB4 CMAP: `$C1AA9C`.
- `pix/inst5` RGB4 CMAP: `$C1AA9C`.
- `pix/splsh` RGB4 CMAP: `no exact baseline match`.

`pix/inst5` and `pix/frnt5` have identical 32-word RGB4 CMAP data, and the baseline snapshot contains that exact sequence at `$C1AA9C`. This establishes `$C1AA9C-$C1AADB` as mutable decoded palette state, not executable code or a disk-pixel payload. A match does not alone establish which file was loaded most recently.

## Static loader references

- `pix/frnt5`: CODE 0+$6EA (relocated extension `$C0E59A`).
- `pix/inst5`: CODE 0+$6CA (relocated extension `$C0E57A`).
- `pix/splsh`: CODE 0+$43A (relocated extension `$C0E2EA`).

Each listed address is the relocated longword extension that names the full `df0:pix/...` path. The three references occur as `PEA` arguments to `$C0E078`: `$C0E2EE` for `splsh`, `$C0E57E` for `inst5`, and `$C0E59E` for `frnt5`. Static disassembly shows that helper reads the bitmap header, allocates a plane-pointer array, and reads plane-sized data. It is therefore an observed ILBM resource-loading path; the callback arguments and eventual display presentation remain unassigned.

The loader-return slots are `$C1AADC` for `splsh`, `$C1AB08` for `inst5`, and `$C1AB38` for `frnt5`; the latter two calls share palette destination `$C1AA9C`. For `splsh`, `$C1693A-$C1697E` later copies five pointer fields from the `$C1AADC` object into BSS cache `$C1AAF4-$C1AB04`. See `analysis/runtime_display_pointer_state.md`. This establishes static resource-to-runtime-object-to-cache dataflow, not a frame-specific presentation claim.

Run the recorded extraction command to write byte-for-byte ILBM copies to an explicit directory. The inventory itself does not duplicate copyrighted source pixels into the repository.


The separate BSS-object slot and pointer-cache paths for all three resources are documented in `analysis/runtime_ilbm_object_slots.md`.
