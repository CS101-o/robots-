#include <MsTimer2.h>

int Motor_right_PWM = 10;  //   0 (min speed) - 255 (max speed) 
int Motor_right_direction = 5;  //   0 Forward - 1 Reverse
int Motor_left_PWM = 9;    //   0 (min speed) - 255 (max speed)  
int Motor_left_direction = 6;   //   0 Forward - 1 Reverse
#define Forward 1
#define Reverse 0

int Left_encoder = 2;
int Right_encoder = 3;
int Left_forward_speed=200;
int Right_forward_speed=200;
int Left_reverse_speed=200;
int Right_reverse_speed=200;

int direction = 0;
float right_dir;
float left_dir;

void forward(){
  analogWrite(Motor_right_PWM,Right_forward_speed); // right motor
  digitalWrite(Motor_right_direction,Forward); //right
  analogWrite(Motor_left_PWM,Left_forward_speed); // left 
  digitalWrite(Motor_left_direction,Forward); //left
}

void Stop(){ // set speeds to 0
  analogWrite(Motor_right_PWM,0); // right motor
  analogWrite(Motor_left_PWM,0); // left 
  //MsTimer2::start();
}

void right(){    
  analogWrite(Motor_right_PWM,Right_forward_speed);
  digitalWrite(Motor_right_direction,Forward);
  analogWrite(Motor_left_PWM,Left_reverse_speed); // left 
  digitalWrite(Motor_left_direction,Reverse); //left

  if (right_dir >= 90.0) {
    Stop();
    right_dir = 0;
    direction++;
  }
  if (direction >= 3) {
      direction = 0;
    }
    else {
      direction++;
  }
}

void left(){
  analogWrite(Motor_left_PWM,Left_forward_speed); // left 
  digitalWrite(Motor_left_direction,Forward); //left
  analogWrite(Motor_right_PWM,Right_reverse_speed);
  digitalWrite(Motor_right_direction,Reverse);

  if (left_dir >= 90.0) {
    Stop();
    left_dir = 0;
    direction--;
  }
  if (direction <= 0) {
      direction = 3;
    }
}

void reverse() {
  analogWrite(Motor_right_PWM,Right_forward_speed);
  digitalWrite(Motor_right_direction,Forward);
  analogWrite(Motor_left_PWM,Left_reverse_speed); // left 
  digitalWrite(Motor_left_direction,Reverse); //left

  if (right_dir >= 180.0) {
    Stop();
    right_dir = 0;
    direction = direction + 2;
  }

  if (direction = 4) {
    direction = 0;
  }
  else if (direction = 5) {
    direction = 1;
  }
}


long int Left_counter, Right_counter;

void Left_int_counter(){
  Left_counter++;
}

void Right_int_counter(){
  Right_counter++;
}

void Distance_readings() {
  float leftDistance = (2 * PI * 0.3 * Left_counter) / 250.0;
  float rightDistance = (2 * PI * 0.3 * Right_counter) / 250.0;
  float distance = (leftDistance + rightDistance)/2.0;


  Serial.print("Distance traveled: ");
  Serial.print(distance);
}

void Direction_calc() {                      
  left_dir = (Left_counter*360.0)/250.0;
  right_dir = (Right_counter*360.0)/250.0;
}

void Direction_readings() {
  /*if (right_dir > 90.0) { //turning right
    direction = direction + 1;
  }
  else if (left_dir > 90.0) { //turning left
    direction = direction - 1;
  }

  if (direction < 0) {
    direction = 3;
  }
  else if (direction > 3) {
    direction = 0;
  }*/
  
  Serial.print("Direction: ");
  Serial.print(direction);
}

void setup() {
 Serial.begin(9600);
  pinMode(Motor_left_PWM, OUTPUT);
  pinMode(Motor_right_PWM, OUTPUT);
  // set encoder counter to 0
  Left_counter=0;
  Right_counter=0;

  pinMode(Left_encoder, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Left_encoder), Left_int_counter, CHANGE);
  pinMode(Right_encoder, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Right_encoder), Right_int_counter, CHANGE);
  // setup Timer2 for motor control
  //MsTimer2::set(5000, forward);
  //MsTimer2::set(10000, left); // 400ms periods
  //MsTimer2::set(8000,left);
  //MsTimer2::start();
  //set PWM set-point
  //forward();

}

void loop() {
  Distance_readings();
  Direction_calc();
  Direction_readings();
  
  right();
  //reverse();
  
}