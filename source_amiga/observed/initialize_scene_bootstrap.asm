; Byte-exact cold-boot initialization body $C08F26-$C090AD.
; Runtime authority: cold_boot_menu_init frame 7769 instruction trace.

                org     $C08F26

CALL_C090C2                     equ $C090C2
CALL_C090F2                     equ $C090F2
CALL_C2FD22                     equ $C2FD22
CALL_C09620                     equ $C09620
CALL_C0910C                     equ $C0910C
CALL_C0915A                     equ $C0915A
CALL_C09266                     equ $C09266

SCENE_RECORD_BANK               equ $C46184
SCENE_RECORD_COUNT              equ 16
SCENE_RECORD_STRIDE             equ $200
SCENE_RECORD_CLEAR_LONGS        equ 41
SCENE_WORK_BANK                 equ $C48184
SCENE_WORK_STRIDE               equ $20
SCENE_WORK_CLEAR_LONGS          equ 8

initialize_scene_bootstrap:
                bsr.w   CALL_C090C2
                bsr.w   CALL_C090F2
                move.b  $C458A8.l,$C458A7.l
                clr.w   $C4FDA2.l
                clr.w   $C4FDA0.l
                clr.l   $C4574A.l
                moveq   #$FF,d0
                move.b  d0,$C45858.l
                move.b  d0,$C457E0.l
                move.b  d0,$C458AE.l
                move.w  #5,$C45AD6.l
                move.l  #$1B8,$C4573E.l
                jsr     CALL_C2FD22.l

                lea.l   SCENE_RECORD_BANK.l,a0
                moveq   #SCENE_RECORD_COUNT-1,d0
.clear_record:
                moveq   #SCENE_RECORD_CLEAR_LONGS-1,d4
                lea.l   (a0),a1
.clear_record_words:
                clr.l   (a1)+
                dbra    d4,.clear_record_words
                ; VASM folds this immediate ADDA to LEA; retain the source
                ; `$D0FC,$0200` encoding.
                dc.w    $D0FC,SCENE_RECORD_STRIDE
                dbra    d0,.clear_record

                lea.l   SCENE_WORK_BANK.l,a0
                moveq   #SCENE_RECORD_COUNT-1,d0
.clear_work:
                moveq   #SCENE_WORK_CLEAR_LONGS-1,d4
                lea.l   (a0),a1
.clear_work_words:
                clr.l   (a1)+
                dbra    d4,.clear_work_words
                ; Preserve `$D0FC,$0020`, rather than VASM's LEA fold.
                dc.w    $D0FC,SCENE_WORK_STRIDE
                dbra    d0,.clear_work

                bsr.w   CALL_C09620
                move.w  #$140,$C459A6.l
                move.w  #$140,$C459A8.l
                move.b  #0,$C45848.l
                move.b  #1,$C45857.l
                move.w  #$800,$C4FDD2.l
                bsr.w   CALL_C0910C
                bsr.w   CALL_C0915A
                move.l  #$03000000,$C45664.l
                clr.l   $C45C4A.l
                clr.l   $C45C4E.l
                clr.l   $C45C52.l
                move.w  #$1C20,$C45A94.l
                ; Preserve MOVE.W #0 absolute-long (`$33FC`) over CLR.W.
                dc.w    $33FC,$0000
                dc.l    $C45A96
                move.w  #$A7,$C45984.l
                move.w  #$32,$C45986.l
                move.w  #$320,$C45988.l
                moveq   #$FF,d0
                move.l  d0,$C45AFA.l
                move.l  d0,$C45B02.l
                move.b  d0,$C45855.l
                move.b  #$F,$C458BE.l
                move.w  #$7FFF,$C45AE8.l
                move.l  #$00800000,$C456FA.l
                move.w  #$A8,$C45A3E.l
                move.w  #$FC,$C45A40.l
                move.w  #$80,$C45A42.l
                move.b  #$80,$C457DD.l

                lea.l   $C4E7D0.l,a0
                moveq   #$15,d1
                clr.w   d0
.write_word_table:
                move.w  d0,(a0)+
                addq.w  #1,d0
                dbra    d1,.write_word_table

                moveq   #$30,d1
                lea.l   $C457FA.l,a0
                move.l  #$C4582A,d0
                sub.l   a0,d0
                subq.w  #1,d0
.fill_30:
                move.b  d1,(a0)+
                dbra    d0,.fill_30

                lea.l   $C4580A.l,a0
                moveq   #$20,d1
                moveq   #$1B,d0
.fill_20:
                move.b  d1,(a0)+
                dbra    d0,.fill_20
                bsr.w   CALL_C09266
