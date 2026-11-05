#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <NewPing.h>

#define FRONT_RIGHT_MOTOR 0
#define BACK_RIGHT_MOTOR 1
#define FRONT_LEFT_MOTOR 2
#define BACK_LEFT_MOTOR 3
#define STOP 0

#define FORWARD 1
#define BACKWARD -1

#define TRIG1_PIN 14
#define ECHO1_PIN 12
#define TRIG2_PIN 39
#define ECHO2_PIN 36
#define TRIG3_PIN 33
#define ECHO3_PIN 25
#define TRIG4_PIN 27
#define ECHO4_PIN 26

int trigPin[] = { 14, 39, 33, 27};
int echoPin[] = {12, 36, 25, 26};

NewPing predek_uss(TRIG1_PIN, ECHO1_PIN);
NewPing prava_uss(TRIG2_PIN, ECHO2_PIN);
NewPing zadek_uss(TRIG3_PIN, ECHO3_PIN);
NewPing leva_uss(TRIG4_PIN, ECHO4_PIN);

int cross = 0;
int triangle = 0;

struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;
};

std::vector<MOTOR_PINS> motorPins =
{
  {18,5},  //PREDNI PRAVY MOTOR
  {23,22},  //ZADNI PRAVY MOTOR
  {17,16},  //PREDNI LEVY MOTOR
  {21,19},  //ZADNI LEVY MOTOR
};

void rotateMotor(int motorNumber, int motorDirection)
{
  if (motorDirection == FORWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);
  }
  else if (motorDirection == BACKWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);
  }
  else
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);
  }
}


void setup() {
  for (int i = 0; i < motorPins.size(); i++)
  {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);
    rotateMotor(i, STOP);
  }
  for (int i = 0; i < 4; i++) {
      pinMode(trigPin[i], OUTPUT);
      pinMode(echoPin[i], INPUT);
    }
  Serial.begin(115200);
  Dabble.begin("BluetoothCar");

}
  void loop()
  {

    int predek = predek_uss.ping_cm();
    int prava = prava_uss.ping_cm();
    int zadek = zadek_uss.ping_cm();
    int leva = leva_uss.ping_cm();
    Dabble.processInput();
    if (GamePad.isTrianglePressed())
    {

      triangle = 1;
      cross = 0;
    }
    else if (GamePad.isCrossPressed())
    {
      cross = 1;
      triangle = 0;
    }
    else if (GamePad.isSelectPressed())
    {
      cross = 0;
      triangle = 0;
    }

    Serial.print(cross);
    Serial.print("---");
    Serial.println(triangle);

    Serial.print(predek);
    Serial.print("___");
    Serial.print(prava);
    Serial.print("___");
    Serial.print(zadek);
    Serial.print("___");
    Serial.println(leva);
    delay(50);
    if ((GamePad.isUpPressed()) && (predek > 15)) //dopredeu
    {
      Serial.println("UP");
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);

    }

    else if ((GamePad.isDownPressed())&&(zadek>15))//dozadu
    {
      Serial.println("DOWN");
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);
      
    }
        else if ((cross==1)&&(GamePad.isLeftPressed())&&(leva>15)&&(zadek>15))//leva
    {
      Serial.println("DOWN_LEFT");
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, STOP);


    }
    else if ((cross==1)&&(GamePad.isRightPressed())&&(prava>15)&&(zadek>15))//prava
    {
      Serial.println("DOWN_RIGHT");
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);


    }

        else if ((triangle==1)&&(GamePad.isLeftPressed())&&(leva>15)&&(predek>15))//leva
    {
      Serial.println("UP_LEFT");
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);


    }
    else if ((triangle==1)&&(GamePad.isRightPressed())&&(prava>15)&&(predek>15))//prava
    {
      Serial.println("UP_RIGHT");
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, STOP);


    }
    

    else if ((GamePad.isLeftPressed())&&(leva>15))//leva
    {
      Serial.println("LEFT");
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);

    }
    else if ((GamePad.isRightPressed())&&(prava>15))//prava
    {
      Serial.println("RIGHT");
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);

    }


    else if (GamePad.isSquarePressed())
    {
      Serial.println("R_LEFT");
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);

    }
    else if (GamePad.isCirclePressed())
    {
      Serial.println("R_RIGHT");
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);

    }




    else
    {
      Serial.println("STOP");
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);
    }

  }
