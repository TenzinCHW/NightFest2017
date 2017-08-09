#include <SimpleZigBeeRadio.h>
#include <SimpleTimer.h>

SimpleZigBeeRadio xbee = SimpleZigBeeRadio();
SimpleTimer t = SimpleTimer();

uint8_t flag = 0;
uint8_t state = 0;


void setup() {
  Serial.begin(9600);
  Serial1.begin( 9600 );
  xbee.setSerial( Serial1 );
  xbee.setAcknowledgement(true);
  pinMode(10, OUTPUT);
  xbee.prepareATCommand('AP', 2);
  xbee.send();
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
    
  while ( xbee.available() ) {
    // ... read the data.
    xbee.read();
    // If a complete message is available, display the contents
    if ( xbee.isComplete() ) {
      Serial.print("\nIncoming Message: ");
      SimpleZigBeePacket m = xbee.getIncomingPacketObject();
      printPacket(m);
      if (lastByte()) {  // Tx should send a 1 whenever turn on or off
        state = !state;
      }
      flag = 1; // There was a change in state
    }
  }
}

bool lastByte() {
  return xbee.getRXPayload(xbee.getRXPayloadLength() - 1);
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
