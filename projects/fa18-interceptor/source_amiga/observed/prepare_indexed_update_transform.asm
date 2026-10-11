; Byte-exact observed indexed update/transform setup $C149BE-$C14A81.
; The subsequent bit-7 path and all later update logic remain raw.

                org     $C149BE

ACTIVE_UPDATE_SELECTOR          equ     $C459B4
INDEXED_UPDATE_RECORD_BASE      equ     $C46184
ACTIVE_UPDATE_RECORD_POINTER    equ     $C18210
TRANSFORM_INPUT_X               equ     $C45AA0
TRANSFORM_INPUT_Y               equ     $C45AA2
TRANSFORM_INPUT_Z               equ     $C45AA4
TRANSFORM_OUTPUT_X              equ     $C45A4C
TRANSFORM_OUTPUT_Y              equ     $C45A4E
TRANSFORM_OUTPUT_Z              equ     $C45A50
CALL_INDEXED_UPDATE_TRANSFORM   equ     $C25754
CONTINUE_INDEXED_UPDATE_FLAG7   equ     $C14AF2

prepare_indexed_update_transform:
                link.w  a6,#-$36
                movem.l d2-d4/a2-a3,-(a7)
                move.w  ACTIVE_UPDATE_SELECTOR.l,d0
                moveq   #9,d1
                move.w  d0,-$24(a6)
                ext.l   d0
                asl.l   d1,d0
                movea.l d0,a0
                adda.l  #INDEXED_UPDATE_RECORD_BASE,a0
                move.l  a0,d0
                move.l  d0,ACTIVE_UPDATE_RECORD_POINTER.l
                movea.l d0,a0
                move.l  a0,-$28(a6)
                addq.l  #2,a0
                movea.l d0,a1
                addq.l  #4,a1
                movea.l d0,a2
                dc.w    $D4FC,$0020 ; adda.w #$20,a2; retain original non-relaxed opcode
                movea.l d0,a3
                move.w  $96(a3),d1
                move.w  d1,TRANSFORM_INPUT_X.l
                move.w  $96(a3),d2
                move.w  d2,TRANSFORM_INPUT_X.l
                move.w  $9c(a3),d1
                move.w  d1,TRANSFORM_INPUT_Y.l
                move.w  $a2(a3),d2
                move.w  d2,TRANSFORM_INPUT_Z.l
                move.w  $6e(a3),d1
                neg.w   d1
                move.w  d1,-$22(a6)
                ext.l   d1
                move.w  TRANSFORM_INPUT_X.l,d3
                ext.l   d3
                move.w  TRANSFORM_INPUT_Y.l,d4
                ext.l   d4
                ext.l   d2
                move.l  d2,-(a7)
                move.l  d4,-(a7)
                move.l  d3,-(a7)
                move.l  d1,-(a7)
                move.l  a0,-$2c(a6)
                move.l  a1,-$30(a6)
                move.l  a2,-$34(a6)
                jsr     CALL_INDEXED_UPDATE_TRANSFORM.l
                lea.l   $10(a7),a7
                move.w  TRANSFORM_OUTPUT_X.l,-2(a6)
                move.w  TRANSFORM_OUTPUT_Y.l,-4(a6)
                move.w  TRANSFORM_OUTPUT_Z.l,-6(a6)
                movea.l -$2c(a6),a0
                move.w  (a0),d0
                btst.l  #7,d0
                bne.b   CONTINUE_INDEXED_UPDATE_FLAG7
