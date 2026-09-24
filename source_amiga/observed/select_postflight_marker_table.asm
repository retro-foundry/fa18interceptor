; Byte-exact C348B2-C348FF table selection prefix.
                org $C348B2
RETURN equ $C348B0
JOIN equ $C34900
TABLE_A equ $C349D0
TABLE_B equ $C349EA
FRAME_WORD equ $C458DA
select_postflight_marker_table:
                tst.w d4
                bne.b alternate
                cmpi.w #$58,d0
                ble.b RETURN
                cmpi.w #$E6,d0
                bge.b RETURN
                cmpi.w #$27,d1
                ble.b RETURN
                cmpi.w #$8D,d1
                bge.b RETURN
                lea TABLE_A.l,a0
                bra.b JOIN
alternate:       move.w FRAME_WORD.l,d2
                andi.w #3,d2
                beq.b RETURN
                cmpi.w #$60,d0
                ble.b RETURN
                cmpi.w #$DE,d0
                bge.b RETURN
                cmpi.w #$2E,d1
                ble.b $C34948
                cmpi.w #$86,d1
                bge.b $C34948
                lea TABLE_B.l,a0
