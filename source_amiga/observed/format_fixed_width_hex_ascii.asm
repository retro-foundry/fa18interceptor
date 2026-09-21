; Byte-exact fixed-width hexadecimal formatter $C0F56A-$C0F5F7.
; Stack arguments: output pointer at 8(a6), source long at 12(a6),
; digit count byte at 19(a6).  The routine writes backwards, then blanks
; leading ASCII zeroes while retaining the final digit.

                org     $C0F56A

ASCII_ZERO                      equ $30
ASCII_NINE                      equ $39
ASCII_HEX_ALPHA_OFFSET          equ 7
ASCII_SPACE                     equ $20
HEX_NIBBLE_MASK                 equ $0F

format_fixed_width_hex_ascii:
                link.w  a6,#-2
                move.b  19(a6),d0
                ext.w   d0
                ext.l   d0
                add.l   d0,8(a6)
                clr.b   -1(a6)

convert_next_nibble:
                move.b  -1(a6),d0
                cmp.b   19(a6),d0
                bge.b   finish_nibble_conversion
                move.l  12(a6),d0
                andi.l  #HEX_NIBBLE_MASK,d0
                addi.l  #ASCII_ZERO,d0
                move.b  d0,-2(a6)
                cmpi.b  #ASCII_NINE,d0
                ble.b   store_hex_digit
                addq.b  #ASCII_HEX_ALPHA_OFFSET,-2(a6)

store_hex_digit:
                movea.l 8(a6),a0
                move.b  -2(a6),(a0)
                subq.l  #1,8(a6)
                addq.b  #1,-1(a6)
                move.l  12(a6),d0
                lsr.l   #4,d0
                move.l  d0,12(a6)
                bra.b   convert_next_nibble

finish_nibble_conversion:
                subq.b  #1,19(a6)
                addq.l  #1,8(a6)
                clr.b   -1(a6)

blank_next_leading_zero:
                move.b  -1(a6),d0
                cmp.b   19(a6),d0
                bge.b   finish_fixed_width_format
                movea.l 8(a6),a0
                move.b  (a0),d0
                cmpi.b  #ASCII_ZERO,d0
                bne.b   finish_fixed_width_format
                move.b  #ASCII_SPACE,(a0)
                addq.l  #1,8(a6)
                nop
                addq.b  #1,-1(a6)
                bra.b   blank_next_leading_zero

finish_fixed_width_format:
                unlk    a6
                rts
