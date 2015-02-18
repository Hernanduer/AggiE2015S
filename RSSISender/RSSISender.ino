/* USB to Serial - Teensy becomes a USB to Serial converter
 http://dorkbotpdx.org/blog/paul/teensy_as_benito_at_57600_baud
 
 You must select Serial from the "Tools > USB Type" menu
 
 This example code is in the public domain.
 */

// set this to the hardware serial port you wish to use
#define HWSERIAL Serial1

unsigned long baud = 9600;
const int reset_pin = 4;
const int led_pin = 13;  // 13 = Teensy 3.X, 11 = Teensy 2.0, 6 = Teensy++ 2.0
const int led_on = HIGH;
const int led_off = LOW;

void setup() {
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, led_off);
  digitalWrite(reset_pin, HIGH);
  pinMode(reset_pin, OUTPUT);
  Serial.begin(baud);	// USB, communication to PC or Mac
  HWSERIAL.begin(baud);	// communication to hardware serial
}

void writeStuff(const char*co, const int s, const bool wait = true) {
  digitalWrite(led_pin, led_on);
  for (int i = 0; i < s; ++i) {
    HWSERIAL.write(co[i]);
    delay(10);
  }
  digitalWrite(led_pin, led_off);
  while (!HWSERIAL.available() && wait) {
    delay(10);
  }
}

void loop() {
  unsigned char c;
  String out = String();
  while (1) {
    while (!HWSERIAL.available()) {
      delay(10);
    }
    while (HWSERIAL.available()) {
      if (HWSERIAL.read() == 'L') {
        writeStuff("+++", 3);
        while (HWSERIAL.available()) {
          HWSERIAL.read();
        }
        out = String();
        writeStuff("ATDB\r", 5);
        while (HWSERIAL.available()) {
          c = HWSERIAL.read();
          if (c == 'L') {
            delay(20);
            continue;
          }
          out += (char)c;
          delay(10);
        }
        Serial.println(out);
        writeStuff("ATCN\r", 5, false);
      }
      delay(10);
    }
  }
}








