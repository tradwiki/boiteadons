//Code copied from user PaulStoffregen on the teensy forums at the following adress:
//https://forum.pjrc.com/threads/48308-test-code-for-piezo

const int thresholdMin = 200;  // minimum reading, avoid "noise"
const int aftershockMillis = 150; // time of aftershocks & vibration
const int velocityOffset = 10;

//dancing configuration
const int numMotors = 2;
const int motorPins[numMotors] = {11, 12};
const int numSteps = 24;
const int fastSustainSteps = 12;

int state = 0; // 0=idle, 1=looking for peak, 2=ignore aftershocks
int peak;    // remember the highest reading
elapsedMillis msec; // timer to end states 1 and 2

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2500) /* wait for serial monitor */ ;

  //write LOW to motors even if WITH_MOTORS is false
  //just to be sure they stay off if they are still plugged in
  if (numMotors > 0) {
    for (int motor = 0; motor < numMotors; motor++) {
      pinMode(motorPins[motor], OUTPUT);
      digitalWrite(motorPins[motor], LOW);
    }
  }

  Serial.println("Piezo Peak Capture");
}

void loop() {
  int piezo = analogRead(A0);

  if (state == 0) {
    // IDLE state: if any reading is above a threshold, begin peak
    if (piezo > thresholdMin) {
      state = 1;
      peak = piezo;
      msec = 0;
    }
  } else if (state == 1) {
    // Peak Tracking state: for 10 ms, capture largest reading
    if (piezo > peak) {
      peak = piezo;
    }
    if (msec >= velocityOffset) {
      Serial.print("peak = ");
      Serial.println(peak);
      state = 2;
      msec = 0;
    }
  } else {
    // Ignore Aftershock state: wait for things to be quiet again
    if (piezo > thresholdMin) {
      msec = 0; // keep resetting timer if above threshold
    } else if (msec > aftershockMillis) {

      //blocking call to main dance routine
      dance();

      state = 0; // go back to idle after 30 ms below threshold
    }
  }
}

void dance() {

  int longDelay = 400;
  int shortDelay = 0;

  int longPulse = 250;
  int shortPulse = 175;

  int increments = (longDelay - shortDelay) / numSteps;

  for (int delayDuration = longDelay; delayDuration >= shortDelay; delayDuration -= increments) {
    int pulseDuration = map(delayDuration, shortDelay, longDelay, shortPulse, longPulse);
    pulseMotor(0, pulseDuration);
    delay(delayDuration);
    pulseMotor(1, pulseDuration);
    delay(delayDuration);
  }
  for (int i = 0; i < fastSustainSteps; i++) {

    //right leg seems to prefer long pulses???
    pulseMotor(0, longPulse);
    delay(shortDelay);
    pulseMotor(1, shortPulse);
    delay(shortDelay);
  }

  delay(1500);

  digitalWrite(motorPins[0], HIGH);
  digitalWrite(motorPins[1], HIGH);
  delay(300);
  digitalWrite(motorPins[0], LOW);
  digitalWrite(motorPins[1], LOW);


}

void pulseMotor (int motor, int pulseDuration) {
  digitalWrite(motorPins[motor], HIGH);
  delay(pulseDuration);
  digitalWrite(motorPins[motor], LOW);

  if (motor == 0) {
    Serial.println("right");
  } else {
    Serial.println("left");
  }
}
