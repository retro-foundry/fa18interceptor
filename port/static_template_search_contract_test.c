#include "static_template_search.h"
#include <assert.h>
int main(void){uint8_t r[]={0,8,0,1,0,4,0,7,0,9};uint16_t i;assert(!fa18_search_static_template_row(r,sizeof r,7,&i)&&i==2);assert(fa18_search_static_template_row(r,sizeof r,6,&i)==-1);assert(fa18_search_static_template_row(r,3,1,&i)==-1);}
