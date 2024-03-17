#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "CA100820";

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  // Define an integer array
  int data[4] = {1,2,3,4};

  // Send the integer array
  radio.write(&data, sizeof(data));

  delay(100);
}
