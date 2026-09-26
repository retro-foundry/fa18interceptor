# Matrix-side leaf at `$C1342C`

Classification: **bounded structural leaf**. In the sealed run003 frame-6,000
matrix-side packet, `$C2D618` calls `$C1342C` and it returns to `$C2D61E`
after 103 instructions. Future playback is empty. Canonical P-code is
`pcode/raw/run003_6000_c1342c/` (103 RAM instruction starts, 487 operations,
no nested calls). Its arithmetic and record ownership remain unassigned.

The observed route publishes `$C46184 + ($C459B4 << 9)` at `$C18210`, selects
the static table base `$C3D690` when the index is zero, and returns through
`$C13A22`.

Three contiguous exercised fragments are now byte-exact source:

- `initialize_matrix_side_record.asm` (`$C1342C-$C13487`) selects the
  512-byte record and derives local pointers at `+$02/$56/$58/$5A`.
- `select_zero_matrix_side_table.asm` (`$C13490-$C13499`) publishes the
  zero-index table local.
- `clear_matrix_side_record_flags.asm` (`$C134A2-$C134BB`) clears record-header
  mask `$0040` and gates on `$C458CC` bit 6.
- The four `matrix_side_*threshold` and `clear_matrix_side_status_mask` slices
  retain the observed positive-index path at `$C134BC`, `$C1350A`, `$C1353A`,
  and `$C1356A` without filling the alternate-branch gaps.
- `load_matrix_side_component_prefix.asm` and
  `gate_matrix_side_first_component.asm` sign-extend the selected record's
  `+$28/$29/$2A` bytes and preserve the two exercised zero-component fragments.
- The `matrix_side_*component*` slices at `$C136BE-$C137C9` preserve the
  observed first/second output clears and all subsequent zero-component jumps,
  without claiming their skipped nonzero alternatives.
- The `$C13952-$C13A29` slices clear the third output, apply the trace's final
  selected-record/status gates, and retain the observed epilogue.

The enclosing static routine remains substantially wider than the live packet.
Untraced branch gaps and later paths are deliberately not reconstructed.

## run060 first-angle source instance

At replay frame 949, the active root record is class `$10` and `$C458CC` has
bit 6 set. `$C2D5FC` therefore calls this leaf before loading its matrix-product
input triple. The bounded trace returns to `$C2D6C6` after 176 instructions;
the immediately following `$C2D620/$C2D624/$C2D628` loads give:

```text
A1 = $C46184
D0/D2/D4 = $FFF1 / $0000 / $0000
```

That tuple is passed through `$C2D6FC` into `$C2DEE0`, which normalizes its
negative first component before deriving the first run060 pitch-like output.
This establishes `$C1342C` as the preceding generator/update stage for this
specific matrix-product input. It does not establish that record `+$56` has a
persistent angle meaning: the pre/post snapshots of the larger bounded packet
need not retain the temporary value read at `$C2D620`.
