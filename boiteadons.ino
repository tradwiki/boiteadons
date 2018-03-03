//Code copied from user PaulStoffregen on the teensy forums at the following adress:
//https://forum.pjrc.com/threads/48308-test-code-for-piezo

const int thresholdMin = 120;  // minimum reading, avoid "noise"
const int aftershockMillis = 150; // time of aftershocks & vibration
const int velocityOffset = 10;

//dancing configuration
const bool withDance = true;
const int numMotors = 2;
const int motorPins[numMotors] = {11, 12};
const int crescendoSteps = 12;
const int midSteps = 16;
//intro and end
const int fastSustainSteps = 6;

//debug mode
const int rawOnly = false;

int state = 0; // 0=idle, 1=looking for peak, 2=ignore aftershocks
int peak;    // remember the highest reading
elapsedMillis msec; // timer to end states 1 and 2

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2500) /* wait for serial monitor */ ;

  //write LOW to motors
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
  if (rawOnly) {
    Serial.println(piezo);
  }
  else if (state == 0) {
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

      if (withDance) {
        //blocking call to main dance routine
        dance();
      }
      state = 0; // go back to idle after 30 ms below threshold
    }
  }
}

void dance() {

  int longDelay = 400;
  int shortDelay = 0;

  int longPulse = 250;
  int shortPulse = 175;


  //Fast
  for (int i = 0; i < fastSustainSteps; i++) {
    //right leg seems to prefer long pulses???
    pulseMotor(0, shortPulse);
    delay(shortDelay);
    pulseMotor(1, shortPulse);
    delay(shortDelay);
  }
  randomSeed(analogRead(A1));
  int type = random(0, 5);
  switch (type) {
    case 0:
      // Roomba mid
      Serial.println("ROOMBA");
      for (int i = 0; i < midSteps; i++) {
        if (i % 2 == 0) {
          pulseMotor(0, 200);
          delay(600);
          pulseMotor(1, 200);
          delay(200);
        } else {
          pulseMotor(1, 200);
          delay(200);
          pulseMotor(0, 200);
          delay(600);
        }
      }
      break;

    case 1:
      // 4/4
      Serial.println("4/4");
      for (int i = 0; i < midSteps; i++) {
        pulseMotor(0, 200);
        pulseMotor(1, 200);
        delay(400);
      }
      break;

    case 2:
      //Crescendo mid
      Serial.println("Crescendo");
      for (int delayDuration = longDelay; delayDuration >= shortDelay; delayDuration -= (longDelay - shortDelay) / midSteps) {
        int pulseDuration = map(delayDuration, shortDelay, longDelay, shortPulse, longPulse);
        pulseMotor(0, pulseDuration);
        delay(delayDuration);
        pulseMotor(1, pulseDuration);
        delay(delayDuration);
      }
      break;

    case 3:
      // 6/8
      Serial.println("6/8");
      for (int i = 0; i < midSteps; i++) {
        pulseMotor(0, 200);
        delay(200);
        pulseMotor(0, 200);
        delay(200);
        pulseMotor(1, 200);
        delay(200);
      }
      break;

    case 4:
      //Crescendo long
      Serial.println("long crescendo");
      int incrementsLong = (longDelay - shortDelay) / midSteps * 2;
      for (int delayDuration = longDelay; delayDuration >= shortDelay; delayDuration -= incrementsLong) {
        int pulseDuration = map(delayDuration, shortDelay, longDelay, shortPulse, longPulse);
        pulseMotor(0, pulseDuration);
        delay(delayDuration);
        pulseMotor(1, pulseDuration);
        delay(delayDuration);
      }
      break;
  }

  //Crescendo finish
  int increments = (longDelay - shortDelay) / crescendoSteps;
  for (int delayDuration = longDelay; delayDuration >= shortDelay; delayDuration -= increments) {
    int pulseDuration = map(delayDuration, shortDelay, longDelay, shortPulse, longPulse);
    pulseMotor(0, pulseDuration);
    delay(delayDuration);
    pulseMotor(1, pulseDuration);
    delay(delayDuration);
  }

  //Fast finish
  for (int i = 0; i < fastSustainSteps; i++) {
    //right leg seems to prefer long pulses???
    pulseMotor(0, shortPulse);
    delay(shortDelay);
    pulseMotor(1, shortPulse);
    delay(shortDelay);
  }

  //Finale
  delay(1500);
  digitalWrite(motorPins[0], HIGH);
  digitalWrite(motorPins[1], HIGH);
  delay(300);
  digitalWrite(motorPins[0], LOW);
  digitalWrite(motorPins[1], LOW);

  Serial.println("DONE");
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
