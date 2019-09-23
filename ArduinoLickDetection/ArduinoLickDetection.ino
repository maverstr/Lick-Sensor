//https://github.com/GreyGnome/EnableInterrupt
#include "EnableInterrupt/EnableInterrupt.h"

uint32_t pressedTimeStamp;
uint32_t pressedTimeStampSensor;
uint32_t pressedTimeStampBNC;
uint32_t timeStampDropletPresent;

const uint32_t debounceDelay = 500;

//INTERRUPT FLAGS
volatile boolean flagSensorInput = false;
volatile boolean flagswitchInputInterrupt = false;
volatile boolean flagBNCTrigger = false;
volatile boolean flagPushButton = false;
volatile boolean flagAirValveInput = false;

//GPIO
const int sensorInputPin = 2;
const int valveOutputPin = 6;
const int drainValveOutputPin = 7;
const int switchPin = A0;
const int BNCTriggerPin = 3;
const int pushButtonPin = 13;

//DURATIONS
const int mainValveActivationDuration = 200;
const int drainValveActivationDuration = 20;
const int drainValveAutoActivationDelay = 6000;

//ODORS
const int rewardList[] = {1}; //TO ADAPT TO THE EXPERIMENT
bool flagRewardingOdor = false;
int odorCounter = 0;
int rewardListLength = 0;

//DROPLET FLAG
bool isDropletPresent = false;


void setup() {
  rewardListLength = sizeof(rewardList);
  pressedTimeStamp = millis();
  Serial.begin(500000);
  pinMode(sensorInputPin, INPUT);
  pinMode(valveOutputPin, OUTPUT);
  pinMode(drainValveOutputPin, OUTPUT);
  pinMode(switchPin, INPUT);
  pinMode(BNCTriggerPin, INPUT);
  pinMode(pushButtonPin, INPUT_PULLUP);

  enableInterrupt(sensorInputPin, sensorInputInterrrupt, FALLING);
  enableInterrupt(pushButtonPin, AirValveTriggerInterrupt, RISING); //to trigger the drain
  enableInterrupt(switchPin, switchInputInterrupt, CHANGE);
  //enableInterrupt(pushButtonPin, pushButtonInterrupt, RISING);//to trigger the main valve
  //enableInterrupt(BNCTriggerPin, BNCTriggerInterrupt, RISING);
}

void loop() {
  if (flagSensorInput) {
    flagSensorInput = false;
    valveSensorActivation();
    //rewardDelivery();
  }
  if (flagswitchInputInterrupt) {
    flagswitchInputInterrupt = false;
    valveSwitchActivation();
  }
  if (flagPushButton) {
    flagPushButton = false;
    valvePushButtonActivation();
  }
  if (flagAirValveInput) {
    flagAirValveInput = false;
    drainValveActivation();
  }
  if (flagBNCTrigger) {
    flagBNCTrigger = false;
    assessRewardingOdorList();
  }
  dropletAutomaticRemoval();
}

// ===============================
// =====      INTERRUPTS      ====
// ===============================

void sensorInputInterrrupt() {
  flagSensorInput = true;
}

void switchInputInterrupt() {
  flagswitchInputInterrupt = true;
}

void pushButtonInterrupt() {
  flagPushButton = true;
}

void AirValveTriggerInterrupt() {
  flagAirValveInput = true;
}

void BNCTriggerInterrupt() {
  flagBNCTrigger = true;
}

// ===============================
// ====  Activation handling  ====
// ===============================

void rewardDelivery() {
  if (millis() > pressedTimeStamp + debounceDelay && flagRewardingOdor) {
    delay(1);
    if (!digitalRead(sensorInputPin)) { //debounce on release
      pressedTimeStamp = millis();
      digitalWrite(valveOutputPin, HIGH);
      delay(mainValveActivationDuration);
      //digitalWrite(valveOutputPin, LOW);
      digitalWrite(valveOutputPin, digitalRead(switchPin));
      Serial.println("valve activated input");
      isDropletPresent = true;
      timeStampDropletPresent = millis();
    }
  }
}

void valveSensorActivation() {
  if (millis() > pressedTimeStampSensor + debounceDelay) {
    delay(1);
    if (!digitalRead(sensorInputPin)) { //debounce on release
      pressedTimeStampSensor = millis();
      digitalWrite(valveOutputPin, HIGH);
      delay(mainValveActivationDuration);
      //digitalWrite(valveOutputPin, LOW);
      digitalWrite(valveOutputPin, digitalRead(switchPin));
      Serial.println("valve activated input");
      timeStampDropletPresent = millis();
      isDropletPresent = true;
    }
  }
}

void valveSwitchActivation() {
  if (millis() > pressedTimeStamp + debounceDelay) {
    pressedTimeStamp = millis();
    digitalWrite(valveOutputPin, digitalRead(switchPin));
    Serial.println("valve activated switch");
    if (!digitalRead(switchPin)) { //if switched off, meaning stopped flowing
    timeStampDropletPresent = millis();
      isDropletPresent = true;
    }
  }
}

void valvePushButtonActivation() {
  if (millis() > pressedTimeStamp + debounceDelay) {
    delay(1);
    if (!digitalRead(pushButtonPin)) { //debounce on release
      pressedTimeStamp = millis();
      digitalWrite(valveOutputPin, HIGH);
      delay(mainValveActivationDuration);
      //digitalWrite(valveOutputPin, LOW);
      digitalWrite(valveOutputPin, digitalRead(switchPin));
      Serial.println("valve activated pushButton");
      isDropletPresent = true;
      timeStampDropletPresent = millis();
    }
  }
}

void drainValveActivation() {
  if (millis() > pressedTimeStamp + debounceDelay) {
    pressedTimeStamp = millis();
    digitalWrite(drainValveOutputPin, HIGH);
    delay(drainValveActivationDuration);
    digitalWrite(drainValveOutputPin, LOW);
    Serial.println("DRAIN activated");
    isDropletPresent = false;
  }
}

// ===============================
// =====     odors reward     ====
// ===============================

void assessRewardingOdorList() {
  if (millis() > pressedTimeStampBNC + debounceDelay) {
    pressedTimeStampBNC = millis();
    if (odorCounter > rewardListLength) {
      odorCounter = 0;
    }
    flagRewardingOdor = rewardList[odorCounter];
    odorCounter++;
  }
}

// ===============================
// ===== droplet auto removal ====
// ===============================

void dropletAutomaticRemoval() {
  if (isDropletPresent && millis() > timeStampDropletPresent + drainValveAutoActivationDelay) {
    digitalWrite(drainValveOutputPin, HIGH);
    delay(drainValveActivationDuration);
    digitalWrite(drainValveOutputPin, LOW);
    Serial.print("DRAIN AUTO ACTIVATION AFTER : "); Serial.println(drainValveAutoActivationDelay);
    isDropletPresent = false;
  }
}


