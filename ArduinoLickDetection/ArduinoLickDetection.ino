//Functions declaration
void TTLinputRising();

uint32_t pressedTimeStamp;
const uint32_t debounceDelay = 500;

volatile boolean flagTTLinput = false;


const int TTLinputPin = 2;
const int outputPin = 6;


void setup() {
  pressedTimeStamp = millis();
  Serial.begin(9600);
  pinMode(TTLinputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(TTLinputPin), TTLinputRising, RISING);
}

void loop(){
  if (flagTTLinput) {
    flagTTLinput = false;
    valveActivation();
  }
}


void TTLinputRising() {
  flagTTLinput = true;
}

void valveActivation() {
  if (millis() > pressedTimeStamp + debounceDelay) {
    pressedTimeStamp = millis();
    digitalWrite(outputPin, !digitalRead(outputPin));
    Serial.println("valve activated");
  }
}

