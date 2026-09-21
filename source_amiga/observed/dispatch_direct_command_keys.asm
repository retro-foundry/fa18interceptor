; Byte-exact direct raw-key dispatch table $C1AE28-$C1AEDF.
; Entered only after the preceding command-context gates succeed.

                org     $C1AE28

RAW_KEY_SPACE                   equ $40
RAW_KEY_M                       equ $4C
RAW_KEY_N                       equ $4D
RAW_KEY_T                       equ $4E
RAW_KEY_R                       equ $4F
RAW_KEY_BACKSPACE               equ $38
RAW_KEY_TAB                     equ $39
RAW_KEY_MINUS                   equ $0C
RAW_KEY_PLUS                    equ $0B
RAW_KEY_EQUALS                  equ $0D
RAW_KEY_A                       equ $41
RAW_KEY_EJECT                   equ $12
RAW_KEY_RADAR_RANGE             equ $13
RAW_KEY_GEAR                    equ $24
RAW_KEY_HOOK                    equ $20
RAW_KEY_FLARE                   equ $23
RAW_KEY_CHAFF                   equ $33
RAW_KEY_ECM                     equ $26
RAW_KEY_HUD                     equ $25
RAW_KEY_TARGET                  equ $21
RAW_KEY_TARGET_ALTERNATE        equ $14
RAW_KEY_MODE_ALTERNATE          equ $15
RAW_KEY_MAP                     equ $37

DISPATCH_SPACE_AND_MODE         equ $C1B21C
DISPATCH_M_KEY                  equ $C1B4FC
DISPATCH_N_KEY                  equ $C1B4F4
DISPATCH_T_KEY                  equ $C1B548
DISPATCH_R_KEY                  equ $C1B540
DISPATCH_BACKSPACE              equ $C1B58E
DISPATCH_TAB                    equ $C1B594
DISPATCH_MINUS                  equ $C1B5B8
DISPATCH_EQUALS                 equ $C1B5BC
FINISH_THROTTLE_MODE            equ $C1B5DC
DISPATCH_EJECT                  equ $C1B126
DISPATCH_RADAR_RANGE            equ $C1B1C8
DISPATCH_GEAR                   equ $C1BC12
DISPATCH_HOOK                   equ $C1B616
DISPATCH_FLARE                  equ $C1C0E0
DISPATCH_CHAFF                  equ $C1C172
DISPATCH_ECM                    equ $C1C1F2
DISPATCH_HUD                    equ $C1B264
DISPATCH_TARGET                 equ $C1C052
REQUEST_NEXT_TARGET             equ $C1B1A4
DISPATCH_MODE_ALTERNATE         equ $C1B236
DISPATCH_MAP                    equ $C1BF8C

dispatch_direct_command_keys:
                cmpi.b  #RAW_KEY_SPACE,d0
                beq.w   DISPATCH_SPACE_AND_MODE
                cmpi.b  #RAW_KEY_M,d0
                beq.w   DISPATCH_M_KEY
                cmpi.b  #RAW_KEY_N,d0
                beq.w   DISPATCH_N_KEY
                cmpi.b  #RAW_KEY_R,d0
                beq.w   DISPATCH_R_KEY
                cmpi.b  #RAW_KEY_T,d0
                beq.w   DISPATCH_T_KEY
                cmpi.b  #RAW_KEY_BACKSPACE,d0
                beq.w   DISPATCH_BACKSPACE
                cmpi.b  #RAW_KEY_TAB,d0
                beq.w   DISPATCH_TAB
                cmpi.b  #RAW_KEY_MINUS,d0
                beq.w   DISPATCH_MINUS
                cmpi.b  #RAW_KEY_PLUS,d0
                beq.w   DISPATCH_EQUALS
                cmpi.b  #RAW_KEY_EQUALS,d0
                beq.w   FINISH_THROTTLE_MODE
                cmpi.b  #RAW_KEY_A,d0
                beq.w   FINISH_THROTTLE_MODE
                cmpi.b  #RAW_KEY_EJECT,d0
                beq.w   DISPATCH_EJECT
                cmpi.b  #RAW_KEY_RADAR_RANGE,d0
                beq.w   DISPATCH_RADAR_RANGE
                cmpi.b  #RAW_KEY_GEAR,d0
                beq.w   DISPATCH_GEAR
                cmpi.b  #RAW_KEY_HOOK,d0
                beq.w   DISPATCH_HOOK
                cmpi.b  #RAW_KEY_FLARE,d0
                beq.w   DISPATCH_FLARE
                cmpi.b  #RAW_KEY_CHAFF,d0
                beq.w   DISPATCH_CHAFF
                cmpi.b  #RAW_KEY_ECM,d0
                beq.w   DISPATCH_ECM
                cmpi.b  #RAW_KEY_HUD,d0
                beq.w   DISPATCH_HUD
                cmpi.b  #RAW_KEY_TARGET,d0
                beq.w   DISPATCH_TARGET
                cmpi.b  #RAW_KEY_TARGET_ALTERNATE,d0
                beq.w   REQUEST_NEXT_TARGET
                cmpi.b  #RAW_KEY_MODE_ALTERNATE,d0
                beq.w   DISPATCH_MODE_ALTERNATE
                cmpi.b  #RAW_KEY_MAP,d0
                beq.w   DISPATCH_MAP
