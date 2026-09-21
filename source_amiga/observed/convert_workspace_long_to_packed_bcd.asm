; Byte-exact runtime-backed packed-decimal conversion helper $C25A08-$C25A3D.
; Input: WORKSPACE_INPUT.l. Output: WORKSPACE_PACKED_BCD.l.
; The unsigned input remains in the input workspace; the routine repeatedly
; subtracts decimal place values and accumulates packed BCD in D7.

WORKSPACE_INPUT             equ     $C45B1E
WORKSPACE_PACKED_BCD        equ     $C45B22
PACKED_BCD_PLACE_TABLE      equ     $C25A3E

                org     $C25A08

convert_workspace_long_to_packed_bcd:
                movem.l d5-d7/a6,-(a7)
                lea.l   WORKSPACE_INPUT.l,a6
                move.l  (a6),d5
                lea.l   PACKED_BCD_PLACE_TABLE.l,a6
                moveq   #0,d7
                move.l  #$10000000,d6
convert_workspace_bcd_digit:
                cmp.l   (a6),d5
                bcs.s   convert_workspace_bcd_next_place
                sub.l   (a6),d5
                add.l   d6,d7
                bra.s   convert_workspace_bcd_digit
convert_workspace_bcd_next_place:
                addq.l  #4,a6
                lsr.l   #4,d6
                bne.s   convert_workspace_bcd_digit
                move.l  d7,WORKSPACE_PACKED_BCD.l
                movem.l (a7)+,d5-d7/a6
                rts
