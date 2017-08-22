void setup() {
  Serial.begin(9600);
  unsigned long start_time;
  unsigned long end_time;
  unsigned long i;
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  uint8_t test_on = 1;
  uint8_t test_off = 0;
  bool result;

  start_time = micros();
  for (i = 0; i < 1000000; i++) {
    //    digitalRead(4);
    result = test_on && digitalRead(4);
  }
  end_time = micros();
  Serial.println(end_time - start_time - 2452320); // 2386993 is time to read pin 4
}

void loop() {
}
