// Basic test of Mona robot including proximity sensors and open-loop motion control
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "CA100820";
// pin config for basic platform test

int IR_enable=4;
int IR_threshold= 990; // 0 white close obstacle -- 1023 no obstacle


void setup() {
// initialize serial communication at 9600 bits per second:
 Serial.begin(9600);
// initialize Ports
  pinMode(IR_enable, OUTPUT);
  radio.begin();
  radio.openWritingPipe(address);
  radio.openReadingPipe(0,address);
  radio.setPALevel(RF24_PA_MIN);
}

// Variables for 5 IR proximity sensors 
int IR_right,IR_front,IR_left;

void IR_proximity_read(){    // read IR sensors 
  int n=5;  // average parameter
  digitalWrite(IR_enable, HIGH);  //IR Enable
  IR_right=0;
  IR_front=0;
  IR_left=0;
  for (int i=0;i<n;i++){
    IR_left+=analogRead(A3);
    IR_front+=analogRead(A1);
    IR_right+=analogRead(A7);
    delay(5);
  }
  IR_right/=n;
  IR_front/=n;
  IR_left/=n; 
}

void Obstacle_avoidance(int orientation) {
  bool front_obstacle = IR_front < IR_threshold;
  bool right_obstacle = IR_right < IR_threshold; 
  bool left_obstacle = IR_left < IR_threshold; 

  // Reset walls array
  int walls[4] = {0};

  // Calculate index for the walls array based on orientation
  int wall_index = (orientation + 2) % 4;

  // Set appropriate wall based on obstacle and orientation
  if (front_obstacle && right_obstacle && left_obstacle) {
    Serial.println("LEFT AND RIGHT AND FRONT WALLS");
    walls[wall_index] = 1;
    walls[(wall_index + 1) % 4] = 1;
    walls[(wall_index + 3) % 4] = 1;
  } 
  else if (front_obstacle && right_obstacle) {
    Serial.println("RIGHT AND FRONT WALLS");
    walls[wall_index] = 1;
    walls[(wall_index + 1) % 4] = 1;
  }
  else if (front_obstacle && left_obstacle) {
    Serial.println("LEFT AND FRONT WALLS");
    walls[wall_index] = 1;
    walls[(wall_index + 3) % 4] = 1;
  } 
  else if (right_obstacle && left_obstacle) {
    Serial.println("RIGHT AND LEFT WALL");
    walls[(wall_index + 1) % 4] = 1;
    walls[(wall_index + 3) % 4] = 1;
  } 
  else if (right_obstacle) {
    Serial.println("RIGHT WALL");
    walls[(wall_index + 1) % 4] = 1;
  } 
  else if (left_obstacle) {
    Serial.println("LEFT WALL");
    walls[(wall_index + 3) % 4] = 1;
  } 
  else if (front_obstacle) {
    Serial.println("FRONT WALL");
    walls[wall_index] = 1;
  } 
  else {
    Serial.println("NO WALLS");
  }

  // Send walls array over radio
  radio.write(walls, sizeof(walls));
}


void Receive_Coordinates(){
  if(radio.available()) {
    int coord[4];
    radio.read(&coord, sizeof(coord));
    for(int i=0; i<sizeof(coord) / sizeof(coord[0]); i++){
      Serial.print(coord[i]);
      Serial.print(" ");
    }
  }
  Serial.println();
}
// send IR readings to Serial Monitor

void Send_sensor_readings(){
 Serial.print(IR_left);
 Serial.print(',');
 Serial.print(IR_front);
 Serial.print(',');
 Serial.println(IR_right);  
}


// the loop routine runs over and over again forever:
void loop() {
 int orientation = 0;
 IR_proximity_read();
 //Send_sensor_readings(); 
 radio.stopListening();
 Obstacle_avoidance(orientation);
 radio.startListening();
 Receive_Coordinates();
 delay(100);        // delay in between reads for stability
}
