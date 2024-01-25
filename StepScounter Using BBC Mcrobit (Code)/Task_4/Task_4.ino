#include <LSM303AGR_ACC_Sensor.h>
#include <LSM303AGR_MAG_Sensor.h>
#include <Wire.h>
#include <Adafruit_Microbit.h>
#include <time.h>

#define DEV_I2C Wire1

LSM303AGR_ACC_Sensor Acc(&DEV_I2C);
Adafruit_Microbit_Matrix microbit;

const float stepThreshold = 2500;
int stepCount = 0;
bool aboveThreshold = false;

unsigned long lastStepTime = 0;
unsigned long lastDisplayTime = 0;
unsigned long startTime; // uses millis() to start counting time 
const int totalTime =    5 * 60 * 1000;  // Total time for the countdown (5 minutes)

bool timesUp = false; // Helps to stop count after time is over
bool stopCount = false; // Helps to return last count
bool reset = false; // Use to reset step count after the 5 minute interval is over

void setup() {
  Serial.begin(9600);
  DEV_I2C.begin();
  Acc.begin();
  Acc.Enable();
  microbit.begin();
  startTime = millis();
  pinMode(5, INPUT);
  Serial.println("Welcome to Step Counter!");
  Serial.println("Your 5-minute interval has started.");  // Record the start time for the countdown
}

void loop() {
  // Read accelerometer LSM303AGR.
  int32_t accelerometer[3];
  Acc.GetAxes(accelerometer);

  // Calculate acceleration magnitude.
  float accMagnitude = sqrt(accelerometer[0] * accelerometer[0] + accelerometer[1] * accelerometer[1] + accelerometer[2] * accelerometer[2]);

  // Check if acceleration is above the threshold
  if (accMagnitude > stepThreshold && !aboveThreshold) {
    aboveThreshold = true;
    if (!stopCount) {
      stepCount++;
      //delay(100);
      Serial.println("Step Count: " + String(stepCount));
    }
  } else if (accMagnitude < stepThreshold) {
    aboveThreshold = false;
  }
  
  // Check if it's time to update the display
  const int displayInterval = 10000;  // Set the interval for updating the display (milliseconds)
  unsigned long currentDisplayTime = millis();
  if (currentDisplayTime - lastDisplayTime >= displayInterval) {
    lastDisplayTime = currentDisplayTime;

    // Print the countdown timer
    unsigned long elapsedTime = millis() - startTime;
    int remainingTime = max(0, totalTime - elapsedTime);
    if (remainingTime > 0){
      int minutes = remainingTime / (60 * 1000);
      int seconds = (remainingTime / 1000) % 60;
      char timeString[10];
      sprintf(timeString, "%01d:%02d", minutes, seconds);
      microbit.print(timeString);
    }

    if (remainingTime <= 0 && !timesUp  ) {
      timesUp = true;
      microbit.print("Times uP!");
      Serial.println("Your step count in the 5 minute interval is: " + String(stepCount));
      stopCount = true;
      delay(1000);
      reset = true;
    }
  }
       
        if (!digitalRead(5)   && reset) {
          // Button was pressed, reset timer and step count
          stepCount = 0;
          aboveThreshold = false;
          lastStepTime = millis();
          lastDisplayTime = millis();
          startTime = millis();
          timesUp = false;
          stopCount = false;
          delay(1000);
          
          }
        
}

