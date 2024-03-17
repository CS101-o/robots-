#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN

const byte address[6] = "CA100820";

void setup() {
  // Other setup code
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void MoveToCoordinates(int current_orientation, int current_x, int current_y, int desired_x, int desired_y) {
  // Calculate the change in coordinates
  int dx = desired_x - current_x;
  int dy = desired_y - current_y;
    
  // Determine the movement action based on the current orientation
  if (current_orientation == 0) { // Facing positive y-axis
    if (dx > 0) {  
        Serial.println("RIGHT + FORWARD");
    }
    else if(dx < 0){  
        Serial.println("LEFT + FORWARD");
      }
     else if (dy > 0) {  
        Serial.println("180 + FORWARD");
    }
    else if(dy < 0){  
        Serial.println("FORWARD");
      }
  }
  else if (current_orientation == 1) { // Facing positive y-axis
    if (dx > 0) {  
        Serial.println("FORWARD");
    }
    else if(dx < 0){  
        Serial.println("180 + FORWARD");
      }
     else if (dy > 0) {  
        Serial.println("RIGHT + FORWARD");
    }
    else if(dy < 0){  
        Serial.println("LEFT + FORWARD");
      }
  }
  else if (current_orientation == 2) { // Facing positive y-axis
    if (dx > 0) {  
        Serial.println("LEFT + FORWARD");
    }
    else if(dx < 0){  
        Serial.println("RIGHT + FORWARD");
      }
     else if (dy > 0) {  
        Serial.println("FORWARD");
    }
    else if(dy < 0){  
        Serial.println("180 + FORWARD");
      }
  }
  else if (current_orientation == 3) { // Facing positive y-axis
    if (dx > 0) {  
        Serial.println("180 + FORWARD");
    }
    else if(dx < 0){  
        Serial.println("FORWARD");
      }
     else if (dy > 0) {  
        Serial.println("LEFT + FORWARD");
    }
    else if(dy < 0){  
        Serial.println("RIGHT + FORWARD");
      }
  }
}


void loop() {
  // Example usage of MoveToCoordinates
  int current_orientation = 3; // Assuming the robot is initially facing positive y-axis
  if (radio.available()) {
      int coord[4];
      radio.read(&coord, sizeof(coord));
      int current_x = coord[0]; // Current x-coordinate
      int current_y = coord[1]; // Current y-coordinate
      int desired_x = coord[2]; // Desired x-coordinate
      int desired_y = coord[3]; // Desired y-coordinate

  // Call MoveToCoordinates function
  MoveToCoordinates(current_orientation, current_x, current_y, desired_x, desired_y);
}
}
