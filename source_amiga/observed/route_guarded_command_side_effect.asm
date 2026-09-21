; Byte-exact command-side-effect routes $C3316A-$C331CC.
; The $C33186 entry has a return-bounded trace. The external $C17EF2 callee
; and the meanings of the selector values remain unassigned.
                org     $C3316A

COMMAND_BLOCK_FLAG             equ     $C45785
COMMAND_ROUTE_FLAG             equ     $C457D7
COMMAND_SECONDARY_GUARD        equ     $C4588A
EXTERNAL_COMMAND_SIDE_EFFECT   equ     $C17EF2

route_command_mode_one:
                moveq   #1,d0
                bra.b   command_side_effect_check_secondary_guard

route_signed_command_mode:
                tst.b   COMMAND_BLOCK_FLAG.l
                bne.b   command_side_effect_done
                movem.l d0-d7/a0-a4,-(sp)
                ext.l   d0
                moveq   #4,d1
                bra.b   command_side_effect_prepare_external_call

route_command_mode_two:
                moveq   #2,d0
                moveq   #2,d1
                bra.b   command_side_effect_check_secondary_guard

route_guarded_command_side_effect:
                tst.b   COMMAND_BLOCK_FLAG.l
                bne.b   command_side_effect_done
                tst.b   COMMAND_ROUTE_FLAG.l
                bne.b   route_command_mode_two
                moveq   #2,d0
                moveq   #4,d1
command_side_effect_check_secondary_guard:
                tst.b   COMMAND_SECONDARY_GUARD.l
                bgt.b   command_side_effect_done
                movem.l d0-d7/a0-a4,-(sp)
command_side_effect_prepare_external_call:
                moveq   #1,d6
                move.l  d0,d7
                movea.l d7,a0
                move.l  #$12c,d0
                moveq   #1,d2
                move.l  d0,d3
                move.l  d1,d4
                move.l  d2,d5
                movem.l d0-d7/a0,-(sp)
                jsr     EXTERNAL_COMMAND_SIDE_EFFECT.l
                dc.w    $defc,$0024 ; adda.w #$24,sp; retain original non-relaxed opcode
                movem.l (sp)+,d0-d7/a0-a4
command_side_effect_done:
                rts
