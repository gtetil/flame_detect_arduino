  
#include <mcp_can.h>
#include <SPI.h>

//CAN variables
MCP_CAN CAN0(10); // Set CS to pin 10

//digital input pins
byte di_0_pin = 0; //analog input
byte di_1_pin = 1; //analog input
byte di_2_pin = 2; //analog input
byte di_3_pin = 3; //analog input
byte di_4_pin = 4; //analog input
byte di_5_pin = 5; //analog input
byte di_6_pin = 6; //analog input
byte di_7_pin = 7; //analog input
byte di_pins[8] = {di_0_pin, di_1_pin, di_2_pin, di_3_pin, di_4_pin, di_5_pin, di_6_pin, di_7_pin};
byte debug_trigger_pin = 1;  //this is also the serial Tx pin

//card address pins
byte address_bit_0_pin = 3;
byte address_bit_1_pin = 4;
byte address_bit_2_pin = 5;

//digital input states
byte di_0_state = 0; //analog input
byte di_1_state = 0; //analog input
byte di_2_state = 0; //analog input
byte di_3_state = 0; //analog input
byte di_4_state = 0; //analog input
byte di_5_state = 0; //analog input
byte di_6_state = 0; //analog input
byte di_7_state = 0; //analog input

//card address states
byte address_bit_0_state = 0;
byte address_bit_1_state = 0;
byte address_bit_2_state = 0;

//misc
int ai_threshold = 512;
byte card_address = 0;
unsigned long main_timer = millis();
byte data = 0;
byte prev_data = 0;
byte message[8] = {0, 0, 0, 0, 0, 0, 0, 0};
byte ai_data_lo[8];
byte ai_data_hi[8];

void setup() {
  
  //CAN setup
  CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ);
  pinMode(2, INPUT); // Setting pin 2 for /INT input
  CAN0.setMode(MCP_NORMAL); // Change to normal mode to allow messages to be transmitted

  //Serial.begin(115200);
  
  pinMode(di_6_pin, INPUT);
  pinMode(di_7_pin, INPUT);
  pinMode(address_bit_0_pin, INPUT_PULLUP);
  pinMode(address_bit_1_pin, INPUT_PULLUP);
  pinMode(address_bit_2_pin, INPUT_PULLUP);

  analogWrite(6, 25); //this starts a 10% duty cycle output on digital pin 6

  //create card address
  address_bit_0_state = !digitalRead(address_bit_0_pin);
  address_bit_1_state = !digitalRead(address_bit_1_pin);
  address_bit_2_state = !digitalRead(address_bit_2_pin);
  bitWrite(card_address, 0, address_bit_0_state);
  bitWrite(card_address, 1, address_bit_1_state);
  bitWrite(card_address, 2, address_bit_2_state);

  //to trigger analog debug via CAN
  pinMode(debug_trigger_pin, INPUT_PULLUP);
  
}

void loop() {

  if ((millis() - main_timer) >= 200) {
    main_timer = millis();
    digitalInputs();
    //Serial.println(card_address);
    //Serial.println(message[0]);   
  }
  
}  

void digitalInputs() {

  for (int i=0; i < 8; i++){
    readAnalogDI(di_pins[i], i);
  }

  if (data != prev_data) {
    message[0] = data;
    CAN0.sendMsgBuf(card_address, 0, 8, message);  //id, standard frame, data len, data buf
    prev_data = data;
  }

  //pull serial Tx pin low for debug
  if (digitalRead(debug_trigger_pin) == 0) {  
    CAN0.sendMsgBuf(card_address + 100, 0, 8, ai_data_lo);
    CAN0.sendMsgBuf(card_address + 200, 0, 8, ai_data_hi);
  }
}

int readAnalogDI(byte pin, int index) {
  int aiValue = analogRead(pin);
  int aiState = 0;
  if (aiValue > ai_threshold) {
    aiState = 1;
  }
  bitWrite(data, index, aiState);  //digital data

  //for debug, via CAN
  ai_data_lo[index] = lowByte(aiValue);
  ai_data_hi[index] = highByte(aiValue);
  
}

