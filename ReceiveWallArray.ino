#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


RF24 radio(7, 8); // CE, CSN

const byte address[6] = "CA100820";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    int data[4];
    radio.read(&data, sizeof(data));
    // Print received integers
    for (int i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
      Serial.print(data[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}
