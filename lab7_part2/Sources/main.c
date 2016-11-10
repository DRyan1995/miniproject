/* ********************************************************************** *
 *  Project:     I2C_EEPROM                                               *
 *  Purpose:     EE128 Lab 7, SPI and I2C Communication                   *
 *                                                                        *
 *  Notes:       See Lab 7 manual for a detailed testbench schematic      *
 * ********************************************************************** */

#include <hidef.h>
#include <stdio.h>
#include <mc9s12dg256.h>
#include "i2c.h"


// #define DBG_ON                /* print out debugging information if enabled */


void clearscreen(void)
{
    putchar(0x0C);
}

void main(void) {

  const char *msgMemo = "I2C based EEPROM Reading and Writing";
  char rd;
  char memchar = 0x00;

  char eeprom_address = 0x0A;  /* address in EEPROM to write a byte to */
  short eeprom_data    = 0x4567;  /* data byte to be written to EEPROM */

  clearscreen();
  printf("EE128 LAB 7\r\n");
  printf("I2C based EEPROM Reading and Writing\r\n");

  printf("**************************************\r\n");
  printf("   9S12 will write a page to EEPROM\r\n   ");
  printf("**************************************\r\n");
  printf("EEPROM starting address: 0x%x\r\n", eeprom_address);
  printf("EEPROM page data:    0x%x\r\n", eeprom_data);

#ifdef DBG_ON
  printf("- running in DEBUG mode -\r\n");
#endif

  Init_I2C(0x1F,0xFE);         /* configure I2C module */

  // rd = EEbyteWrite(0xA0,eeprom_address,eeprom_data);
  rd = EEPageWrite(0xA0, eeprom_address, eeprom_data);
  eeAckPoll(0xA0);            /* make sure internal write operation is complete */

  printf("\r\n");
  printf("***************************************\r\n");
  printf("   9S12 will read a page from EEPROM\r\n");
  printf("***************************************\r\n");

  memchar = EErandomRead(0xA0,eeprom_address);

  printf("EEPROM @ 0x%x = 0x%x\r\n", eeprom_address, memchar);

  memchar = EErandomRead(0xA0,eeprom_address+0x04);

  printf("EEPROM @ 0x%x = 0x%x\r\n", eeprom_address+0x04, memchar);


  for(;;) {} /* wait forever */
}
