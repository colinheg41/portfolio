/* Author: Colin Hegarty
 * Date: 5/4/17
 * This program is used to control an RC cart designed to carry a load.
 * Ultrasonic sensors are used as a failsafe to avoid collision with an
 * obstacle or a person.
 */

#include <NewPing.h>

#define num_ultra 2      // Number of sensors.
#define max_distance 45 // Maximum distance (in cm) to ping.

//GLOBAL VARIABLES

// motor port numbers for the two motors
int motor1 = 10;
int motor1_dir = 13;
int motor2 = 3;
int motor2_dir = 2;

// input base values
int forward_low = 1984;
int forward_neutral = 1460;
int forward_high = 820;
int right_low = 1710;
int right_neutral = 1387;
int right_high = 1045;

// Number of total ultrasonic sensors used: 4
// pin numbers for each of the ultrasonic sensors
int ultrasonic_pins[2] = {46, 47};
int trigger_pin = 53;
// current data for each of the ultrasonic sensors
int ultrasonic_current[2] {1000, 1000};
int ultrasonic_previous[2] = {1000, 1000};
int ultrasonic_counter = 0;
bool ultrasonic_check = true;
int zeroes[2] = {0, 0};

// RC input pins
int RC1 = 35;
int RC2 = 34;
// RC input value for forward/backward
int RC_forward;
// RC input value for turning left/right
int RC_right;

bool reverse = true;

// put your setup code here, to run once:
void setup() {
  // Set motors to output and sensors/RC to input
  Serial.begin(9600);
  pinMode(motor1, OUTPUT);
  pinMode(motor1_dir, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(motor2_dir, OUTPUT);
  for (unsigned int i = 0; i < 2; ++i) {
    pinMode(ultrasonic_current[i], INPUT);
  }
  pinMode(RC1, INPUT);
  pinMode(RC2, INPUT);
}

// Function for reading ultrasonic sensor data up to a certain distance
NewPing sonar[num_ultra] = {   // Sensor object array.
  NewPing(trigger_pin, ultrasonic_pins[0], max_distance+30), // Each sensor's trigger pin, echo pin, and max distance to ping. 
  NewPing(trigger_pin, ultrasonic_pins[1], max_distance+30)
};

// Read ultrasonic sensors values
bool read_ultrasonic() {
  // Loop through the sensors
  for (unsigned int i = 0; i < num_ultra; ++i) {
    // Check if current reading is out of range (returns 0)
    if (ultrasonic_current[i] != 0) {
      // Only update previous non-zero value
      ultrasonic_previous[i] = ultrasonic_current[i];
      zeroes[i] = 0;
    }
    else {
      zeroes[i]++;
    }
    // Reset previous value to be out of range if obstacle has been out of range long enough
    if (zeroes[i] >= 10) {
      ultrasonic_previous[i] = 100;
    }
    // Read sensor
    ultrasonic_current[i] = sonar[i].ping_cm();
  }
  // Output sensor data
  for (unsigned int i = 0; i < num_ultra; ++i) {
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(ultrasonic_current[i]);
    Serial.print("\t");
  }
  for (unsigned int i = 0; i < num_ultra; ++i) {
    Serial.print("Previous ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(ultrasonic_previous[i]);
    Serial.print("\t");
  }
  Serial.print("\n");

  // Determine whether obstacle is front of object or not
  for (unsigned int i = 0; i < num_ultra; ++i) {
    if ((ultrasonic_current[i] < max_distance && ultrasonic_current[i] != 0 && ultrasonic_current[i] != 5) || (ultrasonic_current[i] == 0 && ultrasonic_previous[i] < max_distance && ultrasonic_previous[i] != 5)) {
      return false;
    }
  }
  return true;
}

// Read RC values and send correct output to motors
void set_motors(bool stopper) {
  // Read RC inputs
  RC_forward = pulseIn(RC1, HIGH);
  RC_right = pulseIn(RC2, HIGH);

  // Print RC data
  /*Serial.print("forward: ");
  Serial.print(RC_forward);
  Serial.print("\t\tright: ");
  Serial.print(RC_right);
  Serial.print("\n");*/

  int forward, right;

  // Map raw RC values to useful motor values
  if (forward < forward_neutral) {
    forward = map(RC_forward, forward_neutral, forward_high, 0, 255);
  }
  else {
    forward = map(RC_forward, forward_low, forward_neutral, -255, 0);
  }
  if (right < right_neutral) {
    right = map(RC_right, right_neutral, right_high, 0, 255);
  }
  else {
    right = map(RC_right, right_low, right_neutral, -255, 0);
  }

  // Dead-zone in case an input is slightly off-center
  if (abs(forward) < 20) {
    forward = 0;
  }
  if (abs(right) < 20) {
    right = 0;
  }
  
  // Value sent to the motors
  int power1 = -forward-right;
  int power2 = -forward+right;

  // Stop each motor if it is supposed to move forward and there is an obstacle
  if (!stopper && power1 < 0) {
    power1 = 0;
  }
  if (!stopper && power2 < 0) {
    power2 = 0;
  }
  // Dead-zone
  if (abs(power1) < 15) {
    power1 = 0;
  }
  // Maximum of 255
  else if (abs(power1) > 255) {
    if (power1 > 0) {
      power1 = 255;
    }
    else {
      power1 = -255;
    }
  }
  // Same for motor 2 as motor 1
  if (abs(power2) < 15) {
    power2 = 0;
  }
  else if (abs(power2) > 255) {
    if (power2 > 0) {
      power2 = 255;
    }
    else {
      power2 = -255;
    }
  }

  // Set motors to 0 if RC input times out and gives 0
  if (RC_forward == 0 || RC_right == 0) {
    power1 = 0;
    power2 = 0;
  }
  
  // Whether the motors will be moving in forward or reverse
  if(power1 >= 0) {
    analogWrite(motor1, power1);
    digitalWrite(motor1_dir, LOW);
  }
  else {
    analogWrite(motor1, -power1);
    digitalWrite(motor1_dir, HIGH);
  }
  if(power2 >= 0) {
    analogWrite(motor2, power2);
    digitalWrite(motor2_dir, HIGH);
  }
  else {
    analogWrite(motor2, -power2);
    digitalWrite(motor2_dir, LOW);
  }
}

// put your main code here, to run repeatedly:
void loop() {
  // Check sensors every 5 loops
  if (ultrasonic_counter == 0) {
    ultrasonic_check = read_ultrasonic();
    ultrasonic_counter = 5;
  }
  else {
    ultrasonic_counter--;
  }
  // Set motor values based on sensor data
  set_motors(ultrasonic_check);
  delay(10);
}
