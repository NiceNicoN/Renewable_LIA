#include <Stepper.h>
#include <DHT11.h>

#define PIN_SPRINKLER_MOTOR1 9
#define PIN_SPRINKLER_MOTOR3 10
#define PIN_SPRINKLER_MOTOR2 11
#define PIN_SPRINKLER_MOTOR4 12

#define PIN_HUMIDITY_SENSOR 8
#define PIN_SPRINKLER_MODE_BUTTON 13

const int STEPS_PER_FULL_ROTATION = 2048;

Stepper myStepper(STEPS_PER_FULL_ROTATION,
                  PIN_SPRINKLER_MOTOR1,
                  PIN_SPRINKLER_MOTOR2,
                  PIN_SPRINKLER_MOTOR3,
                  PIN_SPRINKLER_MOTOR4);

DHT11 dht11(PIN_HUMIDITY_SENSOR);

const int HumidityThreshold = 40;

// ----- millis timing -----
unsigned long previousStepMillis = 0;
const unsigned long stepInterval = 3; // milliseconds between steps

// motor state
bool sprinklerRunning = false;
int currentSteps = 0;
int targetSteps = STEPS_PER_FULL_ROTATION;

// button variables
int sprinklerModeCounter = 0;
int sprinklerModeState = 0;
int sprinklerModeLastState = 0;

void setup() {

  Serial.begin(9600);

  pinMode(PIN_SPRINKLER_MODE_BUTTON, INPUT);

  myStepper.setSpeed(15);
}

void loop() {

  // ALWAYS RUNNING
  checkSprinklerMode();

  int humidity = getHumidity();

  Serial.print("Humidity: ");
  Serial.println(humidity);

  // Start sprinkler if humidity low
  if (humidity < HumidityThreshold && !sprinklerRunning) {

    sprinklerRunning = true;
    currentSteps = 0;

    Serial.println("Sprinkler Started");
  }

  // NON-BLOCKING STEPPER CONTROL
  runStepperNonBlocking();
}

void runStepperNonBlocking() {

  if (!sprinklerRunning) return;

  unsigned long currentMillis = millis();

  // Time for next step?
  if (currentMillis - previousStepMillis >= stepInterval) {

    previousStepMillis = currentMillis;

    myStepper.step(1);
  
    currentSteps++;

    // Finished rotation?
    if (currentSteps >= targetSteps) {

      sprinklerRunning = false;

      Serial.println("Rotation Complete");
    }
  }
}

void checkSprinklerMode() {

  sprinklerModeState = digitalRead(PIN_SPRINKLER_MODE_BUTTON);

  // Detect button press edge
  if (sprinklerModeState == HIGH &&
      sprinklerModeLastState == LOW) {

    sprinklerModeCounter++;

    if (sprinklerModeCounter > 3) {
      sprinklerModeCounter = 0;
    }

    Serial.print("Mode: ");
    Serial.println(sprinklerModeCounter);
  }

  sprinklerModeLastState = sprinklerModeState;
}

int getHumidity() {

  int humidity = dht11.readHumidity();

  if (humidity != DHT11::ERROR_CHECKSUM &&
      humidity != DHT11::ERROR_TIMEOUT) {

    return humidity;
  }

  return 0;
}