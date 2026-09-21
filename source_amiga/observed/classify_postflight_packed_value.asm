; Byte-exact static-only packed-value classification $C3341A-$C3345B.
                org     $C3341A
POSTFLIGHT_VALUE_STORE equ $C45B1E
POSTFLIGHT_PACKED_VALUE equ $C45B22
POSTFLIGHT_HELPER equ $C25A08
POSTFLIGHT_CONTINUE equ $C33476
classify_postflight_packed_value:
                divu.w  #$A,d0
                ext.l   d0
                moveq   #0,d2
                move.l  d0,POSTFLIGHT_VALUE_STORE.l
                jsr     POSTFLIGHT_HELPER.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,d0
                move.l  d0,d1
                andi.w  #$FF0F,d0
                andi.w  #$FF,d1
                cmpi.w  #$20,d1
                ble.s   POSTFLIGHT_CONTINUE
                cmpi.w  #$70,d1
                bgt.s   postflight_packed_high
                ori.w   #$50,d0
                ; MOVE.L #-$32,D2; retain the original non-MOVEQ encoding.
                dc.w    $243C
                dc.l    -$32
                bra.s   POSTFLIGHT_CONTINUE
postflight_packed_high:
                ; MOVE.L #-$64,D2; retain the original non-MOVEQ encoding.
                dc.w    $243C
                dc.l    -$64
