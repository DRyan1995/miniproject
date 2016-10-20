#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12dg256.h>

unsigned long counter = 0;
unsigned char ROT_HLD;
unsigned char ROT_DIR;
unsigned long i;

//中断位置定义 参考lab manual
typedef void (*near tIsrFunc)(void); /* Type of interrupt vector entry */
__interrupt void IRQ_ISR(void); /* Declaration of ISR for IRQ */
const tIsrFunc _vect @0x3E74 = IRQ_ISR; /* 0x3E72: entry for IRQ */

__interrupt void IRQ_ISR(void){
  unsigned char decoder[10] = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B };//可以参考第三章课件 数码管显示一种方式
  PORTA ^=  0x80; //数码管dp端口的tick 参见task1要求 利用^运算 

  // get the switch input
  ROT_HLD = 0x20 & PORTK; //我们连的是k4和k5吧 所以是0x20和0x10
  ROT_DIR = 0x10 & PORTK;

  if (ROT_HLD) {
    return;
  }
  if (ROT_DIR) {
    if (counter <= 0) { //边界处理
      counter = 99;
    }else{
      counter --;
    }
  }else{
    if (counter >= 99) {//边界处理
      counter = 0;
    }else{
      counter ++;
    }
  }

    //用位运算来给两个数码管赋值 &是and运算 |是or运算
    PORTA &= 0x80;
    PORTB &= 0x80;
    PORTA |= decoder[(counter%10)];
    PORTB |= decoder[(counter/10)];
}

void main(void) {
  //初始化
  DDRA = 0xff;
  DDRB = 0xff;
  DDRK = 0x00;
  PORTA = 0;
  PORTB = 0;
  //参考lab manual 给中断register 赋值
  PORTE = 0xff;
  INTCR = 0x40;

	EnableInterrupts;
  while (1) {
  }
}
