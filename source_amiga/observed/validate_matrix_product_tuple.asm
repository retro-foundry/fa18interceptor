; Byte-exact observed-entry validation prefix $C2EC9C-$C2ECC5.
; Rejects out-of-bound D0/D1 components before the unobserved projection path.
                org $C2EC9C
validate_matrix_product_tuple:
 moveq #-4,d7
 bra.b .validate_components
.alternate_entry_fd:
 moveq #-3,d7
 bra.b .validate_components
.alternate_entry_fe:
 moveq #-2,d7
 bra.b .validate_components
.alternate_entry_ff:
 moveq #-1,d7
.validate_components:
 cmp.w d2,d0
 bge.b $C2EC82
 cmp.w d2,d1
 bge.b $C2EC82
 move.w d0,d3
 neg.w d3
 cmp.w d2,d3
 bge.b $C2EC82
 move.w d1,d3
 neg.w d3
 cmp.w d2,d3
 bge.b $C2EC82
 tst.w d2
 ble.b $C2EC70
