#include <SimpleZigBeeRadio.h>
#include <SimpleZigBeePacket.h>
SimpleZigBeeRadio xbee = SimpleZigBeeRadio();
SimpleOutgoingZigBeePacket zbp = SimpleOutgoingZigBeePacket();

int addresses[] = {0x41270684, 0x41558EC9, 0x412706A0};
uint8_t exFrame[] = { 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff };

void setup() {
  // Start the serial ports ...
  Serial.begin( 9600 );
  Serial1.begin(9600);
  xbee.setSerial( Serial1 );
  xbee.setAcknowledgement(true);
  Serial.println("Ready to send");
  zbp.setFrameType(ZIGBEE_TRANSMIT_REQUEST);
  zbp.setFrameID(0x01);
  zbp.setAddress(0x00000000, 0x00000000, 0xfffe);
  zbp.setTXRequestBroadcastRadius(0x00);
  zbp.setTXRequestOption(0x00);
  uint8_t exPayload[] = { 0xa1, 0xa2, 0xa3 };
  zbp.setTXRequestPayload( exPayload, sizeof(exPayload) ); // Same as zbp.setPayload( exPayload, 3 );

  // Now that the TX packet is complete,
  // we will display the contents to the serial port.
  printPacket(zbp);
}

void loop() {
  xbee.send(zbp);
  getResponseFromX();
//  sendToAll();
  delay(1000);
}

void sendToAll() {
  xbee.prepareTXRequestToCoordinator(exFrame, sizeof(exFrame));
  xbee.send();
  getResponseFromX();
  printPacket(xbee.getOutgoingPacketObject());
  delay(1000);
}

void getResponseFromX() {
  while (xbee.available()) {
    xbee.read();
    if (xbee.isComplete()) {
      if (xbee.isTXStatus()) {
        Serial.println("TX Status");
        Serial.print("Status: ");
        Serial.println(xbee.getTXStatusDeliveryStatus(), HEX);
      } else if (xbee.isATResponse()) {
        Serial.println("AT Command Response");
        Serial.print("Status: ");
        Serial.println(xbee.getATResponseStatus(), HEX);
      }
    }
  }
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
