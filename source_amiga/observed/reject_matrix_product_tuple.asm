; Byte-exact $C2EC82-$C2EC8F shared rejected-tuple return for $C2EC9C.
                org $C2EC82
PRODUCT_TUPLE_RESULT equ $C45958
reject_matrix_product_tuple:
 moveq #0,d0
 move.l #-1,PRODUCT_TUPLE_RESULT.l
 rts
