#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "CA100820";
// pin config for basic platform test

int IR_enable=4;
int IR_threshold= 990;
int Motor_right_PWM = 10;  //   0 (min speed) - 255 (max speed) 
int Motor_right_direction = 5;  //   0 Forward - 1 Reverse
int Motor_left_PWM = 9;    //   0 (min speed) - 255 (max speed)  
int Motor_left_direction = 6;   //   0 Forward - 1 Reverse
#define Forward 1
#define Reverse 0

int Left_forward_speed=0;
int Right_forward_speed=0;
int Left_reverse_speed=255;
int Right_reverse_speed=255;

int direction = 0;
float right_dir = 0;
float left_dir = 0;
float distance;

/*void Distance_readings() {
  float leftDistance = (2 * PI * 0.15 * Left_counter) / 250.0;
  float rightDistance = (2 * PI * 0.15 * Right_counter) / 250.0;
  distance = (leftDistance + rightDistance)/2.0;


  Serial.print("Distance traveled: \n");
  Serial.print(distance);
}*/
void forward() {
  analogWrite(Motor_right_PWM,Right_forward_speed); // right motor
  digitalWrite(Motor_right_direction,Forward); //right
  analogWrite(Motor_left_PWM,Left_forward_speed); // left 
  digitalWrite(Motor_left_direction,Forward); //left
  delay(900);
 
  

}

void Stop(){ // set speeds to 0
  analogWrite(Motor_right_PWM,255); // right motor
  digitalWrite(Motor_right_direction,Forward);
  analogWrite(Motor_left_PWM,255); // left 
  digitalWrite(Motor_left_direction,Forward); //left
  //MsTimer2::start();
}

void left(){  
  Stop();
  delay(100);

  direction--;
  if (direction < 0) {
    direction = 3;
  }

  analogWrite(Motor_right_PWM,Right_forward_speed);
  digitalWrite(Motor_right_direction,Forward);
  analogWrite(Motor_left_PWM,Left_reverse_speed); // left 
  digitalWrite(Motor_left_direction,Reverse); //left
  delay(480);

}

void right(){
  Stop();
  delay(100);

  direction++;
  if (direction > 3){
    direction = 0;
  }

  analogWrite(Motor_left_PWM,Left_forward_speed); // left 
  digitalWrite(Motor_left_direction,Forward); //left
  analogWrite(Motor_right_PWM,Right_reverse_speed);
  digitalWrite(Motor_right_direction,Reverse);
  delay(480);

}

void reverse() {
  Stop();
  delay(100);

  direction = direction + 2;
  if (direction == 4) {
    direction = 0;
  }
  else if (direction == 5) {
    direction = 1;
  }

  analogWrite(Motor_left_PWM,Right_forward_speed);
  digitalWrite(Motor_left_direction,Forward);
  analogWrite(Motor_right_PWM,Left_reverse_speed); // left 
  digitalWrite(Motor_right_direction,Reverse); //left
  delay(960);
}

void Direction_readings() { 
  Serial.print("Direction: \n");
  Serial.print(direction);
}

void setup() {
// initialize serial communication at 9600 bits per second:
 Serial.begin(9600);
// initialize Ports
  pinMode(IR_enable, OUTPUT);
  radio.begin();
  radio.openWritingPipe(address);
  radio.openReadingPipe(0,address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  pinMode(Motor_left_PWM, OUTPUT);
  pinMode(Motor_right_PWM, OUTPUT);
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

void Obstacle_avoidance(int direction) {
  bool front_obstacle = IR_front < IR_threshold;
  bool right_obstacle = IR_right < IR_threshold; 
  bool left_obstacle = IR_left < IR_threshold; 

  // Reset walls array
  int walls[4] = {0};

  // Calculate index for the walls array based on orientation
  int wall_index = direction;

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

void MoveToCoordinates(int current_direction, int current_x, int current_y, int desired_x, int desired_y) {
  // Calculate the change in coordinates
  int dx = desired_x - current_x;
  int dy = desired_y - current_y;
    
  // Determine the movement action based on the current orientation
  if (current_direction == 0) { // Facing positive y-axis
    if (dx > 0) {  
        Serial.println("RIGHT + FORWARD");
        right();
    }
    else if(dx < 0){  
        Serial.println("LEFT + FORWARD");
        left();
      }
     else if (dy > 0) {  
        Serial.println("180 + FORWARD");
        reverse();
    }
    else if(dy < 0){  
        Serial.println("FORWARD");
        forward();
      }
  }
  else if (current_direction == 1) { // Facing positive y-axis
    if (dx > 0) {  
        Serial.println("FORWARD");
        forward();
    }
    else if(dx < 0){  
        Serial.println("180 + FORWARD");
        reverse();
      }
     else if (dy > 0) {  
        Serial.println("RIGHT + FORWARD");
        right();
    }
    else if(dy < 0){  
        Serial.println("LEFT + FORWARD");
        left();
      }
  }
  else if (current_direction == 2) { // Facing positive y-axis
    if (dx > 0) {  
        Serial.println("LEFT + FORWARD");
        left();
    }
    else if(dx < 0){  
        Serial.println("RIGHT + FORWARD");
        right();
      }
     else if (dy > 0) {  
        Serial.println("FORWARD");
        forward();
    }
    else if(dy < 0){  
        Serial.println("180 + FORWARD");
        reverse();
      }
  }
  else if (current_direction == 3) { // Facing positive y-axis
    if (dx > 0) {  
        Serial.println("180 + FORWARD");
        reverse();
    }
    else if(dx < 0){  
        Serial.println("FORWARD");
        forward();
      }
     else if (dy > 0) {  
        Serial.println("LEFT + FORWARD");
        left();
    }
    else if(dy < 0){  
        Serial.println("RIGHT + FORWARD");
        right();
      }
  }
}

// the loop routine runs over and over again forever:
void loop() {
 int direction = 0;
 IR_proximity_read();
 //Send_sensor_readings(); 
 radio.stopListening();
 Obstacle_avoidance(direction);
 radio.startListening();
 Receive_Coordinates();
 delay(100);        // delay in between reads for stability

 // Example usage of MoveToCoordinates
  int current_direction = 3; // Assuming the robot is initially facing positive y-axis
  if (radio.available()) {
      int coord[4];
      radio.read(&coord, sizeof(coord));
      int current_x = coord[0]; // Current x-coordinate
      int current_y = coord[1]; // Current y-coordinate
      int desired_x = coord[2]; // Desired x-coordinate
      int desired_y = coord[3]; // Desired y-coordinate

  // Call MoveToCoordinates function
  MoveToCoordinates(current_direction, current_x, current_y, desired_x, desired_y);
}
}