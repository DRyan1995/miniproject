#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or FlbtnFlags).
#include <SoftwareSerial.h>
#include <Stepper.h>

#define LOW 0
#define HIGH 1

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

// btn flbtnFlag;
int btnFlag;

// for software serial
SoftwareSerial mySerial(9, 10);//RX, TX

// for stepper motor
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 11, 12, 13, A5);
short stepperBusy;

// for Joystick
#define LR_INPUT A0
#define UD_INPUT A1
int LRValue, UDValue;
short leftPressed, rightPressed, upPressed, downPressed;

// for wifi module
short wifiConfigured;
short wifiSetting;

// for preset strings
String sendInstruction = "AT+CIPSEND=0,14\r\n";
String content = "btn is pressed!";

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
  StartUartSecPulse(1);
  StartBtnSecPulse(1);
  StartStepperSecPulse(1);
  StartA2dSecPulse(1);
  // StartJudgeSecPulse(1);
  vTaskStartScheduler();
}

void serial_setup(){
  Serial.begin(1200);
  mySerial.begin(9600);
}

void wifi_setup(){
  // delay(1000);
  // mySerial.write("AT+RST\r\n");
  // delay(1500);
  mySerial.write("AT+CWMODE=1\r\n");
  delay(100);
  Serial.write(mySerial.read());
  mySerial.write("AT+CWJAP=\"Yolanda\",\"ZiYuBB1995\"\r\n");
  delay(5000);
  Serial.write(mySerial.read());
  mySerial.write("AT+CIPMUX=1\r\n");
  delay(100);
  Serial.write(mySerial.read());
  delay(100);
  mySerial.write("AT+CIPSERVER=1,88888\r\n");
  wifiConfigured = 1;
  wifiSetting = 0;
  delay(200);
}

void setup() { // start_up
  port_iodr_init();
  port_ioval_init();
  serial_setup();
  task_init();
}

void loop() {
  // nothing here
}

//this is for shift register
void send_data(unsigned char rowData, unsigned char colData){
  unsigned char mask;
  digitalWrite(SRCLR_PIN, HIGH); // set SRCLR high
  digitalWrite(RCLK_PIN, LOW); // set RCLK low
  for(int i = 8;i >= 0;i--){
    mask = (1 << i);
    digitalWrite(SRCLK_PIN, LOW); // set SRCLK low
    digitalWrite(SERIAL_ROW_PIN, (rowData & mask)?HIGH:LOW);
    digitalWrite(SERIAL_COL_PIN, (colData & mask)?HIGH:LOW);
    digitalWrite(SRCLK_PIN, HIGH); // set SRCLK high
  }
    digitalWrite(RCLK_PIN, HIGH);
    for(int pin = 2; pin <= 7; pin++){
      digitalWrite(pin, LOW);
    }
}

void sendMsg(int devId){
  mySerial.print(sendInstruction);
  delay(20);
  mySerial.print(content);
  return;
}

enum sendState {sendInit, SEND} send_state;
enum btnState {btnInit, POLLING} btn_state;
enum stepperState {stepperInit, CLOCKWISE, COUNTERCLOCKWISE} stepper_state;
enum uartState {uartInit, uartListening} uart_state;
enum a2dState {a2dInit, a2dListening} a2d_state;
enum judgeState {judgeInit, judging} judge_state;

void send_Init(){
  send_state = sendInit;
}

void btn_Init(){
  btn_state = btnInit;
}

void stepper_Init(){
  stepper_state = stepperInit;
}

void uart_Init(){
  uart_state = uartInit;
}

void a2d_Init(){
  a2d_state = a2dInit;
}

void judge_Init(){
  judge_state = judgeInit;
}

void send_Tick(){
  switch (send_state) { // actions
    case sendInit:
      colData = 0x09;
      rowData = ~0x05;
    break;
    case SEND:
      send_data(rowData, colData);
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
      // for testing
      if(!btnFlag){
        ++colData;
        ++rowData;
        sendMsg(0);
        // mySerial.print("fuckyou");
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

void stepper_Tick(){
  switch (stepper_state) { //actions
    case stepperInit:
      myStepper.setSpeed(240);
      stepperBusy = 0;
    break;
    case CLOCKWISE:
      if(!stepperBusy){
        stepperBusy = 1;
        myStepper.step(stepsPerRevolution);
        stepperBusy = 0;
      }
    break;
    case COUNTERCLOCKWISE:
      if(!stepperBusy){
        stepperBusy = 1;
        myStepper.step(-stepsPerRevolution);
        stepperBusy = 0;
      }
    break;
    default:
      stepperBusy = 0;
    break;
  }

  switch (stepper_state) { // transitions
    case stepperInit:
      stepper_state = CLOCKWISE;
    break;
    case CLOCKWISE:
      stepper_state = CLOCKWISE;
    break;
    case COUNTERCLOCKWISE:
    break;
    default:
      stepper_state = stepperInit;
    break;
  }
}

void uart_Tick(){
  switch (uart_state) { // actions
    case uartInit:
      wifiSetting = 0;
      wifiConfigured = 0;
    break;
    case  uartListening:
      if (wifiSetting) {
        if(mySerial.available()){
          Serial.write(mySerial.read());
        }
        break;
      }
      if (!wifiConfigured) {
        wifiSetting = 1;
        wifi_setup();
        break;
      }
      //*** for the monitor debugg **
      if(Serial.available()){
          mySerial.write(Serial.read());
      }
      // ***************************
      // for testing
      // if(upPressed){
      //   mySerial.write("UP!\r\n");
      // }
      // if(leftPressed){
      //   mySerial.write("LEFT!\r\n");
      // }

      //***************
      if(mySerial.available()){
        Serial.write(mySerial.read());
      }
    break;
    default:
    break;
  }

  switch (uart_state) { // transitions
    case uartInit:
      uart_state = uartListening;
    break;
    case  uartListening:
      uart_state = uartListening;
    break;
    default:
      uart_state = uartInit;
    break;
  }
}

void a2d_Tick(){
  switch (a2d_state) { // actions
    case a2dInit:
      leftPressed = 0;
      rightPressed = 0;
      upPressed = 0;
      downPressed = 0;
    break;
    case a2dListening:
      LRValue = analogRead(LR_INPUT);
      LRValue = map(LRValue, 0, 1023, 0, 255);
      delay(20);
      UDValue = analogRead(UD_INPUT);
      UDValue = map(UDValue, 0, 1023, 0, 255);
      delay(20);
      // test
      // Serial.print("LR: ");
      // Serial.print(LRValue);
      // Serial.print(" UD: ");
      // Serial.println(UDValue);
      // ************
      leftPressed = (LRValue < 115)?1:0;
      rightPressed = (LRValue > 140)?1:0;
      downPressed = (UDValue < 115)?1:0;
      upPressed = (UDValue > 140)?1:0;
    break;
    default:
    break;
  }

  switch (a2d_state) { // transitions
    case a2dInit:
      a2d_state = a2dListening;
    break;
    case a2dListening:
      a2d_state = a2dListening;
    break;
    default:
      a2d_state = a2dInit;
    break;
  }
}

void judge_Tick(){
  switch (judge_state) { //actions
    case judgeInit:
    break;
    case judging:
      if(!btnFlag){
        String content = "btn pressed!";
        // sendMsg(0, content);
      }
    break;
    default:
    break;
  }

  switch (judge_state) { // transitions
    case judgeInit:
      judge_state = judging;
    break;
    case judging:
      judge_state = judging;
    break;
    default:
      judge_state = judgeInit;
    break;
  }
}

void SendSecTask(){
  send_Init();
  for(;;){
    send_Tick();
    delay(20);
  }
}

void BtnSecTask(){
  btn_Init();
  for(;;){
    btn_Tick();
    delay(50);
  }
}

void StepperSecTask(){
  stepper_Init();
  for(;;){
    stepper_Tick();
    delay(100);
  }
}

void UartSecTask(){
  uart_Init();
  for(;;){
    uart_Tick();
    delay(1);
  }
}

void A2dSecTask(){
  a2d_Init();
  for(;;){
    a2d_Tick();
    delay(100);
  }
}

void JudgeSecTask(){
  judge_Init();
  for(;;){
    judge_Tick();
    delay(300);
  }
}

void StartSendSecPulse(unsigned portBASE_TYPE Priority){
  xTaskCreate(SendSecTask, (signed portCHAR *)"SendSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartBtnSecPulse(unsigned portBASE_TYPE Priority){
  xTaskCreate(BtnSecTask, (signed portCHAR *)"BtnSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartStepperSecPulse(unsigned portBASE_TYPE Priority){
  xTaskCreate(StepperSecTask, (signed portCHAR *)"StepperSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartUartSecPulse(unsigned portBASE_TYPE Priority){
  xTaskCreate(UartSecTask, (signed portCHAR *)"UartSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartA2dSecPulse(unsigned portBASE_TYPE Priority){
  xTaskCreate(A2dSecTask, (signed portCHAR *)"A2dSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartJudgeSecPulse(unsigned portBASE_TYPE Priority){
  xTaskCreate(JudgeSecTask, (signed portCHAR *)"JudgeSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
