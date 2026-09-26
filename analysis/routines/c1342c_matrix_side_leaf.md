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

The same frame-949 packet identifies the update arithmetic for its first
working input. At `$C13BDC`, the leaf addresses root `+$56`; the local target
at `+$0A(A6)` is `$FFD3` (signed −45). The exercised instructions
`$C13BE6-$C13BF8` perform:

```text
old = $FFFA  (−6)
delta = arithmetic_shift_right(old − target, 2) = (39 >> 2) = 9
new = old − delta = $FFF1  (−15)
```

Thus the first component is a one-quarter relaxation toward a leaf-computed
target for this packet. The target's upstream source and its physical/control
meaning are still unassigned.

For this exact target, the immediate provenance is also observed. The selected
signed lane local is `−2`; `$C135BE-$C135CC` takes its absolute value, doubles
it to word offset `+$0004`, reads `$002D` from the zero-index table rooted at
`$C3D690`, and negates it before storing `$FFD3` at `$C45B5E`. That value is
passed to `$C13BA0` and becomes the smoothing target above. This establishes a
table-driven target lane, not the table's coordinate convention or gameplay
meaning.

The lane itself is root record byte `+$28`, loaded and sign-extended at
`$C1357A-$C13590`. In this frame it is `$FE` (signed −2), while the adjacent
`+$29/+2A` byte lanes are both zero. Therefore the fully bounded first-input
path is:

```text
root +$28 = −2
  -> $C3D690 word lane +4 = $002D
  -> negate to target −45
  -> quarter-step root +$56: −6 -> −15
  -> $C2DEE0 / $C2D954 produce and publish $7070
```

This is an executed numerical dependency chain for the initial run060 turn;
the three byte lanes' physical axes remain unassigned.

## run060 third-lane working output

At frame 1754, this leaf runs after `$C1B410` has changed root `+$2A` to −3.
The bounded `$C1342C` packet changes its working triple at root
`+$56/+58/+5A` from `$0000/$0000/$0000` to `$0000/$0000/$0013`. The following
`$C2D620/$C2D624/$C2D628` load supplies that same `$0013` as the third
`$C2DEE0` input, which returns the later published `$0010` third orientation
component. This proves the packet-local `+$2A`-window to working-third-input
handoff; the exact arithmetic inside the wider leaf still needs a bounded
instruction-level derivation.

That arithmetic is now bounded for the same packet. `$C1386A` reads
`$C45B62=$FFC1` (signed −63), negates it, and `$C13C64-$C13C80` derives
`(63 >> 1) + (63 >> 3) = 31 + 7 = 38`. `$C13CBE-$C13CCE` then applies a
one-half relaxation to working `+$5A`:

```text
old = 0
target = 38
new = old - ((old - target) >> 1) = 19 = $0013
```

The upstream producer of `$C45B62` is not yet bounded to root control lane
`+$2A`; this derivation establishes the exact local working-output arithmetic.
