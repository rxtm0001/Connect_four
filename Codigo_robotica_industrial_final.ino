bool homing = 1;
bool laboratorio = 0;
bool pruebaXYZ = 0;
bool pruebabailecito = 1;
bool pruebaGrab = 0;
bool pruebaGripper = 0;

#include <AccelStepper.h>
#include <MultiStepper.h>
#include <math.h>
#include <Servo.h>

#define switchBaseR 26
#define switchBaseL 27
#define switchHombro 28
#define switchCodo 29
#define miliGripperPin 10
#define megaGripperPin 9
#define ledPrueba 30

//Relation
#define RELATION_BASE 22.222222222
#define RELATION_HOMBRO 680.00001632
#define RELATION_CODO 13.33333333333

double CapturaX = 0.00;
double CapturaY = 0.00;
double CapturaZ = 0.00;
double CapturaXYZ = 0;

double posXvalue = -100;
double posYvalue = -100;
double posZvalue = -100;



//Communication
String msg = "STATIC";
String data = "";

//States
int agarraFicha = 0;
int pos1Ficha = 1;

// Stepper Homing Base
int homingBase = 1;  // Used to check if move is completed

// Stepper Homing Hombro
int homingHombro = 1;     // Used to check if move is completed
long inihomeHombro = -1;  // Used to Home Stepper at startup

// Stepper Homing Codo
int homingCodo = 1;  // Used to check if move is completed
long inihomeCodo = -1;

//Calculations
double angles[3];
//Array of desired stepper positions
long positions[3];

//Estados del programa
int laboratorioDone = -1;
int labValue = -1;


// EG X-Y position bed driven by 2 steppers
// Alas its not possible to build an array of these with different pins for each :-(

//Attach servo
Servo megaGripper;
Servo miliGripper;
int megagripperPos = 0;
int miligripperPos = 0;
int Reset = -1;
int resetValue = -1;

//PIN 1 PUL PIN 2 DIR
AccelStepper stepBase(1, 2, 3);    //Base
AccelStepper stepHombro(1, 4, 5);  //Hombro
AccelStepper stepCodo(1, 8, 7);    //Codo

// Up to 10 steppers can be handled as a group by MultiStepper
MultiStepper steppers;

//Stepper homing

void reset() {

  CapturaXYZ = 0;
  CapturaX = 0;
  CapturaY = 0;
  CapturaZ = 0;

  posXvalue = -100;
  posYvalue = -100;
  posZvalue = -100;
}
void configureSteppers() {

  // Configure each stepper
  stepBase.setMaxSpeed(400);

  stepHombro.setMaxSpeed(50000);

  stepCodo.setMaxSpeed(700);
}

void configureSteppersSlow (){
    // Configure each stepper
  stepBase.setMaxSpeed(200);

  stepHombro.setMaxSpeed(25000);

  stepCodo.setMaxSpeed(300);

}

//HomeBaseL
long homeBaseL() {

  stepBase.setMaxSpeed(250);
  stepBase.setAcceleration(17000);

  Serial.print("Stepper Base is Homing...");
  stepBase.setCurrentPosition(0);
  long inihomeBase = 1;  // Used to Home Stepper at startup


  while (digitalRead(switchBaseL)) {  // Make the Stepper move CCW until the switch is activated
    stepBase.moveTo(inihomeBase);     // Set the position to move to
    inihomeBase++;                    // Decrease by 1 for next move if needed
    stepBase.run();                   // Start moving the stepper
  }

  stepBase.setCurrentPosition(0);  // Set the current position as zero for now
  stepBase.setMaxSpeed(25);
  stepBase.setAcceleration(17000);
  inihomeBase = 1;

  while (!digitalRead(switchBaseL)) {  // Make the Stepper move CW until the switch is deactivated
    stepBase.moveTo(inihomeBase);
    stepBase.run();
    inihomeBase--;
  }

  long anglebaseHome = 52;
  stepBase.setMaxSpeed(500);
  stepBase.setAcceleration(17000);
  stepBase.setCurrentPosition(0);
  stepBase.moveTo(-RELATION_BASE * anglebaseHome);
  stepBase.runToPosition();
  stepBase.setCurrentPosition(0 * RELATION_BASE);
  Serial.println("Base Homing Completed");
  Serial.println("");
}
//HomeHombro
long homeHombro() {

  stepHombro.setMaxSpeed(50000);
  stepHombro.setAcceleration(17000);
  long inihomeHombro = 1;  // Used to Home Stepper at startup
  Serial.print("Stepper Hombro is Homing...");
  stepHombro.setCurrentPosition(0);


  while (digitalRead(switchHombro)) {     // Make the Stepper move CCW until the switch is activated
    stepHombro.moveTo(inihomeHombro);     // Set the position to move to
    inihomeHombro = inihomeHombro + 100;  // Decrease by 1 for next move if needed
    stepHombro.run();                     // Start moving the stepper
  }

  stepHombro.setCurrentPosition(0);     // Set the current position as zero for now
  stepHombro.setMaxSpeed(5000);         // Set Max Speed of Stepper (Slower to get better accuracy)
  stepHombro.setAcceleration(17000.0);  // Set Acceleration of Stepper
  inihomeHombro = 1;

  while (!digitalRead(switchHombro)) {  // Make the Stepper move CW until the switch is deactivated
    stepHombro.moveTo(inihomeHombro);
    stepHombro.run();
    inihomeHombro = inihomeHombro - 25;
  }

  long anglehombroHome = 84;
  stepHombro.setMaxSpeed(50000);
  stepHombro.setAcceleration(17000);
  stepHombro.setCurrentPosition(0);
  stepHombro.moveTo(-RELATION_HOMBRO * anglehombroHome);
  stepHombro.runToPosition();
  stepHombro.setCurrentPosition(90 * RELATION_HOMBRO);
  Serial.println("Hombro Homing Completed");
  Serial.println("");
}
//Home Codo
long homeCodo() {

  stepCodo.setMaxSpeed(300);
  stepCodo.setAcceleration(17000);
  stepCodo.setMinPulseWidth(1000);

  Serial.print("Stepper Codo is Homing...");
  stepCodo.setCurrentPosition(0);
  long inihomeCodo = 1;  // Used to Home Stepper at startup


  while (digitalRead(switchCodo)) {  // Make the Stepper move CCW until the switch is activated
    stepCodo.moveTo(inihomeCodo);    // Set the position to move to
    inihomeCodo--;                   // Decrease by 1 for next move if needed
    stepCodo.run();                  // Start moving the stepper
  }

  stepCodo.setCurrentPosition(0);  // Set the current position as zero for now
  stepCodo.setMaxSpeed(50);
  stepCodo.setAcceleration(17000);
  stepCodo.setMinPulseWidth(1000);
  inihomeCodo = 1;

  while (!digitalRead(switchCodo)) {  // Make the Stepper move CW until the switch is deactivated
    stepCodo.moveTo(inihomeCodo);
    stepCodo.run();
    inihomeCodo++;
  }

  long anglehomeCodo = 108;
  stepCodo.setMaxSpeed(500);
  stepCodo.setAcceleration(17000);
  stepCodo.setMinPulseWidth(1000);
  stepCodo.setCurrentPosition(0);
  stepCodo.moveTo(RELATION_CODO * anglehomeCodo);
  stepCodo.runToPosition();
  stepCodo.setCurrentPosition(0 * RELATION_CODO);
  Serial.println("Codo Homing Completed");
  Serial.println("");
}
long moveStepperstoAngle(long cmBase, long cmHombro, long cmCodo) {
  moveBase(cmBase);
  moveHombro(cmHombro);
  moveCodo(cmCodo);
  steppers.moveTo(positions);
  steppers.runSpeedToPosition();
}
long moveBase(long cmMove) {

  //Relation Base
  positions[0] = cmMove * RELATION_BASE;
}
long moveHombro(long cmMove) {
  //Relation Hombro
  positions[1] = cmMove * RELATION_HOMBRO;
}

long moveCodo(long cmMove) {

  //Relation Codo
  long relationCodo = 0.6;
  positions[2] = cmMove * RELATION_CODO;
}
void homeSteppers() {
  closeGripper();
  homeBaseL();
  homeCodo();
  homeHombro();
}
int closeGripper() {
  miliGripper.write(180);
  return 1;
}
int openGripper() {
  miliGripper.write(30);
  return 1;
}
//Función para calcular los ángulos
long calculateAngles(double x3_0, double y3_0, double z3_0, double angles[3]) {

  double a1 = 9.8;
  double a2 = 15;
  double a3 = 12.886;

  double the1 = atan2(-y3_0, x3_0);
  double r1 = -1 * (sqrt((x3_0 * x3_0) + (y3_0 * y3_0)));
  double r2 = z3_0 - a1;
  double phy2 = atan2(r2, r1);
  double r3 = sqrt(r1 * r1 + r2 * r2);
  double phy1 = acos(((a3 * a3) - (a2 * a2) - (r3 * r3)) / (-2 * a2 * r3));
  double the2 = phy2 - phy1;
  double phy3 = acos(((r3 * r3) - (a2 * a2) - (a3 * a3)) / (-2 * a2 * a3));
  double the3 = PI - phy3;
  //Ángulos de the1 deben ser de -180 a 45, si no no está en el área de trabajo.
  angles[0] = (the1 * (180 / PI));    //Codo
  angles[1] = ((the2) * (180 / PI));  //Hombro
  angles[2] = the3 * (180 / PI);      //Base
}
long moveXYZ(double X, double Y, double Z) {


  calculateAngles(X, Y, Z, angles);
  configureSteppers();
  positions[0] = angles[0] * RELATION_BASE;
  positions[1] = angles[1] * RELATION_HOMBRO;
  positions[2] = angles[2] * RELATION_CODO;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition();
}

long moveXYZSlow(double X, double Y, double Z) {


  calculateAngles(X, Y, Z, angles);
  configureSteppersSlow();
  positions[0] = angles[0] * RELATION_BASE;
  positions[1] = angles[1] * RELATION_HOMBRO;
  positions[2] = angles[2] * RELATION_CODO;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition();
}
void pruebaMoveXYZ() {



  if (CapturaX == 0) {
    Serial.println("Inserte la posicion en X deseada en cm.");
    while (CapturaX != 1) {

      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        posXvalue = data.toDouble();
        Serial.print("El punto en X elegido es de ");
        Serial.print(posXvalue);
        Serial.println("cm.");
        CapturaX = 1;
      }
    }
  }

  if (CapturaY == 0) {
    Serial.println("Inserte la posicion en Y deseada en cm.");
    while (CapturaY != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        posYvalue = data.toDouble();
        Serial.print("El punto en Y elegido es de ");
        Serial.print(posYvalue);
        Serial.println("cm.");
        CapturaY = 1;
      }
    }
  }

  if (CapturaZ == 0) {
    Serial.println("Inserte la posicion en Z deseada en cm.");
    while (CapturaZ != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        posZvalue = data.toDouble();
        Serial.print("El punto en Z elegido es de ");
        Serial.print(posZvalue);
        Serial.println("cm.");
        CapturaZ = 1;
        Reset = 0;
      }
    }
  }

  moveXYZ(posXvalue, posYvalue, posZvalue);

  //Reset
  if (Reset == 0) {
    Serial.println("==========================================================================");
    Serial.println("                           Movimiento terminado                         ");
    Serial.println("==========================================================================");
    Serial.println("Si desea solo reiniciar 1, regresar a 0, 90, 0 pulse 2, hacer homing 3");
    while (Reset != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        resetValue = data.toInt();
        if (resetValue == 1) {
          reset();
          Reset = 1;
        } else if (resetValue == 2) {
          reset();
          moveStepperstoAngle(0, 90, 0);
          Reset = 1;
        } else if (resetValue == 3) {
          homeSteppers();
          reset();
          Reset = 1;
        }
      }
    }
  }
}

void pruebaMoveXYZ2() {
  if (CapturaXYZ == 0) {
    Serial.println("Inserte las coordenadas en el formato X,Y,Z.");
    while (CapturaXYZ != 1) {
      while (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        int comma1 = data.indexOf(',');
        int comma2 = data.lastIndexOf(',');
        if (comma1 != -1 && comma2 != -1 && comma1 != comma2) {
          posXvalue = data.substring(0, comma1).toFloat();
          posYvalue = data.substring(comma1 + 1, comma2).toFloat();
          posZvalue = data.substring(comma2 + 1).toFloat();
          Serial.print("Las coordenadas elegidas son (X,Y,Z): ");
          Serial.print(posXvalue);
          Serial.print(",");
          Serial.print(posYvalue);
          Serial.print(",");
          Serial.println(posZvalue);
          CapturaXYZ = 1;
          Reset = 0;
        } else {
          Serial.println("Formato incorrecto. Por favor, utilice el formato X,Y,Z.");
        }
      }
    }
  }

  moveXYZ(posXvalue, posYvalue, posZvalue);

  //Reset
  if (Reset == 0) {
    Serial.println("==========================================================================");
    Serial.println("                           Movimiento terminado                         ");
    Serial.println("==========================================================================");
    Serial.println("Si desea solo reiniciar 1, regresar a 0, 90, 0 pulse 2, hacer homing 3");
    while (Reset != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        resetValue = data.toInt();
        if (resetValue == 1) {
          Serial.println("Reset 1");
          reset();
          Reset = 1;
        } else if (resetValue == 2) {
          reset();
          moveStepperstoAngle(0, 90, 0);
          Reset = 1;
        } else if (resetValue == 3) {
          homeSteppers();
          reset();
          Reset = 1;
        }
      }
    }
  }
}
void practicaLab() {

  //Moneda []

  //Moneda [2,2]
  megaGripper.write(110);
  moveXYZ(0, 12, 31);
  delay(100);
  openGripper();
  delay(1000);
  moveXYZ(0, 25, 20);
  delay(1000);
  closeGripper();
  delay(1000);
  moveXYZ(0, 24, 23);
  delay(100);
  moveXYZ(0, 21, 27);
  delay(100);
  moveXYZ(0, 12, 31);
  delay(100);
  moveXYZ(20, -7, 27);
  openGripper();
  delay(1000);

  //Moneda [1,2]
  megaGripper.write(110);
  moveXYZ(0, 20, 28);
  delay(100);
  moveXYZ(0, 22, 20);
  delay(1000);
  closeGripper();
  delay(1000);
  moveXYZ(0, 18, 30);
  delay(100);
  moveXYZ(0, 12, 31);
  delay(100);
  moveXYZ(20, -7, 27);
  delay(1000);
  openGripper();
  delay(1000);
  //==============================

  //Moneda [2,1]
  megaGripper.write(130);
  moveXYZ(0, 12, 31);
  delay(100);
  moveXYZ(7, 22, 20);
  delay(1000);
  closeGripper();
  delay(1000);
  moveXYZ(7, 13, 31);
  delay(100);
  moveXYZ(20, -7, 27);
  delay(1000);
  openGripper();
  delay(1000);

  //=============================

  //Moneda [3,2]
  megaGripper.write(110);
  moveXYZ(0, 12, 31);
  megaGripper.write(110);
  openGripper();
  delay(1000);
  moveXYZ(-7, 24.5, 20);
  delay(1000);
  closeGripper();
  delay(1000);
  moveXYZ(-7, 22, 25);
  delay(100);
  moveXYZ(0, 12, 31);
  delay(100);
  moveXYZ(20, -7, 27);
  openGripper();
  delay(1000);



  //Moneda [3,1]
  megaGripper.write(110);
  moveXYZ(0, 12, 31);
  openGripper();
  delay(1000);
  moveXYZ(-7, 21, 20);
  closeGripper();
  delay(1000);
  moveXYZ(-7, 21, 26);
  moveXYZ(0, 12, 31);
  moveXYZ(20, -7, 27);
  openGripper();
  delay(1000);

  //=============================
  //Moneda [1,1]
  // megaGripper.write(130);
  // moveXYZ(0,12,31);
  // moveXYZ(6.5,25,20);
  // delay(1000);
  // closeGripper();
  // delay(1000);
  // moveXYZ(9,20,25);
  // moveXYZ(7,13,31);
  // moveXYZ(20, -7, 27);
  // delay(1000);
  // openGripper();
  // delay(1000);


  laboratorio = 0;
  laboratorioDone = 0;

  //Reset
  if (laboratorioDone == 0) {
    Serial.println("==========================================================================");
    Serial.println("                           Movimiento terminado                         ");
    Serial.println("==========================================================================");
    Serial.println("Si desea reiniciar 1, prueba XYZ 2 y hacer home y reset 3");
    while (laboratorioDone != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        labValue = data.toInt();
        if (labValue == 1) {
          laboratorio = 1;
          laboratorioDone = 1;
        } else if (labValue == 2) {
          pruebaXYZ = 1;
          laboratorioDone = 1;
        } else if (labValue == 3) {
          homeSteppers();
          laboratorio = 1;
          laboratorioDone = 1;
        }
      }
    }
  }
}
//Posición central 1 0,-25,20
//Posición central 2 0, -22, 20

void grabPiece() {
  megaGripper.write(110);
  delay(500);
  moveXYZ(24, 3, 20);
  delay(500);
  openGripper();
  delay(500);
  moveXYZSlow(24, 4, 20);
  delay(500);
  moveXYZSlow(24, 5, 17);
  delay(500);
  closeGripper();
  delay(500);
  moveXYZ(0, 0, 37);
}


void dropPiece1() {
  megaGripper.write(110);
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
  moveXYZ(-11, 13, 27);
  delay(500);
  openGripper();
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
}

void dropPiece2() {
  megaGripper.write(110);
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
  moveXYZ(-8, 14, 26);
  delay(500);
  openGripper();
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
}

void dropPiece3() {
  megaGripper.write(110);
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
  moveXYZ(-5, 15, 26);
  delay(500);
  openGripper();
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
}

void dropPiece4() {
  megaGripper.write(120);
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
  moveXYZ(-2, 15, 26);
  delay(500);
  openGripper();
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
}

void dropPiece5() {
  megaGripper.write(130);
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
  moveXYZ(2, 16, 26);
  delay(500);
  openGripper();
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
}

void dropPiece6() {
  megaGripper.write(140);
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
  moveXYZ(5, 17, 26);
  delay(500);
  openGripper();
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
}

void dropPiece7() {
  megaGripper.write(140);
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
  moveXYZ(9, 17, 26);
  delay(500);
  openGripper();
  delay(500);
  moveXYZ(0, 12, 31);
  delay(500);
}

void givepiecePlayer() {
  moveXYZ(0, 12, 31);
  delay(500);
  moveXYZ(0, 4, 37);
  delay(3000);
  openGripper();
  delay(500);
}

void bailecito() {
  moveXYZ(0,18,31);
  openGripper();
  delay(250);
  closeGripper();
  delay(250);
  openGripper();
  delay(250);
  closeGripper();
  moveXYZ(0, 14, 33);
  delay(500);
  moveXYZ(5, 5, 28);
  moveXYZ(0, 0, 28);
  delay(500);
  moveXYZ(5, 5, 28);
  moveXYZ(0, 0, 28);
  delay(500);
  moveXYZ(20, -3, 28);


}

void setup() {


  //======================================
  Serial.begin(9600);

  miliGripper.attach(miliGripperPin);
  megaGripper.attach(megaGripperPin);

  pinMode(switchBaseR, INPUT_PULLUP);
  pinMode(switchBaseL, INPUT_PULLUP);
  pinMode(switchHombro, INPUT_PULLUP);
  pinMode(switchCodo, INPUT_PULLUP);


  //====================================
  //Add Steppers to Multistepper
  steppers.addStepper(stepBase);
  steppers.addStepper(stepHombro);
  steppers.addStepper(stepCodo);
  //====================================

  //=====================================
  //Test Servos
  closeGripper();
  openGripper();
  megaGripper.write(110);
  //=====================================
}

void loop() {



  if (homing == 1) {
    homeSteppers();
    delay(5);
    homing = 0;
  }

  if (pruebaGrab == 1) {
    grabPiece();
  }

  if (pruebabailecito == 1) {
    bailecito();
    pruebabailecito = 0;
  }


  if (laboratorio == 1) {
    practicaLab();
    delay(5);
    laboratorio == 0;
  }

  if (pruebaXYZ == 1) {
    pruebaMoveXYZ2();
  }
  
  if (pruebaGripper ==1) {
    openGripper();
    delay(1000);
    closeGripper();
    delay(1000);

  }  

  //####------------------------------------------####
  //--------------COMMUNICATION PYTHON----------------
  //####------------------------------------------####

  while (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');

    if (data == "test") {

      Serial.print("Mensaje recibido en el Arduino");
      digitalWrite(ledPrueba, HIGH);
      delay(500);
      digitalWrite(ledPrueba, LOW);
      delay(500);
      digitalWrite(ledPrueba, HIGH);
      delay(500);
      digitalWrite(ledPrueba, LOW);
      delay(500);
      digitalWrite(ledPrueba, HIGH);

    }

    else if (data == "homeSteppers") {
      Serial.print("Mensaje recibido en el Arduino, haciendo Homing...");
      homeSteppers();
    }

    else if (data == "Iniciojuego") {
      grabPiece();
      givepiecePlayer();
    }

    else if (data == "Posicion1") {
      grabPiece();
      dropPiece1();
    }

    else if (data == "Posicion2") {
      grabPiece();
      dropPiece2();
    }

    else if (data == "Posicion3") {
      grabPiece();
      dropPiece3();
    }

    else if (data == "Posicion4") {
      grabPiece();
      dropPiece4();
    }

    else if (data == "Posicion5") {
      grabPiece();
      dropPiece5();
    }

    else if (data == "Posicion6") {
      grabPiece();
      dropPiece6();
    }

    else if (data == "Posicion7") {
      grabPiece();
      dropPiece7();
    }

    else if (data == "Bailecito") {
      bailecito();
    }

    //####------------------------------------------####
    //--------------COMMUNICATION PYTHON----------------
    //####------------------------------------------####
  }
}
//#######################################################
//-------------PRUEBA LIMITSWTICH-------------
//#######################################################

// Serial.print("Base derecha: ");
// Serial.println(digitalRead(switchBaseR));
// Serial.print("Base izquierda ");
// Serial.println(digitalRead(switchBaseL));
// Serial.print("Hombro ");
// Serial.println(digitalRead(switchHombro));
// Serial.print("Codo ");
// Serial.println(digitalRead(switchCodo));
// delay(1000);


//#######################################################
//-------------PRUEBA MOTORES-------------
//#######################################################



// positions[0] = 50;
// positions[1] = 10000;
// positions[2] = 2500;
// steppers.moveTo(positions);
// steppers.runSpeedToPosition(); // Blocks until all are in position
// delay(1000);

// // Move to a different coordinate
// positions[0] = -0;
// positions[1] = 10000;
// positions[2] = -250;
// steppers.moveTo(positions);
// steppers.runSpeedToPosition(); // Blocks until all are in position
// delay(1000);
// Serial.println("Movimiento Terminado");

//Test Grippers
// Serial.println("Prueba Mega Gripper");
// megaGripper.write(0);
// delay(1000);
// megaGripper.write(90);
// delay(1000);

// Serial.println("Prueba Mili Gripper");
// miliGripper.write(0);
// delay(1000);
// miliGripper.write(90);
// delay(1000);

// delay(3000);
