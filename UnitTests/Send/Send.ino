#include <SimpleZigBeeRadio.h>
// Create the XBee object ...
SimpleZigBeeRadio xbee = SimpleZigBeeRadio();

// Value and payload to be sent
int val = 0;
// Variables to store time
unsigned long time = 0;
unsigned long last_sent = 0;
uint8_t POD = 1;
uint8_t state[] = {0, 0, 0};
uint8_t seq_group[] = {0, 0, 0, 0, 0};   // records data for every 3 entries - each entry takes 4 bits (possible values 1-9). Only master needs to change this

void setup() {
  // Start the serial ports ...
  Serial.begin( 9600 );
  Serial3.begin(9600);

  //  xbeeSerial.begin( 9600 );
  // ... and set the serial port for the XBee radio.
  xbee.setSerial( Serial3 );
  // Receive TX Status packets
  xbee.setAcknowledgement(true);
  // The frame data in a ZigBee packet refers to the data between
  // the length LSB and the checksum.
  // Now store the example frame in the packet object.
  Serial.println("Ready to send");
}

uint8_t data;
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
      case 'z': set_occupied(true); break;
      case 'x': set_occupied(false); break;
      case 'c': set_ready(); break;
      case 'v': reset_ready(POD); break;
      case '1': data = prepare_msg(false); send_update(data); break;
      case '2': data = prepare_msg(true); send_update(data); break;
      case '3': send_sequence(0x11); Serial.println(0x11,BIN); break;
      default: continue;
    }
  }
//  printPacket(xbee.getOutgoingPacketObject());
  Serial.println(data, BIN);
  delay(1000);
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

uint8_t prepare_msg(bool go) {  // returns the byte message to send for a "status update"
  uint8_t msg = POD << 4;  // byte msg format: first 4 MSB is pod number, 5th MSB is flag to indicate msg is master's sequence assignment - interpret whole msg differently
  // 6th MSB is flag for "go" signal when all pods are ready to enter all_occupied funciton, 7th MSB is flag to indicate that pod sending the msg is ready
  // LSB (8th MSB) is flag to indicate whether pod sending the msg is occupied
  if (go) msg |= 0x04;
  if (is_ready(POD)) msg |= 0x02;
  if (occupied(POD)) msg |= 0x01;
  return msg;
}

void send_sequence(uint8_t data) {
  uint8_t seq = check_set_seq(data);
  if (data & 0x01) {  // pod which sent this message is now occupied; must assign it to some sequence number from 1 to 3
    send_update((data & 0b11110000) | (0x08 | seq));   // 0x08 means 4th LSB is 1, and OR with seq_group / 3 + 1 means the first 3 bits will represent the sequence number to play (1 to 3)
  }
  // the pod will ownself check that it has finished and go back to default non-occupied state sequence
}

uint8_t check_set_seq(uint8_t data) {
  uint8_t replacewith;
  uint8_t findval;
  if (data & 0x01) {
    findval = 0;
    replacewith = data >> 4;
  } else {
    findval = data >> 4;
    replacewith = 0;
  }
  for (size_t i = 1; i < 6; i++) {
    if (i < 5) {
      if ((seq_group[i-1] & 0b00001111) == findval) {
        seq_group[i-1] |= replacewith;
        if ((i % 3) == 1) return (i * 2 + 1) / 3;
        else return (i * 2 + 2) / 3;
      }
    }
    if ((seq_group[i-1] >> 4) == findval) {
      seq_group[i-1] |= replacewith << 4;
      if ((i % 3) == 0) return i * 2 / 3;
      else return (i * 2 + 1) / 3;
    }
  }
}

void set_occupied(bool occ) {
  uint8_t set = 0;
  if (occ) set = 1;
  if (POD > 8) {
    state[1] ^= (-set ^ state[1]) & (1 << (POD - 9));
  } else {
    state[2] ^= (-set ^ state[2]) & (1 << (POD - 1));
  }
}

bool occupied(uint8_t pod_num) {
  if (pod_num > 8) {
    return state[1] & (0x01 << (POD - 9));
  }
  return state[2] & (0x01 << (POD - 1));
}

void set_ready() {
  if (POD > 6) {
    state[0] |= 0x01 << (POD - 7);
  } else {
    state[1] |= 0x01 << (POD + 1);
  }
}

bool is_ready(uint8_t pod_num) {  // returns true if pod_num pod's "ready" state is 1
  if (pod_num > 6) {
    return state[0] & (0x01 << (pod_num - 7));
  }
  return state[1] & (0x01 << (pod_num + 1));
}

void send_update(uint8_t msg) { // cannot join w/ prepare_msg function because must generate special msg for master instructions
  uint8_t payload[] = {msg};
  xbee.prepareTXRequestBroadcast(payload, sizeof(payload));
  xbee.send();
}

void reset_ready(uint8_t pod_num) {  // sets the pod_num pod "ready" state to 0
  if (pod_num > 6) {
    state[0] &= ~(0x01 << (pod_num - 7));
  } else {
    state[1] &= ~(0x01 << (pod_num + 1));
  }
}

void reset_ready_all() {  // sets all pods' "ready" state to 0
  for (size_t i = 1; i < 11; i++) {
    reset_ready(i);
  }
}

void print_state() {
  for (int i = 0; i < 3; i++) {
    Serial.print(state[i], BIN);
    Serial.print(' ');
  }
  Serial.println();
}

