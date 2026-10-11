; Byte-exact renderer overlay D3 negative-mask selection $C3008A-$C30091.

                org     $C3008A

select_renderer_overlay_d3_negative_mask:
                move.w  #$FFF0,d3
                tst.w   d7
                beq.b   $C30098
