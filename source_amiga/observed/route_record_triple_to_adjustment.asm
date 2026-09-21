; Byte-exact observed post-load routing $C2D6C6-$C2D6FB.

                org     $C2D6C6

RECORD_PATH_GATE                equ     $C457AE
RECORD_PATH_AUXILIARY           equ     $C457AF
ACTIVE_UPDATE_SELECTOR          equ     $C459B4
RECORD_ADJUSTMENT_ENABLE        equ     $C4578F
CALL_RECORD_DEPTH_ADJUSTMENT    equ     $C2DD4E

route_record_triple_to_adjustment:
                move.b  RECORD_PATH_AUXILIARY.l,d7
                or.b    RECORD_PATH_GATE.l,d7
                bne.w   $C2D630
                move.b  $62(a1),d3
                andi.b  #$f0,d3
                cmpi.b  #$10,d3
                bne.b   $C2D6FC
                tst.w   ACTIVE_UPDATE_SELECTOR.l
                bne.b   $C2D6F4
                tst.b   RECORD_ADJUSTMENT_ENABLE.l
                beq.b   $C2D6FC
                move.w  $6a(a1),d7
                bsr.w   CALL_RECORD_DEPTH_ADJUSTMENT
