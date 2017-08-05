#include <SimpleZigBeeRadio.h>
//#include "FastLED.h"
#include <SimpleTimer.h>

// Create the XBee object ...
SimpleZigBeeRadio xbee = SimpleZigBeeRadio();
SimpleTimer t;
//CRGB LEDs[1900];

uint8_t flag = 0;
uint8_t state = 0;


void setup() {
//  FastLED.addLeds<UCS1903B, 6, RGB>(LEDs, 0, 190);
//  FastLED.addLeds<UCS1903B, 7, RGB>(LEDs, 190, 190);
//  FastLED.addLeds<UCS1903B, 8, RGB>(LEDs, 380, 190);
//  FastLED.addLeds<UCS1903B, 9, RGB>(LEDs, 570, 190);
//  FastLED.addLeds<UCS1903B, 10, RGB>(LEDs, 760, 190);
//  FastLED.addLeds<UCS1903B, 11, RGB>(LEDs, 950, 190);
//  FastLED.addLeds<UCS1903B, 12, RGB>(LEDs, 1140, 190);
//  FastLED.addLeds<UCS1903B, 13, RGB>(LEDs, 1330, 190);
//  FastLED.addLeds<UCS1903B, 14, RGB>(LEDs, 1520, 190);
//  FastLED.addLeds<UCS1903B, 15, RGB>(LEDs, 1710, 190);
  // Start the serial ports ...
  Serial.begin(9600);
  Serial1.begin( 9600 );
  // ... and set the serial port for the XBee radio.
  xbee.setSerial( Serial1 );
  // Set a non-zero frame id to receive Status and Response packets.
  xbee.setAcknowledgement(true);
  pinMode(10, OUTPUT);
  t.setInterval(1000, xbeeRead);
  Serial.println("Ready to receive.");
}

void loop() {
  t.run();
  if (!state) {
    blinkOnOff();
  } else {
    alwaysOn();
  }
  delay(10); // Small delay for stability
}

void blinkOnOff() {
  for (int i = 0; i < 10; i++) {
    t.run();
    if (flag) {
      flag = 0;
      return;
    } else {
      digitalWrite(10, HIGH);
      delay(100);
      digitalWrite(10, LOW);
      delay(100);
    }
  }
}

void alwaysOn() {
  for (int i = 0; i < 10; i++) {
    t.run();
    if (flag) {
      flag = 0;
      return;
    } else {
      digitalWrite(10, HIGH);
      delay(100);
    }
  }
}

void xbeeRead() {
    // If data is waiting in the XBee serial port ...
  while ( xbee.available() ) {
    // ... read the data.
    xbee.read();
    // If a complete message is available, display the contents
    if ( xbee.isComplete() ) {
      Serial.print("\nIncoming Message: ");
      SimpleZigBeePacket m = xbee.getIncomingPacketObject();
      printPacket(m);
      //printPacket( xbee.getIncomingPacketObject() );
      if (lastByte(m)) {  // Tx should send a 1 whenever turn on or off
        state = !state;
      }
      flag = 1; // There was a change in state
    }
  }
}

bool lastByte(SimpleZigBeePacket & stuff) {
  return stuff.getFrameData(stuff.getFrameLength() - 1);
}

void printPacket(SimpleZigBeePacket & p) {
  Serial.print( START, HEX );
  Serial.print(' ');
  Serial.print( p.getLengthMSB(), HEX );
  Serial.print(' ');
  Serial.print( p.getLengthLSB(), HEX );
  Serial.print(' ');
  // Frame Type and Frame ID are stored in Frame Data
  uint8_t checksum = 0;
  for ( int i = 0; i < p.getFrameLength(); i++) {
    Serial.print( p.getFrameData(i), HEX );
    Serial.print(' ');
    checksum += p.getFrameData(i);
  }
  // Calculate checksum based on summation of frame bytes
  checksum = 0xff - checksum;
  Serial.print(checksum, HEX );
  Serial.println();
}
