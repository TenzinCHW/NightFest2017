#include <SimpleZigBeeRadio.h>

int PIR = 21;
int LED = 13;
SimpleZigBeeRadio Xbee = SimpleZigBeeRadio();

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  Xbee.setSerial(Serial3);
  Xbee.setAcknowledgement(true);
  pinMode(PIR, INPUT);
  pinMode(20, INPUT);
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PIR), PIR_ISR, CHANGE);
  Serial.println("Ready");
}

unsigned long timenow;
unsigned long past;
unsigned long total;

void loop() {
  Serial.println("OMG WHAT");
  if (Xbee.available()) {
    Xbee.read();
    if (Xbee.isComplete()) {
      if (Xbee.isRX()) {
        Serial.println("Received something");
        Serial.println(Xbee.getRXPayload(0));
      }
    }
  }
  // NEVER digitalRead on a PIR! It gives false readings.
  // Always use an interrupt.
  //  if (digitalRead(PIR)) {
  //    past = millis();
  //    while (digitalRead(PIR)) {
  //      timenow = millis();
  //    }
  //    total = (timenow - past) / 1000;
  //    Serial.println(total);
  //  }
  delay(100);
}

void PIR_ISR() {
  if (digitalRead(PIR)) {
    Serial.println("PIR activated");
  } else {
    Serial.println("PIR deactivated");
  }
}
