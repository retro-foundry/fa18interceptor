; Byte-exact $C2F622-$C2F625 return target for nonpositive D1 in C2F688.
                org $C2F622
reject_renderer_nonpositive_span:
 moveq #-1,d2
 rts
