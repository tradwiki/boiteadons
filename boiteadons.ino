//Code copied from user PaulStoffregen on the teensy forums at the following adress:
//https://forum.pjrc.com/threads/48308-test-code-for-piezo

const int numMotors = 2;

//digital pin numbers for each sensor
const int motorPins[numMotors] = {11, 12};

const int thresholdMin = 200;  // minimum reading, avoid "noise"
const int aftershockMillis = 150; // time of aftershocks & vibration
const int velocityOffset = 10;

const int pulseDuration = 200;
const int betweenLegs = 500;
const int numSteps = 20;

int state = 0; // 0=idle, 1=looking for peak, 2=ignore aftershocks
int peak;    // remember the highest reading
elapsedMillis msec; // timer to end states 1 and 2

void setup() {
  Serial.begin(115200);
  //pinMode(A0, INPUT_DISABLE);
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
  for (int i = 0; i < numSteps; i++) {
    pulseMotor(0);
    delay(betweenLegs);
    pulseMotor(1);
  }
}

void pulseMotor (int motor) {
  digitalWrite(motorPins[motor], HIGH);
  delay(pulseDuration);
  digitalWrite(motorPins[motor], LOW);
}
