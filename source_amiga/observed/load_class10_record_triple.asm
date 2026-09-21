; Byte-exact observed $10-class record triple load $C2D5FC-$C2D62F.

                org     $C2D5FC

UPDATE_CONTROL_WORD             equ     $C458CC
CALL_CLASS10_RECORD_UPDATE      equ     $C1342C
CONTINUE_RECORD_POSTLOAD        equ     $C2D6C6

load_class10_record_triple:
                move.b  $62(a1),d0
                andi.b  #$f0,d0
                cmpi.b  #$10,d0
                bne.b   $C2D63A
                move.w  UPDATE_CONTROL_WORD.l,d0
                andi.w  #$40,d0
                beq.b   $C2D620
                move.l  a1,-(a7)
                jsr     CALL_CLASS10_RECORD_UPDATE.l
                movea.l (a7)+,a1
                move.w  $56(a1),d0
                move.w  $58(a1),d2
                move.w  $5a(a1),d4
                bra.w   CONTINUE_RECORD_POSTLOAD
