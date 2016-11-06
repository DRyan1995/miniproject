#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).

#define SERIAL_ROW_PIN 2
#define SERIAL_COL_PIN 3
#define OE_PIN 4
#define RCLK_PIN 5
#define SRCLK_PIN 6
#define SRCLR_PIN 7

void port_iodr_init(){ // iodirection init
  pinMode(SERIAL_COL_PIN, OUTPUT);
  pinMode(SERIAL_ROW_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  pinMode(RCLK_PIN, OUTPUT);
  pinMode(SRCLR_PIN, OUTPUT);
  pinMode(SRCLK_PIN, OUTPUT);
}
unsigned char rowData;
unsigned char colData;
unsigned char temp;
void setup() {
  // put your setup code here, to run once:
  port_iodr_init();
  digitalWrite(OE_PIN, LOW);
  temp = 0;
}

void loop() {
  // nothing here
  colData = 0x5;
  rowData = ~0x5;
  send_data(rowData, colData);
  delay(200);
}

void send_data(unsigned char rowData, unsigned char colData){
  unsigned char mask;
  digitalWrite(SRCLR_PIN, HIGH); // set SRCLR high
  digitalWrite(RCLK_PIN, LOW); // set RCLK low

  for(int i = 8;i >= 0;i--){
    mask = (1 << i);
    // PORTB &= ~SRCLK_MASK; // set SRCLK low
    digitalWrite(SRCLK_PIN, LOW);
    digitalWrite(SERIAL_ROW_PIN, (rowData & mask)?HIGH:LOW);
    digitalWrite(SERIAL_COL_PIN, (colData & mask)?HIGH:LOW);
    // PORTB |= SRCLK_MASK; // set SRCLK high
    digitalWrite(SRCLK_PIN, HIGH); // set SRCLK high
  }
    // PORTB |= RCLK_MASK; //set RCLK high
    digitalWrite(RCLK_PIN, HIGH);
    for(int pin = 2; pin <= 7; pin++){
      digitalWrite(pin, LOW);
    }
    // PORTB = 0x00; //reset
}
