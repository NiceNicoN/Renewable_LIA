#include <Stepper.h>
#include <DHT11.h>
//#include <AccelStepper.h>

const bool PRINT = false;

#define PIN_SPRINKLER_MOTOR1 9   // Connected to the Blue coloured wire
#define PIN_SPRINKLER_MOTOR3 10  // Connected to the Pink coloured wire
#define PIN_SPRINKLER_MOTOR2 11  // Connected to the Yellow coloured wire
#define PIN_SPRINKLER_MOTOR4 12  // Connected to the Orange coloured wire

#define PIN_FAN_RELAY 2
#define PIN_SPRINKLER_RELAY 3
#define PIN_HEATER_RELAY 4
#define PIN_LIGHT_RELAY 5
#define PIN_HUMIDITY_SENSOR 8
#define PIN_SPRINKLER_MODE_BUTTON 13
#define PIN_FAN_MODE_BUTTON A0

const int STEPS_PER_FULL_ROTATION = 2048;  // 28BYJ-48 has 2048 steps per rotation in full step mode as given in data sheet
const int STEPS_PER_NO_ROTATION = 0;
const int STEPS_SPEED = 19;

DHT11 dht11(PIN_HUMIDITY_SENSOR);
Stepper myStepper(STEPS_PER_FULL_ROTATION, PIN_SPRINKLER_MOTOR1, PIN_SPRINKLER_MOTOR2, PIN_SPRINKLER_MOTOR3, PIN_SPRINKLER_MOTOR4);
const int HumidityThreshold = 45;

int sprinklerModeCounter = 0;
int sprinklerModeState = 0;
int sprinklerModeLastState = 0;
bool sprinklerRunning = false;

int fanModeCounter = 0;
int fanModeState = 0;
int fanModeLastState = 0;

unsigned long lastTime = 0;
unsigned long lastTime_DHT = 0;

// ----- millis timing -----
unsigned long previousStepMillis = 0;
const unsigned long stepInterval = 3; // milliseconds between steps

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_SPRINKLER_MODE_BUTTON, INPUT);
  pinMode(PIN_FAN_MODE_BUTTON, INPUT);
  pinMode(PIN_SPRINKLER_RELAY, OUTPUT);
  pinMode(PIN_FAN_RELAY, OUTPUT);
  pinMode(PIN_HEATER_RELAY, OUTPUT);

  myStepper.setSpeed(STEPS_SPEED);
}

void loop() {
  // put your main code here, to run repeatedly:
   int temperature;
   int humidity;
   temperature=getTemperature();
   humidity=getHumidity();

  Serial.println(temperature);
   Serial.println(humidity);


 if (temperature > 26){
    Serial.println("Activate Fan");
    activateFan(0);
 }
 if (temperature <=26){
    activateFan(1);
 }
  if (temperature < 25){
    activateHeater(0);
 }
 else{
    activateHeater(1);
 }
 if (humidity < HumidityThreshold){
    //moveSprinkler(STEPS_PER_FULL_ROTATION);
    moveSprinkler(STEPS_PER_FULL_ROTATION);
 }
}

int checkSprinklerMode (){
  sprinklerModeState = digitalRead(PIN_SPRINKLER_MODE_BUTTON);
  //  Serial.print("Digital read: ");
  //   Serial.println(sprinklerModeState);
  if (sprinklerModeState==1){
    sprinklerModeCounter++;
  //  Serial.print("Spr Counter: ");
  //   Serial.println(sprinklerModeCounter);
    if (sprinklerModeCounter>3){
    sprinklerModeCounter = 0;
  }
  }
  return sprinklerModeCounter;
  //Mode 0 = Clockwise
  //Mode 1 = CW CCW
  //Mode 2 = /6 CW
  //Mode 3 = off

}
void activateHeater(bool state){
  digitalWrite(PIN_HEATER_RELAY, state);
}
void activateFan(bool state){
  digitalWrite(PIN_FAN_RELAY, state);
}
void moveSprinkler(int StepsRevolution){
  digitalWrite(PIN_SPRINKLER_RELAY,0);
  myStepper.step(StepsRevolution);
  digitalWrite(PIN_SPRINKLER_RELAY,1);
}
int getTemperature(){
    int temperature = dht11.readTemperature();

    // Check the result of the reading.
    // If there's no error, print the temperature value.
    // If there's an error, print the appropriate error message.
    if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {
        return temperature;
        /*Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");*/
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(temperature));
    }
}
int getHumidity(){
    int humidity = dht11.readHumidity();

    // Check the result of the reading.
    // If there's no error, print the humidity value.
    // If there's an error, print the appropriate error message.
    if (humidity != DHT11::ERROR_CHECKSUM && humidity != DHT11::ERROR_TIMEOUT) {
        return humidity;
        // Serial.print("Humidity: ");
        // Serial.print(humidity);
        // Serial.println(" %");
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(humidity));
    }

}