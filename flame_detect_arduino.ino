  
#include <mcp_can.h>
#include <SPI.h>

//digital input pins
byte di_0_pin = 0; //analog input
byte di_1_pin = 1; //analog input
byte di_2_pin = 2; //analog input
byte di_3_pin = 3; //analog input
byte di_4_pin = 4; //analog input
byte di_5_pin = 5; //analog input
byte di_6_pin = 8;
byte di_7_pin = 9;

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
byte di_6_state = 0;
byte di_7_state = 0;

//card address states
byte address_bit_0_state = 0;
byte address_bit_1_state = 0;
byte address_bit_2_state = 0;

//misc
int ai_threshold = 512;
byte card_address = 0;
unsigned long main_timer = millis();
byte data = 0;
byte message[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  
  //Serial.begin(115200);
  CAN.begin(CAN_500KBPS);  
  
  pinMode(di_6_pin, INPUT);
  pinMode(di_7_pin, INPUT);
  pinMode(address_bit_0_pin, INPUT);
  pinMode(address_bit_1_pin, INPUT);
  pinMode(address_bit_2_pin, INPUT);

  analogWrite(6, 25); //this starts a 10% duty cycle output on digital pin 6

  //create card address
  address_bit_0_state = digitalRead(address_bit_0_pin);
  address_bit_1_state = digitalRead(address_bit_1_pin);
  address_bit_2_state = digitalRead(address_bit_2_pin);
  bitWrite(card_address, 0, address_bit_0_state);
  bitWrite(card_address, 1, address_bit_1_state);
  bitWrite(card_address, 2, address_bit_2_state);
  
}

void loop() {

  if ((millis() - main_timer) >= 100) {
    main_timer = millis();
    digitalInputs();
    CAN.sendMsgBuf(card_address, 0, 8, message);  //id, standard frame, data len, data buf
    //Serial.println(card_address);
    //Serial.println(message[0]);
  }
  
}  

void digitalInputs() {
  di_0_state = readAnalogDI(di_0_pin);
  di_1_state = readAnalogDI(di_1_pin);
  di_2_state = readAnalogDI(di_2_pin);
  di_3_state = readAnalogDI(di_3_pin);
  di_4_state = readAnalogDI(di_4_pin);
  di_5_state = readAnalogDI(di_5_pin);
  di_6_state = digitalRead(di_6_pin);
  di_7_state = digitalRead(di_7_pin);

  bitWrite(data, 0, di_0_state);
  bitWrite(data, 1, di_1_state);
  bitWrite(data, 2, di_2_state);
  bitWrite(data, 3, di_3_state);
  bitWrite(data, 4, di_4_state);
  bitWrite(data, 5, di_5_state);
  bitWrite(data, 6, di_6_state);
  bitWrite(data, 7, di_7_state);

  message[0] = data;
}

int readAnalogDI(byte pin) {
  int aiValue = analogRead(pin);
  int aiState = 0;
  if (aiValue > ai_threshold) {
    aiState = 1;
  }
  return aiState;
}

