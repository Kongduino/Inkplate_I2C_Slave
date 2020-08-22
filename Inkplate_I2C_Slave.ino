#include <Arduino.h>
#include <Wire.h>
#include <WireSlave.h>
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x04

void receiveEvent(int howMany);

char buff[128];

void hexDump(char *buf, uint16_t len) {
  String s = "|", t = "| |";
  Serial.println(F("  |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f |"));
  Serial.println(F("  +------------------------------------------------+ +----------------+"));
  for (uint16_t i = 0; i < len; i += 16) {
    for (uint8_t j = 0; j < 16; j++) {
      if (i + j >= len) {
        s = s + "   "; t = t + " ";
      } else {
        char c = buf[i + j];
        if (c < 16) s = s + "0";
        s = s + String(c, HEX) + " ";
        if (c < 32 || c > 127) t = t + ".";
        else t = t + (char)c;
      }
    }
    uint8_t index = i / 16;
    Serial.print(index, HEX); Serial.write('.');
    Serial.println(s + t + "|");
    s = "|"; t = "| |";
  }
  Serial.println(F("  +------------------------------------------------+ +----------------+"));
}

void setup() {
  Serial.begin(115200);
  Serial.flush();
  Serial.print("\n\nStarting WireSlave... ");
  bool success = WireSlave.begin(SDA, SCL, I2C_SLAVE_ADDR);
  if (!success) {
    Serial.println("failed! Aborting.");
    while (1) delay(100);
  }
  Serial.println("success!");
  WireSlave.onReceive(receiveEvent);
  Serial.println("Waiting for messages...");
}

void loop() {
  // the slave response time is directly related to how often
  // this update() method is called, so avoid using long delays inside loop()
  WireSlave.update();
  // let I2C and other ESP32 peripherals interrupts work
  delay(1);
}

// function that executes whenever a complete and valid packet
// is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (1 < WireSlave.available()) {
    // loop through all but the last byte
    char c = WireSlave.read(); // receive byte as a character
    Serial.print(c); // print the character
  }
  int x = WireSlave.read(); // receive byte as an integer
  Serial.println(x); // print the integer
}
