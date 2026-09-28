#include "static_template_search.h"
static int16_t w(const uint8_t*p){return (int16_t)((uint16_t)p[0]<<8|p[1]);}
int fa18_search_static_template_row(const uint8_t*r,size_t z,int16_t n,uint16_t*out){if(!r||!out||z<2)return -1;uint16_t bytes=(uint16_t)((uint16_t)r[0]<<8|r[1]);if((bytes&1)||2u+bytes>z)return -1;int lo=0,hi=bytes/2-1;while(lo<=hi){int mid=lo+(hi-lo)/2;int16_t v=w(r+2+mid*2);if(v==n){*out=(uint16_t)mid;return 0;}if(n<v)hi=mid-1;else lo=mid+1;}return -1;}
