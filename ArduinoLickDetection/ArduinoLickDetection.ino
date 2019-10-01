//https://github.com/GreyGnome/EnableInterrupt
#include "EnableInterrupt/EnableInterrupt.h"

uint32_t pressedTimeStampMainValve;
uint32_t pressedTimeStampDrainValve;
uint32_t pressedTimeStampSensor;
uint32_t pressedTimeStampBNC;
uint32_t timeStampDropletPresent;

const uint32_t debounceDelay = 500;
const uint32_t debounceDelayToggleSwitches = 200;

//INTERRUPT FLAGS
volatile boolean flagTouchSensorInput = false;
volatile boolean flagMainValvePushButton = false;
volatile boolean flagDrainValvePushButton = false;
volatile boolean flagMainValveSwitchInput = false;
volatile boolean flagDrainValveSwitchInput = false;
volatile boolean flagBNCTrigger = false;

// ===============================
// =====         GPIO         ====
// ===============================
//valves
const int mainValveOutputPin = 6;
const int drainValveOutputPin = 7;
//toggle switches
const int mainValveSwitchPin = A0;
const int drainValveSwitchPin = A1;
//pushbuttons
const int mainValvePushButtonPin = 13;
const int drainValvePushButtonPin = 12;
//BNC
const int BNCTriggerPin = 3;
//touchSensor
const int touchSensorInputPin = 2;

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
  //sets initial time stamps
  pressedTimeStampMainValve = millis();
  pressedTimeStampDrainValve = millis();
  pressedTimeStampSensor = millis();
  pressedTimeStampBNC = millis();

  Serial.begin(500000);
  pinMode(mainValveOutputPin, OUTPUT);
  pinMode(drainValveOutputPin, OUTPUT);
  pinMode(mainValveSwitchPin, INPUT_PULLUP);
  pinMode(drainValveSwitchPin, INPUT_PULLUP);
  pinMode(mainValvePushButtonPin, INPUT_PULLUP);
  pinMode(drainValvePushButtonPin, INPUT_PULLUP);
  pinMode(BNCTriggerPin, INPUT);
  pinMode(touchSensorInputPin, INPUT);

  //Sets the interrupts (pinNumber, ISR(interrupt service routine), edge of signal detection)
  enableInterrupt(touchSensorInputPin, touchSensorInputInterrrupt, FALLING);
  enableInterrupt(mainValvePushButtonPin, mainValvePushButtonInterrupt, RISING);
  enableInterrupt(mainValveSwitchPin, mainValveSwitchInterrupt, CHANGE);
  enableInterrupt(drainValvePushButtonPin, drainValvePushButtonInterrupt, RISING);
  enableInterrupt(drainValveSwitchPin, drainValveSwitchInterrupt, CHANGE);
  enableInterrupt(BNCTriggerPin, BNCTriggerInterrupt, RISING);
}

void loop() {
  //The loop assess all the flags from the interrupts and activates the relevant functions.
  //It then assess the time since a droplet is present for automatic droplet removal.
  if (flagTouchSensorInput) {
    flagTouchSensorInput = false;
    mainValveTouchSensorActivation();
    //rewardDelivery();
  }
  if (flagMainValvePushButton) {
    flagMainValvePushButton = false;
    mainValvePushButtonActivation();
  }
  if (flagDrainValvePushButton) {
    flagDrainValvePushButton = false;
    drainValvePushButtonActivation();
  }
  if (flagMainValveSwitchInput) {
    flagMainValveSwitchInput = false;
    mainValveSwitchActivation();
  }
  if (flagDrainValveSwitchInput) {
    flagDrainValveSwitchInput = false;
    drainValveSwitchActivation();
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

void touchSensorInputInterrrupt() {
  flagTouchSensorInput = true;
}

void mainValvePushButtonInterrupt() {
  flagMainValvePushButton = true;
}

void drainValvePushButtonInterrupt() {
  flagDrainValvePushButton = true;
}

void mainValveSwitchInterrupt() {
  flagMainValveSwitchInput = true;
}

void drainValveSwitchInterrupt() {
  flagDrainValveSwitchInput = true;
}

void BNCTriggerInterrupt() {
  flagBNCTrigger = true;
}

// ===============================
// ====  Activation handling  ====
// ===============================

void rewardDelivery() {
  if (millis() > pressedTimeStampSensor + debounceDelay && flagRewardingOdor) {
    delay(1);
    if (!digitalRead(touchSensorInputPin)) { //debounce on release
      pressedTimeStampSensor = millis();
      digitalWrite(mainValveOutputPin, HIGH);
      delay(mainValveActivationDuration);
      //digitalWrite(mainValveOutputPin, LOW);
      digitalWrite(mainValveOutputPin, digitalRead(mainValveSwitchPin));
      Serial.println("MAIN activated input");
      isDropletPresent = true;
      timeStampDropletPresent = millis();
    }
  }
}

void mainValveTouchSensorActivation() {
  if (millis() > pressedTimeStampSensor + debounceDelay) {
    delay(1);
    if (!digitalRead(touchSensorInputPin)) { //debounce on release
      pressedTimeStampSensor = millis();
      digitalWrite(mainValveOutputPin, HIGH);
      delay(mainValveActivationDuration);
      //digitalWrite(mainValveOutputPin, LOW);
      digitalWrite(mainValveOutputPin, digitalRead(mainValveSwitchPin));
      Serial.println("MAIN activated input");
      timeStampDropletPresent = millis();
      isDropletPresent = true;
    }
  }
}

void mainValveSwitchActivation() {
  if (millis() > pressedTimeStampMainValve + debounceDelayToggleSwitches) {
    pressedTimeStampMainValve = millis();
    digitalWrite(mainValveOutputPin, digitalRead(mainValveSwitchPin));
    Serial.println("MAIN activated switch");
    if (!digitalRead(mainValveSwitchPin)) { //if switched off, meaning stopped flowing
      timeStampDropletPresent = millis();
      isDropletPresent = true;
    }
  }
}

void drainValveSwitchActivation() {
  if (millis() > pressedTimeStampDrainValve + debounceDelayToggleSwitches) {
    pressedTimeStampDrainValve = millis();
    digitalWrite(drainValveOutputPin, digitalRead(drainValveSwitchPin));
    Serial.println("DRAIN activated switch");
    isDropletPresent = false;
  }
}

void mainValvePushButtonActivation() {
  if (millis() > pressedTimeStampMainValve + debounceDelay) {
    delay(1);
    if (!digitalRead(mainValvePushButtonPin)) { //debounce on release
      pressedTimeStampMainValve = millis();
      digitalWrite(mainValveOutputPin, HIGH);
      delay(mainValveActivationDuration);
      //digitalWrite(mainValveOutputPin, LOW);
      digitalWrite(mainValveOutputPin, digitalRead(mainValveSwitchPin));
      Serial.println("MAIN activated pushButton");
      isDropletPresent = true;
      timeStampDropletPresent = millis();
    }
  }
}

void drainValvePushButtonActivation() {
  if (millis() > pressedTimeStampDrainValve + debounceDelay) {
    delay(1);
    if (!digitalRead(drainValvePushButtonPin)) { //debounce on release
      pressedTimeStampDrainValve = millis();
      digitalWrite(drainValveOutputPin, HIGH);
      delay(drainValveActivationDuration);
      //digitalWrite(drainValveOutputPin, LOW);
      digitalWrite(drainValveOutputPin, digitalRead(drainValveSwitchPin));
      Serial.println("DRAIN activated pushButton");
      isDropletPresent = false;
    }
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


