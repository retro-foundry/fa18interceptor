; Byte-exact raw-$26 ECM-command slice $C1C1F2-$C1C223.
; Reached by the sealed run003 documented J event.

                org     $C1C1F2

ECM_CONTEXT                     equ $C457B0
ECM_REQUEST_FLAGS               equ $C4599B
ECM_REQUEST_BIT                 equ 3
ECM_COMMAND_MODE                equ $C45840
ECM_COMMAND_MODE_VALUE          equ 3
ECM_ENABLED                     equ $C458B5
COMMAND_SIDE_EFFECT             equ $C33186
SHARED_COMMAND_FALLBACK         equ $C1C23C

toggle_ecm_mode:
                lea.l   ECM_CONTEXT.l,a0
                bset.b  #ECM_REQUEST_BIT,ECM_REQUEST_FLAGS.l
                move.b  #ECM_COMMAND_MODE_VALUE,ECM_COMMAND_MODE.l
                jsr     COMMAND_SIDE_EFFECT.l
                lea.l   ECM_ENABLED.l,a0
                tst.b   (a0)
                beq.s   .enable
                move.b  #0,(a0)
                bra.s   SHARED_COMMAND_FALLBACK
.enable:
                move.b  #1,(a0)
                bra.s   SHARED_COMMAND_FALLBACK
