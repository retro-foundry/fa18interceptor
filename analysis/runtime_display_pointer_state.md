# Runtime display-pointer state

Scope: baseline menu snapshot `captures/baseline_menu/slow.bin`, with the
active display allocation cross-checked against `cockpit_bitplane_assets.md`.
This is a data classification report, not a claim that the surrounding record
is a particular AmigaOS structure.

## Observed data record

`$C1AAF4-$C1AB07` contains five consecutive 32-bit Chip-RAM pointers:

| Offset | Value | Target range |
| --- | --- | --- |
| `$00` | `$012BC0` | `$012BC0-$014AFF` |
| `$04` | `$014B00` | `$014B00-$016A3F` |
| `$08` | `$016A40` | `$016A40-$01897F` |
| `$0C` | `$018980` | `$018980-$01A8BF` |
| `$10` | `$01A8C0` | `$01A8C0-$01C7FF` |

The five starts have the fixed `$1F40` (8,000 byte) stride required by the
320x200 display buffers documented in the Copper list. They are therefore
runtime display-pointer state, not executable instructions and not immutable
on-disk pixel data. The captured Copper section proves only the first four
planes active; the fifth adjacent pointer is retained as structural state and
is not asserted to be active in that display mode.

`$C1AA9C-$C1AADB`, immediately before this pointer run, is the exact 32-word
RGB4 palette shared by `pix/inst5` and `pix/frnt5`; see
`disk_graphics_assets.md`. The adjacent values show that this Slow-RAM region
holds decoded/active display state, but they do not prove an ownership or
presentation order for either source ILBM.

## Proven producer

When `$C457D6` is nonzero, `$C1693A-$C1697E` loads the object pointer held at
`$C1AADC` and copies its five longwords at offsets `$08,$0C,$10,$14,$18` into
`$C1AAF4,$C1AAF8,$C1AAFC,$C1AB00,$C1AB04`, respectively. This proves the
record is a mutable five-pointer cache derived from another runtime object.
It does not prove that every cached entry is enabled by the Copper list.

The static producer chain for `$C1AADC` is also bounded: `$C0E2E8` pushes the
`df0:pix/splsh` identifier, `$C0E2EE` calls `$C0E078`, and `$C0E2F6` stores
that helper's `D0` result at `$C1AADC`. This ties the on-disk splash resource
to an allocated/decoded runtime object and then to the BSS pointer cache. It
does not prove when this chain ran in the preserved baseline snapshot or that
the resulting object was the Copper-visible image at any chosen frame.

For comparison, the same five-pointer shape occurs at `$C18272` and
`$C456BE`, where it names a separate `$04DB30-$05776F` five-plane allocation.
That comparison is intentionally structural: it identifies pointer-bearing
runtime data records without relabelling them as code or immutable assets.

## Exclusions

- The report does not classify either Chip-RAM allocation as an on-disk asset.
- It does not claim that `$C0E078` writes this record; startup trace evidence
  is unavailable because preserved cold-boot runs remain in the ROM idle loop.
- It does not infer the exact record type from its layout alone.
