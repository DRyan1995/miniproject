#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <SoftwareSerial.h>

//for shift register
#define SERIAL_ROW_PIN 2
#define SERIAL_COL_PIN 3
#define OE_PIN 4
#define RCLK_PIN 5
#define SRCLK_PIN 6
#define SRCLR_PIN 7

//GPIO
#define CONFRIM_BTN_PIN 8

// testing shift regsiter
unsigned char rowData;
unsigned char colData;
unsigned char temp;

// btn flag;
short btnFlag;

// for software serial
SoftwareSerial mySerial(9, 10);//RX, TX

void port_iodr_init(){ // iodirection init
  pinMode(SERIAL_COL_PIN, OUTPUT);
  pinMode(SERIAL_ROW_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  pinMode(RCLK_PIN, OUTPUT);
  pinMode(SRCLR_PIN, OUTPUT);
  pinMode(SRCLK_PIN, OUTPUT);
  pinMode(CONFRIM_BTN_PIN, INPUT);
}

void port_ioval_init(){ //io value init
  digitalWrite(OE_PIN, LOW); // set the output_enable low to display on matrix
}

void task_init(){
  StartSendSecPulse(1);
  StartBtnSecPulse(1);
  vTaskStartScheduler();
}

void serial_setup(){
  Serial.begin(9600);
  mySerial.begin(9600);
}

void wifi_setup(){
  // delay(1000);
  // mySerial.write("AT+RST\r\n");
  // delay(1500);
  mySerial.write("AT+CIPMUX=1\r\n");
  delay(200);
  mySerial.write("AT+CIPSERVER=1\r\n");
  delay(200);
}


void setup() { // start_up
  port_iodr_init();
  port_ioval_init();
  serial_setup();
  wifi_setup();
  task_init();
}

void loop() {
  // nothing here
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

enum sendState {sendInit, SEND} send_state;
enum btnState {btnInit, POLLING} btn_state;

void send_Init(){
  send_state = sendInit;
}

void btn_Init(){
  btn_state = btnInit;
}

void send_Tick(){
  switch (send_state) { // actions
    case sendInit:
      colData = 0x09;
      rowData = ~0x05;
    break;
    case SEND:
      send_data(++rowData, ++colData);
    break;
    default:
    break;
  }

  switch (send_state) { // transitions
    case sendInit:
      send_state = SEND;
    break;
    case SEND:
      send_state = SEND;
    break;
    default:
      send_state = sendInit;
    break;
  }
}

void btn_Tick(){
  switch (btn_state) { // actions
    case btnInit:
      btnFlag = 0;
    break;
    case POLLING:
      btnFlag = digitalRead(CONFRIM_BTN_PIN);
    //   Serial.println(btnFlag);
    //test
    if(Serial.available()){
        mySerial.write(Serial.read());
    }
    if(mySerial.available()){
        Serial.write(mySerial.read());
    }
    break;
    default:
      btnFlag = 0;
    break;
  }

  switch (btn_state) { //transitions
    case btnInit:
      btn_state = POLLING;
    break;
    case POLLING:
      btn_state = POLLING;
    break;
    default:
      btn_state = btnInit;
    break;
  }
}

void SendSecTask(){
  send_Init();
  for(;;){
    send_Tick();
    delay(100);
  }
}

void BtnSecTask(){
  btn_Init();
  for(;;){
    btn_Tick();
  }
}

void StartSendSecPulse(unsigned portBASE_TYPE Priority){
  xTaskCreate(SendSecTask, (signed portCHAR *)"SendSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartBtnSecPulse(unsigned portBASE_TYPE Priority){
  xTaskCreate(BtnSecTask, (signed portCHAR *)"BtnSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
