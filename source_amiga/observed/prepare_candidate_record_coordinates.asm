; Byte-exact observed candidate-coordinate preparation $C270AE-$C27155.
; It normalizes a three-component candidate, conditionally applies one of two
; record-relative offsets, and selects a later table route. Record semantics
; and the `$C458DE` selector's meaning remain unresolved.

                org     $C270AE

RECORD_SELECTOR                 equ     $C458DE
RECORD_SELECTOR_COMPARE         equ     $C459B6
RECORD_BANK_BASE                equ     $C46184
SPECIAL_TABLE_BASE              equ     $C39168
CONTINUE_CANDIDATE_ROUTE        equ     $C2715C
NON_SPECIAL_CANDIDATE_ROUTE     equ     $C27156

prepare_candidate_record_coordinates:
                move.w  d0,-(sp)
                clr.w   -$44(a6)
                bra.s   .save_components
.retry_saved_components:
                move.w  (sp),d0
                move.w  RECORD_SELECTOR.l,d7
                cmp.w   RECORD_SELECTOR_COMPARE.l,d7
                bne.w   $C2718C
                tst.w   -$44(a6)
                bgt.w   $C2718C
                addq.w  #1,-$44(a6)
                movem.l -$2e(a6),d2-d4
                bra.s   .normalize_components
.save_components:
                movem.l d2-d4,-$2e(a6)
.normalize_components:
                andi.l  #$003FFFFF,d2
                andi.l  #$003FFFFF,d4
                asr.l   #8,d2
                asr.l   #8,d3
                asr.l   #8,d4
                move.w  RECORD_SELECTOR.l,d7
                cmp.w   RECORD_SELECTOR_COMPARE.l,d7
                bne.s   .publish_components
                lea     RECORD_BANK_BASE.l,a3
                adda.w  d7,a3
                move.b  $7d(a3),d5
                andi.w  #$f,d5
                tst.w   -$44(a6)
                bgt.s   .select_second_offset
                ; VASM shortens immediate ADDA.W; retain original encoding.
                dc.w    $D6FC,$00A4             ; adda.w #$00A4,a3
                bra.s   .apply_record_offset
.select_second_offset:
                dc.w    $D6FC,$00B6             ; adda.w #$00B6,a3
.apply_record_offset:
                move.w  (a3),d7
                asr.w   d5,d7
                add.w   d7,d2
                move.w  2(a3),d7
                asr.w   d5,d7
                ext.l   d7
                add.l   d7,d3
                move.w  4(a3),d7
                asr.w   d5,d7
                add.w   d7,d4
.publish_components:
                movea.w d2,a2
                movea.l d3,a1
                move.w  d4,-$42(a6)
                lea     (a0,d0.w),a3
                cmpi.b  #$20,$62(a3)
                bne.s   NON_SPECIAL_CANDIDATE_ROUTE
                lea     SPECIAL_TABLE_BASE.l,a4
                bra.s   CONTINUE_CANDIDATE_ROUTE
