uint8_t POD = 1;
uint8_t state[] = {0, 0, 0};

void setup() {
  Serial.begin(9600);
  change_state_test();
//  occupied_test();
//  paired_test();
//  all_occupied_test();
}

void loop() {
}

void reset_state() {
  POD = 1;
  state[0] = 0;
  state[1] = 0;
  state[2] = 0;
}

void change_state_test() {
  Serial.println("Change state test");
  reset_state();

  Serial.println("Expected: 0 4 1");
  change_state(0b00001011);
  Serial.print("Actual: ");
  print_state(); // expected: 0 4 1
  Serial.println("Expected: 4 5 1");
  change_state(0b01001011);
  Serial.print("Actual: ");
  print_state(); // expected: 4 5 1
  Serial.println("Expected: 0 4 1");
  change_state(0b01001000);
  Serial.print("Actual: ");
  print_state();
}

void print_state() {
  for (size_t i = 0; i < 3; i++) {
    Serial.print(state[i]);
    Serial.print(' ');
  }
  Serial.println();
}

void change_state(uint8_t data) {
  uint8_t entered_state = (data & 0x01);
  uint8_t ready_state = ((data & 0x02) >> 1);
  uint8_t pod_num = (data >> 4);
  uint8_t pos;
  if (pod_num > 6) {
    pos = pod_num - 7;
    if ((ready_state << pos) ^ state[0]) {
      state[0] ^= (0x01 << pos);
    }
  } else {
    pos = pod_num + 1;
    if ((ready_state << pos) ^ state[1]) {
      state[1] ^= 0x01 << pos;
    }
  }
  if (pod_num > 8) {
    pos = pod_num - 9;
    if ((entered_state << pos) ^ state[1]) {
      state[1] ^= 0x01 << pos;
    }
  } else {
    pos = pod_num - 1;
    if ((entered_state << pos) ^ state[2]) {
      state[2] ^= 0x01 << pos;
    }
  }
}

void occupied_test() {
  Serial.println("Occupied test");
  reset_state();

  Serial.println("Expected: 0 0");
  Serial.print("Actual: ");
  Serial.print(occupied()); // expected: 0
  Serial.print(' ');
  Serial.println(occupied(1));
  state[2] = 1;
  Serial.println("Expected: 1 1");
  Serial.print("Actual: ");
  Serial.print(occupied()); // expected: 1
  Serial.print(' ');
  Serial.println(occupied(1));
  POD = 10;
  Serial.println("Expected: 0 0");
  Serial.print("Actual: ");
  Serial.print(occupied()); // expected: 0
  Serial.print(' ');
  Serial.println(occupied(10));
  state[1] = 2;
  Serial.println("Expected: 1 1");
  Serial.print("Actual: ");
  Serial.print(occupied()); // expected: 1
  Serial.print(' ');
  Serial.println(occupied(10));
}

bool occupied() {
  if (POD > 8) {
    return state[1] & (0x01 << (POD - 9));
  }
  return state[2] & (0x01 << (POD - 1));
}

bool occupied(uint8_t pod_num) {
  if (pod_num > 8) {
    return (state[1] >> (pod_num - 9)) & 0x01;
  }
  return (state[2] >> (pod_num - 1)) & 0x01;
}

void paired_test() {
  Serial.println("Paired test");
  reset_state();

  Serial.println("Expected: 0");
  Serial.print("Actual: ");
  Serial.println(paired()); // expected: 0
  change_state(0b00001001); // 1 and 4
  change_state(0b00100001);
  Serial.println("Expected: 1");
  Serial.print("Actual: ");
  Serial.println(paired()); // expected: 1

  POD = 3;
  Serial.println("Expected: 0");
  Serial.print("Actual: ");
  Serial.println(paired()); // expected: 0
  change_state(0b00010001); // 2 and 3
  change_state(0b00011001);
  Serial.println("Expected: 1");
  Serial.print("Actual: ");
  Serial.println(paired()); // expected: 1

  POD = 5;
  Serial.println("Expected: 0");
  Serial.print("Actual: ");
  Serial.println(paired()); // expected: 0
  change_state(0b00101001); // 5 and 6
  change_state(0b00110001);
  Serial.println("Expected: 1");
  Serial.print("Actual: ");
  Serial.println(paired()); // expected: 1

  POD = 7;
  Serial.println("Expected: 0");
  Serial.print("Actual: ");
  Serial.println(paired()); // expected: 0
  change_state(0b00111001); // 7 and 8
  change_state(0b01000001);
  Serial.println("Expected: 1");
  Serial.print("Actual: ");
  Serial.println(paired()); // expected: 1

  POD = 9;
  Serial.println("Expected: 0");
  Serial.print("Actual: ");
  Serial.println(paired()); // expected: 0
  change_state(0b01001001); // 9 and 10
  change_state(0b01010001);
  Serial.println("Expected: 1");
  Serial.print("Actual: ");
  Serial.println(paired()); // expected: 1
}

bool paired() {
  if (POD == 1 || POD == 4) {
    return occupied(1) && occupied(4);
  }
  if (POD == 2 || POD == 3) {
    return occupied(2) && occupied(3);
  }
  if (POD == 5 || POD == 6) {
    return occupied(5) && occupied(6);
  }
  if (POD == 7 || POD == 8) {
    return occupied(7) && occupied(8);
  }
  if (POD == 9 || POD == 10) {
    return occupied(9) && occupied(10);
  }
}

void all_occupied_test() {
  Serial.println("All occupied test");
  reset_state();

  Serial.println("Expected: 0");
  Serial.print("Actual: ");
  Serial.println(all_occupied()); // expected: 0
  change_state(0b00001001);
  change_state(0b00100001);
  change_state(0b00010001);
  change_state(0b00011001);
  change_state(0b00101001);
  change_state(0b00110001);
  Serial.println("Expected: 0");
  Serial.print("Actual: ");
  Serial.println(all_occupied()); // expected: 0
  change_state(0b00111001);
  change_state(0b01000001);
  change_state(0b01001001);
  change_state(0b01010001);
  Serial.println("Expected: 1");
  Serial.print("Actual: ");
  Serial.println(all_occupied()); // expected: 1
}

bool all_occupied() {
  bool occ = true;
  for (int i = 1; i < 11; i++) {
    occ = occ && occupied(i);
  }
  return occ;
}

void is_ready_test() {
  Serial.println("Is ready test");
  reset_state();

  Serial.println("Expected: 0 0");
  Serial.print("Actual: ");
  Serial.print(is_ready());
  Serial.print(' ');
  Serial.println(is_ready(1));
  state[1] = 4;
  Serial.println("Expected: 1 1");
  Serial.print("Actual: ");
  Serial.print(is_ready());
  Serial.print(' ');
  Serial.println(is_ready(1));

  POD = 8;
  Serial.println("Expected: 0 0");
  Serial.print("Actual: ");
  Serial.print(is_ready());
  Serial.print(' ');
  Serial.println(is_ready(8));
  state[0] = 2;
  Serial.println("Expected: 1 1");
  Serial.print("Actual: ");
  Serial.print(is_ready());
  Serial.print(' ');
  Serial.println(is_ready(8));
}

bool is_ready() {
  if (POD > 6) {
    return state[0] & (0x01 << (POD - 7));
  }
  return state[1] & (0x01 << (POD + 1));
}

bool is_ready(uint8_t pod_number) {
  if (pod_number > 6) {
    return state[0] & (0x01 << (POD - 7));
  }
  return state[1] & (0x01 << (POD + 1));
}

void all_ready_test() {
  Serial.println("All ready test");
  reset_state();

  Serial.println("Expected: 0");
  Serial.print("Actual: ");
  Serial.println(all_ready());

  state[1] = 252;
  Serial.println("Expected: 0");
  Serial.print("Actual: ");
  Serial.println(all_ready());

  state[0] = 15;
  Serial.println("Expected: 1");
  Serial.print("Actual: ");
  Serial.println(all_ready());
}

bool all_ready() {
  bool rd = true;
  for (int i = 1; i < 11; i++) {
    rd = rd && is_ready(i);
  }
  return rd;
}
