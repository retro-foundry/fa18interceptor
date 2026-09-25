; Byte-exact observed C1E540 continuation $C1E580-$C1E5DB.
; It tests an indexed record byte, prepares negated projection components, and
; conditionally invokes a secondary transform.  Data ownership remains open.

                org     $C1E580

TRANSFORM_SLOT_INDEX            equ     $C4FD5A
PREPARED_COMPONENT_X            equ     $C45A72
PREPARED_COMPONENT_Y            equ     $C45A76
PREPARE_RECORD_TRANSFORM        equ     $C1EC3A
REFINE_RECORD_TRANSFORM         equ     $C1ECFC
SECONDARY_RECORD_TRANSFORM      equ     $C1EBC0

continue_c1e540_record_transform:
                move.w  4(a0),d7
                cmpi.w  #-$1,d7
                beq.b   $C1E598
                andi.w  #$FFF,d7
                move.b  7(a0,d7.w),d7
                andi.b  #$10,d7
                bne.b   $C1E5A2
                subq.w  #1,d5
                blt.b   $C1E53C
                ; SUBA.W #$0018,A1; retain the original immediate encoding.
                dc.w    $92FC,$0018
                bra.b   $C1E562
                move.w  d5,TRANSFORM_SLOT_INDEX.l
                lea     (a1),a0
                bsr.w   PREPARE_RECORD_TRANSFORM
                move.w  PREPARED_COMPONENT_X.l,d2
                move.w  PREPARED_COMPONENT_Y.l,d4
                neg.w   d2
                neg.w   d4
                ext.l   d2
                ext.l   d4
                bsr.w   REFINE_RECORD_TRANSFORM
                move.l  d2,-$c(a6)
                move.l  d4,-$10(a6)
                move.w  (a1),d1
                btst    #4,d1
                beq.b   $C1E5DC
                bsr.w   SECONDARY_RECORD_TRANSFORM
                bra.b   $C1E5E0
