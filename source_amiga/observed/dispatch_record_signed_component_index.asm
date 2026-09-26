; Byte-exact observed signed-component indexed dispatch $C2C45E-$C2C46F.
; It converts D0 to a longword table offset, loads the corresponding dispatch
; target from the PC-relative table, and jumps to it.

                org     $C2C45E

dispatch_record_signed_component_index:
                subq.w  #1,d0
                ext.w   d0
                add.w   d0,d0
                add.w   d0,d0
                lea     $C2BAF8(pc),a0
                dc.w    $2070,$0000         ; movea.l 0(a0,d0.w),a0
                jmp     (a0)
