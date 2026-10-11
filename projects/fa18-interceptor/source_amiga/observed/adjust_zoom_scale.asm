; Byte-exact raw-$1B/$1A zoom-command slice $C1BAE2-$C1BB65.
; Reached by sealed run003 right- and left-bracket events.

                org     $C1BAE2

ZOOM_SCALE_MIN                  equ $20
ZOOM_SCALE_MAX                  equ $80
ZOOM_SCALE                      equ $C45A42
ZOOM_CONTEXT_GUARD              equ $C457B5
ZOOM_ENDPOINT_FLAGS             equ $C457DD
ZOOM_ENDPOINT_MAX_BIT           equ 7
DISPLAY_UPDATE_MODE             equ $C4583D
DISPLAY_UPDATE_MODE_VALUE       equ 3
DISPLAY_UPDATE_REQUEST          equ $C45891
DISPLAY_UPDATE_REQUEST_VALUE    equ $FF
DISPLAY_UPDATE_AUX              equ $C45858
ZOOM_CONTEXT_DECREASE_HANDLER   equ $C1B80A
ZOOM_CONTEXT_INCREASE_HANDLER   equ $C1B890
SHARED_COMMAND_FALLBACK         equ $C1C23C

decrease_zoom_scale:
                moveq   #ZOOM_SCALE_MIN,d3
                tst.b   d5
                beq.s   decrease_scale
                tst.b   ZOOM_CONTEXT_GUARD.l
                bne.w   ZOOM_CONTEXT_DECREASE_HANDLER
decrease_scale:
                cmp.w   ZOOM_SCALE.l,d3
                bge.s   submit_zoom_display_update
                asr.w   ZOOM_SCALE.l
                bra.s   submit_zoom_display_update

increase_zoom_scale:
                tst.b   d5
                beq.s   increase_scale
                tst.b   ZOOM_CONTEXT_GUARD.l
                bne.w   ZOOM_CONTEXT_INCREASE_HANDLER
increase_scale:
                cmpi.w  #ZOOM_SCALE_MAX,ZOOM_SCALE.l
                bge.s   submit_zoom_display_update
                asl.w   ZOOM_SCALE.l
submit_zoom_display_update:
                move.b  #DISPLAY_UPDATE_MODE_VALUE,DISPLAY_UPDATE_MODE.l
                move.b  #DISPLAY_UPDATE_REQUEST_VALUE,DISPLAY_UPDATE_REQUEST.l
                move.b  ZOOM_ENDPOINT_FLAGS.l,d4
                andi.b  #$7F,d4
                bne.s   update_zoom_endpoint_flag
                move.b  #DISPLAY_UPDATE_REQUEST_VALUE,DISPLAY_UPDATE_AUX.l
update_zoom_endpoint_flag:
                cmpi.w  #ZOOM_SCALE_MAX,ZOOM_SCALE.l
                beq.s   set_maximum_zoom_endpoint
                bclr.b  #ZOOM_ENDPOINT_MAX_BIT,ZOOM_ENDPOINT_FLAGS.l
                bra.w   SHARED_COMMAND_FALLBACK
set_maximum_zoom_endpoint:
                bset.b  #ZOOM_ENDPOINT_MAX_BIT,ZOOM_ENDPOINT_FLAGS.l
                bra.w   SHARED_COMMAND_FALLBACK
