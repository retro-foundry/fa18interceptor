; Byte-exact raw-$25 HUD-command slice $C1B264-$C1B27D.
; Reached by the sealed run003 documented H event.

                org     $C1B264

HUD_MODE                       equ $C457A1
HUD_MODE_MAX                   equ 1
SHARED_COMMAND_FALLBACK         equ $C1C23C

toggle_hud_mode:
                move.b  HUD_MODE.l,d4
                addq.b  #1,d4
                cmpi.b  #HUD_MODE_MAX,d4
                ble.s   .store_mode
                clr.b   d4
.store_mode:
                move.b  d4,HUD_MODE.l
                bra.w   SHARED_COMMAND_FALLBACK
