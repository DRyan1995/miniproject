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

// // testing shift regsiter
// unsigned char rowData;
// unsigned char colData;
// unsigned char temp;

// btn flbtnFlag;
int btnFlag;
int unLocked;

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
short upleftPressed, uprightPressed, downleftPressed, downrightPressed;

// for wifi module
short wifiConfigured;
short wifiSetting;

// for preset strings
String sendInstruction = "AT+CIPSEND=0,14\r\n";
String content = "Door Unlocked!";

// for pattern unlock
unsigned char displayMatrix[9][6];
unsigned char destMatrix[9][6];
unsigned char startX, startY;
unsigned char curX, curY;

// for instruction resolve
#define INSTRUCTION_LENGTH 17
char instructions[INSTRUCTION_LENGTH];


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
  StartStepperSecPulse(1);
  StartA2dSecPulse(1);
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
  mySerial.write("AT+CIPMUX=0\r\n");
  delay(100);
  Serial.write(mySerial.read());
  delay(100);
  // mySerial.write("AT+CIPSERVER=1,88888\r\n");
  mySerial.write("AT+CIPSTART=\"TCP\",\"192.168.0.101\",8080\r\n");
  delay(1000);
  wifiConfigured = 1;
  wifiSetting = 0;
  delay(200);
}

void love_init(){
  displayMatrix[5][5] = 1;
  displayMatrix[4][4] = 1;
  displayMatrix[3][3] = 1;
  displayMatrix[2][2] = 1;
  displayMatrix[3][1] = 1;
  displayMatrix[4][1] = 1;
  displayMatrix[5][2] = 1;
  displayMatrix[6][1] = 1;
  displayMatrix[7][1] = 1;
  displayMatrix[8][2] = 1;
  displayMatrix[7][3] = 1;
  displayMatrix[6][4] = 1;
}

void des_init(){
  for(int i = 0; i <= 8; i++)
    for(int j = 0; j <= 5; j++)
      destMatrix[i][j] = 0;
  startX = 5;
  startY = 3;
  // love pattern
  destMatrix[5][5] = 1;
  destMatrix[4][4] = 1;
  destMatrix[3][3] = 1;
  destMatrix[2][2] = 1;
  destMatrix[3][1] = 1;
  destMatrix[4][1] = 1;
  destMatrix[5][2] = 1;
  destMatrix[6][1] = 1;
  destMatrix[7][1] = 1;
  destMatrix[8][2] = 1;
  destMatrix[7][3] = 1;
  destMatrix[6][4] = 1;
  destMatrix[5][3] = 1;
  //******
  // destMatrix[5][3] = 1;
  // destMatrix[5][4] = 1;
}

void des_clear(){
  for(int i = 0; i <= 8; i++)
    for(int j = 0; j <= 5; j++)
      destMatrix[i][j] = 0;
}

int res_compare(){
  for(int i = 1; i <= 8; i++)
    for(int j = 1; j <= 5; j++)
      if(destMatrix[i][j] != displayMatrix[i][j])
        return 0;
  return 1;
}

void instructionPush(char x){
  int i;
  for(i = 0; i < INSTRUCTION_LENGTH - 1; i++)
    instructions[i] = instructions[i + 1];
  instructions[INSTRUCTION_LENGTH - 1] = x;
}

int Char2Int(char x){
  int ret;
  if (x <= '9') {
    ret = x - '0';
  }else{
    ret = x - 'a' + 10;
  }
  return ret;
}

void instructionHandler(){
  int i, j, high, low;
  if (instructions[0] == '*' && instructions[INSTRUCTION_LENGTH - 1] == '*') {
      if (instructions[1] == 'U' && instructions[2] == 'n' && instructions[3] == 'l') {
        Serial.write("Door Unlocked");
        //todo: actions here

      }else if(instructions[1] == 'P' && instructions[2] == 'W' && instructions[3] == 'D'){
        des_clear();
        startX = instructions[4] - '0' + 1;
        startY = instructions[5] - '0' + 1;
        for(i = 1; i <= 5; i++){
          high = Char2Int(instructions[2 * i + 4]);
          low = Char2Int(instructions[2 * i + 5]);
            destMatrix[1][i] = ((low & 0x1) == 0x1)?1:0;
            destMatrix[5][i] = ((high & 0x1) == 0x1)?1:0;
            destMatrix[2][i] = ((low & 0x2) == 0x2)?1:0;
            destMatrix[6][i] = ((high & 0x2) == 0x2)?1:0;
            destMatrix[3][i] = ((low & 0x4) == 0x4)?1:0;
            destMatrix[7][i] = ((high & 0x4) == 0x4)?1:0;
            destMatrix[4][i] = ((low & 0x8) == 0x8)?1:0;
            destMatrix[8][i] = ((high & 0x8) == 0x8)?1:0;
        }
        display_init();
      }

  }
}

void display_init(){
  for(int i = 0; i <= 8; i++){
    for(int j = 0; j <= 5; j++){
      displayMatrix[i][j] = 0;
    }
  }
  // love_init();
  curX = startX;
  curY = startY;
  displayMatrix[curX][curY] = 1;
}

void setup() { // start_up
  des_init();
  display_init();
  port_iodr_init();
  port_ioval_init();
  serial_setup();
  task_init();
}

void loop() {
  // nothing here
}

void Display(){
  if(unLocked){
    send_data(0xff, 0x00);
    return;
  }
  unsigned char rowData, colData;
  for(int i = 5; i >= 1; i--){
    rowData = ~(1 << (i - 1));
    colData = 0;
    for(int j = 1; j <= 8; j++){
      colData += displayMatrix[j][i] * (1 << (j - 1));
      // colData += destMatrix[j][i] * (1 << (j - 1));
    }
    send_data(rowData, colData);
    send_data(rowData, colData);
  }
  for(int i = 1; i <= 5; i++){
    rowData = ~(1 << (i - 1));
    colData = 0;
    for(int j = 1; j <= 8; j++){
      colData += displayMatrix[j][i] * (1 << (j - 1));
      // colData += destMatrix[j][i] * (1 << (j - 1));
    }
    send_data(rowData, colData);
    send_data(rowData, colData);
    // send_data(rowData, colData);
  }

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

void sendMsg(){
  mySerial.print(sendInstruction);
  delay(20);
  mySerial.print(content);
  return;
}

enum sendState {sendInit, WAIT, WAIT_RELEASE, LEFT, RIGHT, UP, DOWN, UPLEFT, UPRIGHT, DOWNLEFT, DOWNRIGHT} send_state;
enum stepperState {stepperInit, READY, CLOCKWISE, COUNTERCLOCKWISE} stepper_state;
enum uartState {uartInit, uartListening} uart_state;
enum a2dState {a2dInit, a2dListening} a2d_state;

void send_Init(){
  send_state = sendInit;
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

void MoveLeft(){
  if (curX == 8) {
    return;
  }
  if (displayMatrix[curX + 1][curY]) {
    return;
  }
  displayMatrix[++curX][curY] = 1;
}

void MoveRight(){
  if (curX == 1) {
    return;
  }
  if (displayMatrix[curX - 1][curY]) {
    return;
  }
  displayMatrix[--curX][curY] = 1;
}

void MoveUp(){
  if (curY == 1) {
    return;
  }
  if (displayMatrix[curX][curY - 1]) {
    return;
  }
  displayMatrix[curX][--curY] = 1;
}

void MoveDown(){
  if (curY == 5) {
    return;
  }
  if (displayMatrix[curX][curY + 1]) {
    return;
  }
  displayMatrix[curX][++curY] = 1;
}

void MoveUpLeft(){
  if(curX == 8 || curY == 1){
    return;
  }
  if(displayMatrix[curX + 1][curY - 1]){
    return;
  }
  displayMatrix[++curX][--curY] = 1;
}

void MoveUpRight(){
  if(curX == 1 || curY == 1){
    return;
  }
  if(displayMatrix[curX - 1][curY - 1]){
    return;
  }
  displayMatrix[--curX][--curY] = 1;
}

void MoveDownLeft(){
  if(curX == 8 || curY == 5){
    return;
  }
  if(displayMatrix[curX + 1][curY + 1]){
    return;
  }
  displayMatrix[++curX][++curY] = 1;
}

void MoveDownRight(){
  if(curX == 1 || curY == 5){
    return;
  }
  if(displayMatrix[curX - 1][curY + 1]){
    return;
  }
  displayMatrix[--curX][++curY] = 1;
}

void send_Tick(){
  switch (send_state) { // actions
    case sendInit:
    break;
    case WAIT:
      // send_data(rowData, colData);
      Display();
    break;
    case WAIT_RELEASE:
      Display();
    break;
    case LEFT:
      MoveLeft();
      Display();
    break;
    case RIGHT:
      MoveRight();
      Display();
    break;
    case UP:
      MoveUp();
      Display();
    break;
    case DOWN:
      MoveDown();
      Display();
    break;
    case UPLEFT:
      MoveUpLeft();
      Display();
    break;
    case UPRIGHT:
      MoveUpRight();
      Display();
    break;
    case DOWNLEFT:
      MoveDownLeft();
      Display();
    break;
    case DOWNRIGHT:
      MoveDownRight();
      Display();
    break;
    default:
    break;
  }

  switch (send_state) { // transitions
    case sendInit:
      send_state = WAIT;
    break;
    case WAIT:
      if(upleftPressed){
        send_state = UPLEFT;
      }else if(uprightPressed){
        send_state = UPRIGHT;
      }else if(downleftPressed){
        send_state = DOWNLEFT;
      }else if(downrightPressed){
        send_state = DOWNRIGHT;
      }else if (leftPressed) {
        send_state = LEFT;
      }else if(rightPressed){
        send_state = RIGHT;
      }else if(upPressed){
        send_state = UP;
      }else if(downPressed){
        send_state = DOWN;
      }else{
        send_state = WAIT;
      }
    break;
    case WAIT_RELEASE:
      if(upPressed | downPressed | leftPressed | rightPressed){
        send_state = WAIT_RELEASE;
      }else{
        send_state = WAIT;
      }
    break;
    case LEFT:
      send_state = WAIT_RELEASE;
    break;
    case RIGHT:
      send_state = WAIT_RELEASE;
    break;
    case UP:
      send_state = WAIT_RELEASE;
    break;
    case DOWN:
      send_state = WAIT_RELEASE;
    break;
    case UPLEFT:
      send_state = WAIT_RELEASE;
    break;
    case UPRIGHT:
      send_state = WAIT_RELEASE;
    break;
    case DOWNLEFT:
      send_state = WAIT_RELEASE;
    break;
    case DOWNRIGHT:
      send_state = WAIT_RELEASE;
    break;
    default:
      send_state = sendInit;
    break;
  }
}

void stepper_Tick(){
  switch (stepper_state) { //actions
    case stepperInit:
      myStepper.setSpeed(120);
      stepperBusy = 0;
    break;
    case READY:
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
      stepper_state = READY;
    break;
    case READY:
      if(unLocked){
        stepper_state = CLOCKWISE;
        // unLocked = 0;
      }else{
        stepper_state = READY;
      }
    break;
    case CLOCKWISE:
      if (stepperBusy) {
        stepper_state = CLOCKWISE;
      }else{
        delay(10000);// delay sometime and relock
        stepper_state = COUNTERCLOCKWISE;
      }
    break;
    case COUNTERCLOCKWISE:
      if (stepperBusy) {
        stepper_state = COUNTERCLOCKWISE;
      }else{
        stepper_state = READY;
        unLocked = 0;
      }
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

      // if(upPressed){
      //   mySerial.write("UP!\r\n");
      // }
      // if(leftPressed){
      //   mySerial.write("LEFT!\r\n");
      // }

      //***************
      if(mySerial.available()){
        char tmp = (char)mySerial.read();
        Serial.write(tmp);
        instructionPush(tmp);
        instructionHandler();
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
      upleftPressed = 0;
      uprightPressed = 0;
      downleftPressed = 0;
      downrightPressed = 0;
    break;
    case a2dListening:
      LRValue = analogRead(LR_INPUT);
      LRValue = map(LRValue, 0, 1023, 0, 255);
      delay(2);
      UDValue = analogRead(UD_INPUT);
      UDValue = map(UDValue, 0, 1023, 0, 255);
      delay(2);
      btnFlag = digitalRead(CONFRIM_BTN_PIN);

      // for joystick
      leftPressed = (LRValue < 103)?1:0;
      rightPressed = (LRValue > 153)?1:0;
      downPressed = (UDValue < 103)?1:0;
      upPressed = (UDValue > 153)?1:0;
      upleftPressed = (leftPressed & upPressed)?1:0;
      uprightPressed = (rightPressed & upPressed)?1:0;
      downleftPressed = (leftPressed & downPressed)?1:0;
      downrightPressed = (rightPressed & downPressed)?1:0;
      // for verifying the pattern
      if(!btnFlag){
        unLocked = res_compare();
        if (unLocked) {
          sendMsg();
        }
        display_init();
      }
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

void SendSecTask(){
  send_Init();
  for(;;){
    send_Tick();
    vTaskDelay(1);
  }
}

void StepperSecTask(){
  stepper_Init();
  for(;;){
    stepper_Tick();
    vTaskDelay(50);
  }
}

void UartSecTask(){
  uart_Init();
  for(;;){
    uart_Tick();
    // vTaskDelay(1);
  }
}

void A2dSecTask(){
  a2d_Init();
  for(;;){
    a2d_Tick();
    vTaskDelay(20);
  }
}

void StartSendSecPulse(unsigned portBASE_TYPE Priority){
  xTaskCreate(SendSecTask, (signed portCHAR *)"SendSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
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
