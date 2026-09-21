; Byte-exact $C23F4A-$C23FC1 mode-zero record-update tail.
; The bounded run004 selected-fire continuation reaches this tail from $C23A7E.

                org     $C23F4A

RECORD_FLAGS_OFFSET             equ $0000
RECORD_STATUS_OFFSET            equ $0001
RECORD_SPEED_OFFSET             equ $004C
RECORD_VECTOR_OFFSET            equ $0034
RECORD_HEADING_OFFSET           equ $006C
RECORD_PITCH_OFFSET             equ $006E

RECORD_UPDATE_SUPPRESS          equ $C457AE
RECORD_GLOBAL_UPDATE_STATE      equ $C45788
RECORD_TARGET_STATE             equ $C459C0

RECORD_ACTIVE_BIT               equ 6
RECORD_SECONDARY_BIT            equ 1
RECORD_TERTIARY_BIT             equ 3
RECORD_SPECIAL_FLAG             equ $0400
DEFAULT_HEADING                 equ $4200
HEADING_STEP                    equ $0240

update_mode_zero_record_motion:
                btst.b  #RECORD_ACTIVE_BIT,RECORD_STATUS_OFFSET(a1)
                dc.w    $67F6           ; beq.s $C23F48 (external shared tail)
                move.b  RECORD_GLOBAL_UPDATE_STATE.l,d1
                or.b    RECORD_UPDATE_SUPPRESS.l,d1
                bne.s   .skip_speed_countdown
                subq.w  #1,RECORD_SPEED_OFFSET(a1)
                dc.w    $6FD2           ; ble.s $C23F38 (external shared tail)
.skip_speed_countdown:
                dc.w    $3229,$0000      ; move.w 0(a1),d1
                andi.w  #RECORD_SPECIAL_FLAG,d1
                bne.s   .special_flag_path
                move.w  RECORD_SPEED_OFFSET(a1),d1
                cmpi.w  #$32,d1
                dc.w    $6C48           ; bge.s $C23FC2 (external next tail)
                cmpi.w  #$0C,d1
                bge.s   .minimum_speed_path
                dc.w    $0069,$0400,$0000 ; ori.w #$0400,0(a1)
.special_flag_path:
                nop
.minimum_speed_path:
                move.w  #$3000,d1
                bra.s   .adjust_heading
.default_heading_path:
                move.w  #DEFAULT_HEADING,d1
.adjust_heading:
                move.l  #0,RECORD_VECTOR_OFFSET(a1)
                move.w  RECORD_HEADING_OFFSET(a1),d0
                cmp.w   d1,d0
                bgt.s   .reduce_heading
                addi.w  #HEADING_STEP,d0
                cmp.w   d1,d0
                ble.s   .store_heading
                bra.s   .set_default_heading
.reduce_heading:
                subi.w  #HEADING_STEP,d0
                cmp.w   d1,d0
                bge.s   .store_heading
.set_default_heading:
                move.w  d1,d0
.store_heading:
                move.w  d0,RECORD_HEADING_OFFSET(a1)
                move.w  d0,RECORD_PITCH_OFFSET(a1)
                moveq   #1,d0
                rts
