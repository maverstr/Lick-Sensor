//https://github.com/GreyGnome/EnableInterrupt
#include "EnableInterrupt/EnableInterrupt.h"


//Functions declaration
void TTLinputRising();

uint32_t pressedTimeStamp;
const uint32_t debounceDelay = 110;

volatile boolean flagTTLinput = false;
volatile boolean flagSwitchInput = false;
volatile boolean flagBNCTrigger = false;
volatile boolean flagPushButton = false;

const int TTLinputPin = 2;
const int outputPin = 6;
const int switchPin = A0;
const int BNCTriggerPin = 3;
const int pushButtonPin = 13;


void setup() {
  pressedTimeStamp = millis();
  Serial.begin(500000);
  pinMode(TTLinputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  //pinMode(switchPin, INPUT);
  //pinMode(BNCTriggerPin, INPUT);
  pinMode(pushButtonPin, INPUT_PULLUP);

  enableInterrupt(TTLinputPin, TTLinputRising, RISING);
  //enableInterrupt(switchPin, switchInput, CHANGE);
  //enableInterrupt(BNCTriggerPin, BNCTrigger, RISING);
  enableInterrupt(pushButtonPin, pushButton, RISING);
}

void loop() {
  /*
  digitalWrite(outputPin, HIGH);
  Serial.println("on");
  delay(100);
  digitalWrite(outputPin,LOW);
  Serial.println("off");
  delay(50);*/
  
  if (flagTTLinput) {
    flagTTLinput = false;
    valveInputActivation();
  }
  if (flagSwitchInput) {
    flagSwitchInput = false;
    valveSwitchActivation();
  }
  if (flagBNCTrigger) {
    flagBNCTrigger = false;
    valveBNCTriggerActivation();
  }
  if(flagPushButton){
    flagPushButton = false;
    valvePushButtonActivation();
  }
}


void TTLinputRising() {
  flagTTLinput = true;
}

void switchInput() {
  flagSwitchInput = true;
}

void BNCTrigger() {
  flagBNCTrigger = true;
}

void pushButton(){
  flagPushButton = true;
}

void valveInputActivation() {
  if (millis() > pressedTimeStamp + debounceDelay) {
    delay(1);
    if (digitalRead(TTLinputPin)) { //debounce on release
      pressedTimeStamp = millis();
      digitalWrite(outputPin, HIGH);
      delay(100);
      digitalWrite(outputPin, LOW);
      //digitalWrite(outputPin, digitalRead(switchPin));
      Serial.println("valve activated input");
    }
  }
}

void valveSwitchActivation() {
  if (millis() > pressedTimeStamp + debounceDelay) {
    pressedTimeStamp = millis();
    digitalWrite(outputPin, digitalRead(switchPin));
    Serial.println("valve activated switch");
  }
}

void valveBNCTriggerActivation() {
  if (millis() > pressedTimeStamp + debounceDelay) {
    delay(1);
    if (digitalRead(BNCTriggerPin)) { //debounce on release
      pressedTimeStamp = millis();
      digitalWrite(outputPin, HIGH);
      delay(500);
      digitalWrite(outputPin, LOW);
      //digitalWrite(outputPin, digitalRead(switchPin));
      Serial.println("valve activated BNC");
    }
  }
}

void valvePushButtonActivation(){
    if (millis() > pressedTimeStamp + debounceDelay) {
    delay(1);
    if (!digitalRead(pushButtonPin)) { //debounce on release
      pressedTimeStamp = millis();
      digitalWrite(outputPin, HIGH);
      delay(200);
      digitalWrite(outputPin, LOW);
      //digitalWrite(outputPin, digitalRead(switchPin));
      Serial.println("valve activated pushButton");
    }
  }
}

