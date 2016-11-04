#include <hidef.h> /* common defines and macros */
#include <mc9s12dg256.h>
#include <stdio.h>
#include "derivative.h"

char getcSCI1(void){
  while(!(SCI1SR1 & 0x20));
  return SCI1DRL;
}

void main(void) {
  SCI1BD = 156;
  SCI1CR1 = 0;
  SCI1CR2 = 0xc;
  while(1){
    putchar(getcSCI1());
  }
}
