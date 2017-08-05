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
  uint8_t exFrame[] = { 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff };
  // Now store the example frame in the packet object.
  zbp.setFrameData(0, exFrame, sizeof(exFrame));
  Serial.println("Ready to send");
}

void loop() {
  printPacket(zbp);
  xbee.send(zbp);
  delay(10000);
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
