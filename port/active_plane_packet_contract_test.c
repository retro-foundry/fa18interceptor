#include "active_plane_packet.h"
#include <assert.h>
int main(void){FA18ActivePlanePacketInput i={{0x100,0x200,0x300,0x400},3,1};FA18BlitOperation o[4];assert(!fa18_build_active_plane_packet(&i,o));assert(o[0].bltcpt==0x128&&o[1].bltcon0==0x3fa&&o[2].bltcon0==0x3fa&&o[3].bltsize==0xd4);return 0;}
