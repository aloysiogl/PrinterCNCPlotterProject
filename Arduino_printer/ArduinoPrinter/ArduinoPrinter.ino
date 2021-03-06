/*
   Made by Aloysio Galvão Lopes
*/

//Pin definitions
const int data = 6;
const int latch = 2;
const int clk = 3;
const int enX = 9;
const int enY = 11;
const int enZ = 10;
const int ledBlue = 4;
const int ledYellow = 5;
const int ledRed = 12;
const int baudRate = 115200;

struct StepperData {
  byte data1 = 0;
  byte data2 = 0;
};

//Stepper motor X steps definition for clockwise rotation
StepperData motorXSteps(byte id) {
  StepperData data;
  switch (id) {
    case 0:
      data.data1 = 0b00001000;
      data.data2 = 0b00000000;
      break;
    case 1:
      data.data1 = 0b01001000;
      data.data2 = 0b00000000;
      break;
    case 2:
      data.data1 = 0b01000000;
      data.data2 = 0b00000000;
      break;
    case 3:
      data.data1 = 0b01000100;
      data.data2 = 0b00000000;
      break;
    case 4:
      data.data1 = 0b00000100;
      data.data2 = 0b00000000;
      break;
    case 5:
      data.data1 = 0b00010100;
      data.data2 = 0b00000000;
      break;
    case 6:
      data.data1 = 0b00010000;
      data.data2 = 0b00000000;
      break;
    case 7:
      data.data1 = 0b00011000;
      data.data2 = 0b00000000;
      break;
  }
  return data;
}

//Stepper motor Y steps definition for clockwise rotation
StepperData motorYSteps(byte id) {
  StepperData data;
  switch (id) {
    case 0:
      data.data1 = 0b00000000;
      data.data2 = 0b00001000;
      break;
    case 1:
      data.data1 = 0b00000000;
      data.data2 = 0b00001001;
      break;
    case 2:
      data.data1 = 0b00000000;
      data.data2 = 0b00000001;
      break;
    case 3:
      data.data1 = 0b00000000;
      data.data2 = 0b00000101;
      break;
    case 4:
      data.data1 = 0b00000000;
      data.data2 = 0b00000100;
      break;
    case 5:
      data.data1 = 0b00000000;
      data.data2 = 0b00000110;
      break;
    case 6:
      data.data1 = 0b00000000;
      data.data2 = 0b00000010;
      break;
    case 7:
      data.data1 = 0b00000000;
      data.data2 = 0b00001010;
      break;
  }
  return data;
}

//Stepper motor Z steps definition for clockwise rotation
StepperData motorZSteps(byte id) {
  StepperData data;
  switch (id) {
    case 0:
      data.data1 = 0b00000000;
      data.data2 = 0b00100000;
      break;
    case 1:
      data.data1 = 0b00000010;
      data.data2 = 0b00100000;
      break;
    case 2:
      data.data1 = 0b00000010;
      data.data2 = 0b00000000;
      break;
    case 3:
      data.data1 = 0b00000010;
      data.data2 = 0b00010000;
      break;
    case 4:
      data.data1 = 0b00000000;
      data.data2 = 0b00010000;
      break;
    case 5:
      data.data1 = 0b00000001;
      data.data2 = 0b00010000;
      break;
    case 6:
      data.data1 = 0b00000001;
      data.data2 = 0b00000000;
      break;
    case 7:
      data.data1 = 0b00000001;
      data.data2 = 0b00100000;
      break;
  }
  return data;
}

//Global variables for the interruption loop
byte motorXCurrentStep = 0;
byte motorYCurrentStep = 0;
byte motorZCurrentStep = 0;
int motorXCommand = 0;
int motorYCommand = 0;
int motorZCommand = 0;
int motorXSpeedLoop = 0;
int motorYSpeedLoop = 0;
int motorZSpeedLoop = 0;
int motorXDelay = 0;
int motorYDelay = 0;
int motorZDelay = 0;
bool overSignalX = false;
bool overSignalY = false;
bool overSignalZ = false;

void setup() {
  //Initializing stepper pins
  pinMode(data, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(enX, OUTPUT);
  pinMode(enY, OUTPUT);
  pinMode(enZ, OUTPUT);

  //Setting up led pins
  pinMode(ledBlue, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);

  //Initializing serial
  Serial.begin(115200);
  Serial.setTimeout(10);

  //Blinking all leds (initialization)
  digitalWrite(ledRed, HIGH);
  delay(150);
  digitalWrite(ledRed, LOW);
  digitalWrite(ledYellow, HIGH);
  delay(150);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledBlue, HIGH);
  delay(150);
  digitalWrite(ledBlue, LOW);

  //Set timer0 interrupt at 2kHz
  cli();//stop interrupts

  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 60;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);


  sei();//allow interrupts

}

void loop() {
}

//Serial event handling
void serialEvent() {
  while (Serial.available() > 1) {
    String serialRead = Serial.readString();
    bool connectionAttempt = true;
    for (int i = 0; i < 6; ++i) {
      if ((char)serialRead[i] != -128) {
        connectionAttempt = false;
      }
    }
    if (serialRead.length() == 6) {
      if (connectionAttempt) {
        Serial.print((char)1);
        ledBlinkRoutine();
      }
      else {
        //Motor X
        motorXCommand = ((byte)serialRead[0])+(serialRead[1]%2)*256;
        if ((serialRead[1] >> 1) % 2){
          motorXCommand *= -1;
        }
        motorXDelay = (byte)(serialRead[1] >> 2);

        //Motor Y
        motorYCommand = ((byte)serialRead[2])+(serialRead[3]%2)*256;
        if ((serialRead[3] >> 1) % 2){
          motorYCommand *= -1;
        }
        motorYDelay = (byte)(serialRead[3] >> 2);

        //Motor Z
        motorZCommand = ((byte)serialRead[4])+(serialRead[5]%2)*256;
        if ((serialRead[5] >> 1) % 2){
          motorZCommand *= -1;
        }
        motorZDelay = (byte)(serialRead[5] >> 2);
      }
    }
  }
}

//Interrupt to move the steppers
ISR(TIMER1_COMPA_vect) {
  if (motorXCommand > 0) {
    digitalWrite(enX, HIGH);
    if (motorXSpeedLoop <= 0) {
      motorXCurrentStep = (motorXCurrentStep + 1) % 8;
      if (motorXCurrentStep % 2 == 0) {
        motorXCommand--;
        if (motorXCommand == 0) {
          overSignalX = true;
        }
      }
      motorXSpeedLoop = motorXDelay;
    }
    else {
      motorXSpeedLoop--;
    }
  }
  else if (motorXCommand < 0) {
    digitalWrite(enX, HIGH);
    if (motorXSpeedLoop <= 0) {
      motorXCurrentStep = (motorXCurrentStep - 1) % 8;
      if (motorXCurrentStep % 2 == 0) {
        motorXCommand++;
        if (motorXCommand == 0) {
          overSignalX = true;
        }
      }
      motorXSpeedLoop = motorXDelay;
    }
    else {
      motorXSpeedLoop--;
    }
  }
  else {
    digitalWrite(enX, LOW);
  }
  if (motorYCommand > 0) {
    digitalWrite(enY, HIGH);
    if (motorYSpeedLoop <= 0) {
      motorYCurrentStep = (motorYCurrentStep + 1) % 8;
      if (motorYCurrentStep % 2 == 0) {
        motorYCommand--;
        if (motorYCommand == 0) {
          overSignalY = true;
        }
      }
      motorYSpeedLoop = motorYDelay;
    }
    else {
      motorYSpeedLoop--;
    }
  }
  else if (motorYCommand < 0) {
    digitalWrite(enY, HIGH);
    if (motorYSpeedLoop <= 0) {
      motorYCurrentStep = (motorYCurrentStep - 1) % 8;
      if (motorYCurrentStep % 2 == 0) {
        motorYCommand++;
        if (motorYCommand == 0) {
          overSignalY = true;
        }
      }
      motorYSpeedLoop = motorYDelay;
    }
    else {
      motorYSpeedLoop--;
    }
  }
  else {
    digitalWrite(enY, LOW);
  }
  if (motorZCommand > 0) {
    digitalWrite(enZ, HIGH);
    if (motorZSpeedLoop <= 0) {
      motorZCurrentStep = (motorZCurrentStep + 1) % 8;
      if (motorZCurrentStep % 2 == 0) {
        motorZCommand--;
        if (motorZCommand == 0) {
          overSignalZ = true;
        }
      }
      motorZSpeedLoop = motorZDelay;
    }
    else {
      motorZSpeedLoop--;
    }
  }
  else if (motorZCommand < 0) {
    digitalWrite(enZ, HIGH);
    if (motorZSpeedLoop <= 0) {
      motorZCurrentStep = (motorZCurrentStep - 1) % 8;
      if (motorZCurrentStep % 2 == 0) {
        motorZCommand++;
        if (motorZCommand == 0) {
          overSignalZ = true;
        }
      }
      motorZSpeedLoop = motorZDelay;
    }
    else {
      motorZSpeedLoop--;
    }
  }
  else {
    digitalWrite(enZ, LOW);
  }
  StepperData toSend;
  StepperData dataX = motorXSteps(motorXCurrentStep);
  StepperData dataY = motorYSteps(motorYCurrentStep);
  StepperData dataZ = motorZSteps(motorZCurrentStep);
  toSend.data1 = dataX.data1 | dataY.data1 | dataZ.data1;
  toSend.data2 = dataX.data2 | dataY.data2 | dataZ.data2;
  writeData(toSend.data1, toSend.data2);
  if ((overSignalX && (overSignalY || motorYCommand == 0) && (overSignalZ || motorZCommand == 0)) || 
  (overSignalY && (overSignalX || motorXCommand == 0) && (overSignalZ || motorZCommand == 0)) ||
  (overSignalZ && (overSignalX || motorXCommand == 0) && (overSignalY || motorYCommand == 0))){
    Serial.print((char)5);
    overSignalX = false;
    overSignalY = false;
    overSignalZ = false;
  }
}

void writeData(byte data1, byte data2) {
  digitalWrite(latch, LOW);
  shiftOut(data, clk, LSBFIRST, data2);
  shiftOut(data, clk, LSBFIRST, data1);
  digitalWrite(latch, HIGH);
}

void ledBlinkRoutine() {
  digitalWrite(ledBlue, HIGH);
  delay(50);
  digitalWrite(ledBlue, LOW);
  digitalWrite(ledYellow, HIGH);
  delay(50);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledRed, HIGH);
  delay(50);
  digitalWrite(ledRed, LOW);
  delay(50);
  digitalWrite(ledYellow, HIGH);
  delay(50);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledBlue, HIGH);
  delay(50);
  digitalWrite(ledBlue, LOW);
}
