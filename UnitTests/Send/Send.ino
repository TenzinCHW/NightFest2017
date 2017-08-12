#include <SimpleZigBeeRadio.h>
//#include <SoftwareSerial.h>
// Create the XBee object ...
SimpleZigBeeRadio xbee = SimpleZigBeeRadio();
// ... and the software serial port. Note: Only one
// SoftwareSerial object can receive data at a time.
//SoftwareSerial xbeeSerial(2, 3); // (RX=>DOUT, TX=>DIN)
// Packet to send: In this example, we will update
// the contents of a packet before sending it.
SimpleZigBeePacket zbp = SimpleZigBeePacket();

// Value and payload to be sent
int val = 0;
// Variables to store time
unsigned long time = 0;
unsigned long last_sent = 0;
uint8_t POD = 10;
uint8_t state[] = {0, 0, 0};

void setup() {
  // Start the serial ports ...
  Serial.begin( 9600 );
  Serial1.begin(9600);

  //  xbeeSerial.begin( 9600 );
  // ... and set the serial port for the XBee radio.
  xbee.setSerial( Serial1 );
  // Receive TX Status packets
  xbee.setAcknowledgement(true);
  // The frame data in a ZigBee packet refers to the data between
  // the length LSB and the checksum.
  uint8_t exFrame[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  // Now store the example frame in the packet object.
  zbp.setFrameData(0, exFrame, sizeof(exFrame));
  Serial.println("Ready to send");
}

uint8_t data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char m;

void loop() {
  while (Serial.available()) {
    m = Serial.read();
    switch (m) {
      case 'a': POD = 1; break;
      case 's': POD = 2; break;
      case 'd': POD = 3; break;
      case 'f': POD = 4; break;
      case 'g': POD = 5; break;
      case 'h': POD = 6; break;
      case 'j': POD = 7; break;
      case 'k': POD = 8; break;
      case 'l': POD = 9; break;
      case ';': POD = 10; break;
      case '1': send_status(0); printPacket(zbp); break;
      case '2': send_status(1); printPacket(zbp); break;
      default: continue;
    }
  }
  delay(1000);
}



void send_status(uint8_t stat) {
  data[POD - 1] = stat;
  for (int i = 0; i < 10; i++) {
    zbp.setFrameData(i, data[i]);
  }
  xbee.send(zbp);
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
  //   Calculate checksum based on summation of frame bytes
  checksum = 0xff - checksum;
  Serial.print(checksum, HEX );
  Serial.println();
}
