; Byte-exact observed record-group copy tail $C1EB54-$C1EBAF.
; It copies six-longword groups through two count-controlled loops, conditionally
; masking one source word before copying.  Group ownership remains structural.

                org     $C1EB54

TRANSFORM_SLOT_INDEX            equ     $C4FD5A
TRANSFORM_SLOT_BASE             equ     $C4FD5C

copy_c1eb2c_transformed_record_groups:
                ; ADDA.W #$0018,A3; retain the original immediate encoding.
                dc.w    $D6FC,$0018
                dbra    d7,$C1EB2C
                move.w  TRANSFORM_SLOT_INDEX.l,d0
                sub.w   TRANSFORM_SLOT_BASE.l,d0
                asl.w   #3,d0
                move.w  d0,d1
                add.w   d0,d0
                add.w   d1,d0
                movem.l (a1,d0.w),d0-d5
                movem.l d0-d5,(a2)
                ; ADDA.W #$0018,A2; retain the original immediate encoding.
                dc.w    $D4FC,$0018
                move.w  d6,d7
                tst.w   (a1)
                blt.b   $C1EBA4
                move.w  2(a1),d0
                andi.w  #$C000,d0
                cmpi.w  #$8000,d0
                bne.b   $C1EBA4
                andi.w  #$FFF,2(a1)
                movem.l (a1),d0-d5
                movem.l d0-d5,(a2)
                dc.w    $D4FC,$0018 ; ADDA.W #$0018,A2
                dc.w    $D2FC,$0018 ; ADDA.W #$0018,A1
                dbra    d7,$C1EB80
                unlk    a6
                rts
