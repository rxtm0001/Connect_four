#include <AccelStepper.h>


const int stepsPerRevolution = 2048;
int pasos_a_realizar = 0;

int motorstepTarget = 0;
float CW = 0;

int pinAstateCurrent = LOW;
int pinAstateLast = pinAstateCurrent;

// Definir pines para el puente H
#define PIN_A1 6
#define PIN_A2 9
#define PIN_B1 10
#define PIN_B2 11

// Inicializar el objeto AccelStepper
AccelStepper stepper(4, PIN_A1, PIN_A2, PIN_B1, PIN_B2);

//Pin numbers for encoder input
#define encoderPinA 2
#define encoderPinB 3

//Encoder one turn steps;
float encoderSteps = 30;

//Last rotary value
int lastCount = 50;

//Variable for encoder counts
volatile long encoderPos = 0;
volatile long encoderCount = 0;
float previousPos = 0;
float motorPos = 0;
float vueltas;

//Mover motor
float mover_motor = 0;


//Functions

//Motor movement


//Interrupt
void updateEncoder() {
  pinAstateCurrent = digitalRead(encoderPinA);

  if (pinAstateCurrent != pinAstateLast) {
    if (digitalRead(encoderPinB) == pinAstateCurrent) {
      encoderPos--;
    } else {
      encoderPos++;
    }
  }

  pinAstateLast = pinAstateCurrent;
}



void setup() {
  Serial.begin(9600);

  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  attachInterrupt(0, updateEncoder, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, RISING);
    // Configurar la velocidad máxima y aceleración
  stepper.setMaxSpeed(500);
}

// ------------------------------------------------------------------
// MAIN LOOP     MAIN LOOP     MAIN LOOP     MAIN LOOP     MAIN LOOP
// ------------------------------------------------------------------
void loop() {

  encoderCount = encoderPos;
  motorstepTarget = map(encoderCount,0,encoderSteps,0,2048);
  stepper.setSpeed(300);
  stepper.moveTo(motorstepTarget);
  stepper.runSpeedToPosition();
  Serial.print(encoderPos); 
  Serial.print(", ");
  Serial.print(motorstepTarget);
  Serial.print("\n");
}






