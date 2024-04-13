#include <AccelStepper.h>
#include <MultiStepper.h>

//Stepper 1 (x)
#define dirPinx 5
#define stepPinx 4
//Stepper 2 (y)
#define dirPiny 8
#define stepPiny 9
//Stepper 3 (z)
#define dirPinz 12
#define stepPinz 13
  
#define enx 5
#define eny 10

//Limit switches

// AccelStepper
AccelStepper stepper1(1, dirPinx, stepPinx);
AccelStepper stepper2(1, dirPiny, stepPiny);
AccelStepper stepper3(1, dirPinz, stepPinz);

// Multistepper
MultiStepper steppers;

//Crear límites
int xlimit = 12;
int ylimit = 12;

//Variables para cálculo de ángulos
float xangle = 0.0;
float yangle = 0.0;
float step = 0.1;
long steps[3];
float incAngulo = 0.1;

// Variables de posición.
int Reset = 0;
int resetValue = 0;
int direccion = 0;
int posXvalue = -1;
int posYvalue = -1;
int radiovalue = -1;
int porcientovalue = -1;
int posibilvalue = -1;

//Cálculo de ángulos
int secciones;
int anguloInicial;
int anguloFinal;

//Variables de flujo del programa.
int PosicionInicial = 0;
int CapturaposX = 0;
int CapturaposY = 0;
int Capturaradio = 0;
int Capturaporciento = 0;
int Capturaposibilidades = -1;
int Elegirdireccion = 0;
int InicioCirculo = 0;

String data;

//Función para calcular relación en cm.
float relationcm(){
  float relation = (1000 / 6)/step;
  return relation;
}

//Función para mover motores
void moveMotorX (float MovX){
  steps[0] = MovX * relationcm();
}

void moveMotorY (float MovY){
  steps[1] = MovY * relationcm();
}

void moveMotorZ (float MovZ){
  steps[2]  = MovZ * relationcm();
}


// Función para calcular la distancia entre dos puntos
float distancia(float x1, float y1, float x2, float y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// // Función para verificar si el círculo cabe en el plano
// bool circuloCabeEnPlano(float x, float y, float r) {
//   // Distancias entre el centro del círculo y los bordes del plano
//   if ()

//   // Encuentra la distancia mínima entre el centro del círculo y los bordes del plano
//   float distanciaMinima = min(min(d1, d2), min(d3, d4));

//   // Comprueba si el radio del círculo es menor o igual a la distancia mínima
//   return r <= distanciaMinima;
// }

// Función para verificar si el círculo cabe en el plano
void definirAngulos(float x_c, float y_c, float r) {

  //Variables internas de la función.
  int anguloInicialposible1 = -1;
  int anguloInicialposible2 = -1;
  int anguloInicialposible3 = -1;
  int anguloInicialposible4 = -1;
  int anguloFinalposible1 = -1;
  int anguloFinalposible2 = -1;
  int anguloFinalposible3 = -1;
  int anguloFinalposible4 = -1;
  int inic = -1;

  for (int angle = 1; angle <= 361; angle++) {

    float anguloRad = radians(angle);

    float x = x_c + r * cos(anguloRad);
    float y = y_c + r * sin(anguloRad);

    if (x >= 0 && x <= xlimit && y >= 0 && y <= ylimit) {
      //Serial.print(angle);
      //Serial.print(", ");

      if (inic == -1) {
        anguloInicialposible1 = angle;
        anguloFinalposible1 = angle;
        inic = 1;

      } else if (angle - anguloFinalposible1 == 1 && inic == 1) {
        anguloFinalposible1 = angle;

      } else if (angle - anguloFinalposible1 > 1 && inic == 1) {
        anguloInicialposible2 = angle;
        anguloFinalposible2 = angle;
        inic = 2;

      } else if (angle - anguloFinalposible2 == 1 && inic == 2) {
        anguloFinalposible2 = angle;

      } else if (angle - anguloFinalposible2 > 1 && inic == 2) {
        anguloInicialposible3 = angle;
        anguloFinalposible3 = angle;
        inic = 3;

      } else if (angle - anguloFinalposible3 == 1 && inic == 3) {
        anguloFinalposible3 = angle;

      } else if (angle - anguloFinalposible3 > 1 && inic == 3) {
        anguloInicialposible4 = angle;
        anguloFinalposible4 = angle;
        inic = 4;

      } else if (angle - anguloFinalposible4 && inic == 4) {
        anguloFinalposible4 = angle;
      }
    }
  }
  //Si tenemos una seccion, el rango será el mismo rango posible.
  if (anguloInicialposible1 > anguloInicialposible2) {

    anguloInicial = anguloInicialposible1;
    anguloFinal = anguloFinalposible1;
    secciones = 1;
  }

  //Si tenemos dos secciones se deberán usar ángulos complementarios, en este caso el usuario puede elegir en cual hacerlo
  else if (anguloInicialposible1 <= anguloInicialposible2 && anguloInicialposible3 == -1) {
    anguloInicial = anguloInicialposible2;
    anguloFinal = anguloFinalposible1 + 360;
    secciones = 2;
  }

  //Si tenemos tres o más secciones
  else if (anguloInicialposible2 <= anguloInicialposible3){

    int a = anguloFinalposible1 - anguloInicialposible1;
    int b = anguloFinalposible2 - anguloInicialposible2;
    int c = anguloFinalposible3 - anguloInicialposible3;
    int d = anguloFinalposible4 - anguloInicialposible4;
    int max = a;
    // Serial.print("Rango a: ");
    // Serial.print(a);
    // Serial.print(", rango b: ");
    // Serial.print(b);
    // Serial.print(", rango c: ");
    // Serial.print(c);
    // Serial.print(", rango d: ");
    // Serial.println(d);

    anguloInicial = anguloInicialposible1;
    anguloFinal = anguloFinalposible1;

    if (b>max){
      max = b;
      anguloInicial = anguloInicialposible2;
      anguloFinal = anguloFinalposible2;
    }
    if (c>max){
      max = c;
      anguloInicial = anguloInicialposible3;
      anguloFinal = anguloFinalposible3;
    }
    if (d>max){
      max = d;
      anguloInicial = anguloInicialposible4;
      anguloFinal = anguloFinalposible4;
    }
    secciones = 3;
  }


  else {
    anguloInicial = -1;
    anguloFinal = -1;
  }

  // Serial.println("");
  // Serial.print("Primera seccion: ");
  // Serial.print(anguloInicialposible1);
  // Serial.print(", ");
  // Serial.print(anguloFinalposible1);
  // Serial.println(". ");
  // Serial.print("Segunda seccion: ");
  // Serial.print(anguloInicialposible2);
  // Serial.print(", ");
  // Serial.print(anguloFinalposible2);
  // Serial.println(". ");
  // Serial.print("Tercera seccion: ");
  // Serial.print(anguloInicialposible3);
  // Serial.print(", ");
  // Serial.print(anguloFinalposible3);
  // Serial.println(". ");
  // Serial.print("Cuarta seccion: ");
  // Serial.print(anguloInicialposible4);
  // Serial.print(", ");
  // Serial.print(anguloFinalposible4);
  // Serial.println(". ");


  // Serial.print("Angulo 1: ");
  // Serial.print(anguloInicial);
  // Serial.print(". ");
  // Serial.print("Angulo 2: ");
  // Serial.print(anguloFinal);
  // Serial.println(".");
}

void resetVars(){
  posXvalue = -1;
  posYvalue = -1;
  radiovalue = -1;
  porcientovalue = -1;
  posibilvalue = -1;
  CapturaposX = 0;
  CapturaposY = 0;
  Capturaradio = 0;
  Capturaporciento = 0;
  Capturaposibilidades = -1;
  PosicionInicial = 0;
  InicioCirculo = 0;
  data = "0";
  //Activar función para bajar cama para parar de escribir
  //
  //
  //
  //
  Serial.println("Reiniciando...");
  delay(250);
  Serial.println(".");
  delay(250);
  Serial.println("..");
  delay(250);
  Serial.println("...");
  delay(250);
  Serial.println("..");
  delay(250);
  Serial.println(".");
}
//Función para revisar cuántas veces cabe el x porcentaje.
int vecesCabe(int angulo_inicial, int angulo_final, float porcentaje) {
  float numporcent = porcentaje / 100;
  int rangoTotal = angulo_final - angulo_inicial;
  float cabe = floor(rangoTotal / (numporcent * 360));

  if (cabe < 1){
    numporcent = rangoTotal / (cabe * 360);
    Serial.println("Ingrese el porciento nuevamente.");
    Serial.print("El porcentaje mínimo que cabe es de: ");
    Serial.print(numporcent);
    Serial.println(".");
        
  }

  // Serial.print("Con un porcentaje de: ");
  // Serial.print(porcentaje);
  // Serial.print("%. El circulo cabe ");
  // Serial.print(cabe);
  // Serial.println(" veces.");
  return cabe;
}
// Función para mover los motores y construir un círculo
void construirCirculo(float centroX, float centroY, float radio, int anguloInicial, int anguloFinal) {
  for (float angulo = anguloInicial; angulo <= anguloFinal; angulo += incAngulo) {

    float paso = 0.1;

    float anguloRad = radians(angulo);  // Convertir el ángulo a radianes

    // Calcular las coordenadas x e y del círculo
    int nX = (centroX + radio * cos(anguloRad)) * relationcm();
    int nY = (centroY + radio * sin(anguloRad)) * relationcm();

    // Mover los motores a las coordenadas calculadas
    long circlePos[2] = { nX, nY };
    steppers.moveTo(circlePos);
    steppers.runSpeedToPosition();
  }
}


void setup() {
  Serial.begin(9600);
  // Agregar los motores al MultiStepper
  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);

  // Establecer la velocidad y aceleración para ambos motores
  stepper1.setMaxSpeed(3000.0);
  stepper1.setAcceleration(500.0);
  stepper2.setMaxSpeed(3000.0);
  stepper2.setAcceleration(500.0);

  // Mover los motores a la posición 0,0 (Home)
  long initialPos[2] = { 0, 0 };
  steppers.moveTo(initialPos);
  steppers.runSpeedToPosition();
}

void loop() {

  if (CapturaposX == 0) {
    Serial.println("");
    Serial.println("");
    Serial.println("==========================================================================");
    Serial.println("                             Iniciando Sistema                            ");
    Serial.println("==========================================================================");
    Serial.println("");
    Serial.println("Inserte la posicion en X deseada en cm.");
    while (CapturaposX != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        posXvalue = data.toInt();
        Serial.print("El punto en X elegido es de ");
        Serial.print(posXvalue);
        Serial.println("cm.");
        if (posXvalue < 0 or posXvalue > 12) {
          Serial.println("Digite un numero valido");
        }
      }
      if (posXvalue >= 0 and posXvalue <= 12) {
        CapturaposX = 1;
      }
    }
  }

  if (CapturaposY == 0) {
    Serial.println("Inserte la posicion en Y deseada en cm.");
    while (CapturaposY != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        posYvalue = data.toFloat();
        Serial.print("El punto en Y elegido es de ");
        Serial.print(posYvalue);
        Serial.println("cm.");
        if (posYvalue < 0 or posYvalue > 12) {
          Serial.println("Digite un numero valido");
        }
      }
      if (posYvalue >= 0 and posYvalue <= 12) {
        CapturaposY = 1;
      }
    }
  }

  if (Capturaradio == 0) {
    Serial.println("Inserte el radio deseado en cm.");
    while (Capturaradio != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        radiovalue = data.toInt();
        Serial.print("Ha ingresado el radio ");
        Serial.print(radiovalue);
        Serial.println("cm.");
        definirAngulos(posXvalue, posYvalue, radiovalue);
        if (radiovalue > 0 && radiovalue <= 12 && anguloInicial != -1 && anguloFinal != -1) {
          Capturaradio = 1;
        } else {
          radiovalue = -1;
          Serial.println("Ingrese el radio nuevamente");
        }
      }
    }
  }

  if (Capturaporciento == 0) {
    Serial.println("Inserte el porciento deseado.");
    while (Capturaporciento != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        porcientovalue = data.toInt();
        Serial.print("Ha ingresado el porciento: ");
        Serial.print(porcientovalue);
        Serial.println("%.");
        if (porcientovalue > 0 && porcientovalue <= 100) {

          if (vecesCabe(anguloInicial, anguloFinal, porcientovalue) == 1) {
            Capturaporciento = 1;
            PosicionInicial = 1;
            Serial.println("Ha ingresado los valores correctos.");

          } else if (vecesCabe(anguloInicial, anguloFinal, porcientovalue) > 1) {
            Capturaposibilidades = 0;
            Capturaporciento = 1;

          } else {
            porcientovalue = -1;
          }
        }
      }
    }
  }

  if (Capturaposibilidades == 0) {
    Serial.print("Puede realizar ");
    Serial.print(porcientovalue);
    Serial.print(" % de circulo ");
    Serial.print(vecesCabe(anguloInicial, anguloFinal, porcientovalue));
    Serial.print(" veces. Seleccione un numero del 1 al ");
    Serial.print(vecesCabe(anguloInicial, anguloFinal, porcientovalue));
    Serial.println(" para realizar el trazado.");

    while (Capturaposibilidades != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        posibilvalue = data.toInt();
        Serial.print("Ha ingresado el numero ");
        Serial.print(posibilvalue);
        Serial.println(".");
        if (posibilvalue >= 1 && posibilvalue <= vecesCabe(anguloInicial, anguloFinal, porcientovalue)) {
          float var_1 = porcientovalue * 0.01;
          int eqx = var_1 * 360;
          anguloFinal = anguloInicial + (eqx * posibilvalue);
          anguloInicial = anguloInicial + (eqx * (posibilvalue - 1));
          Capturaposibilidades = 1;
          PosicionInicial = 1;
        } else {
          posibilvalue = -1;
          Serial.println("Ingrese el numero nuevamente");
        }
      }
    }
  }

  if (PosicionInicial == 1) {
    Serial.print("Enviando a la posicion inicial de (");
    Serial.print(posXvalue);
    Serial.print(",");
    Serial.print(posYvalue);
    Serial.print(")");
    Serial.println("cm.");

    //Mover motores a coordenadas X y Y.
    moveMotorX(posXvalue);
    moveMotorY(posYvalue);
    steppers.moveTo(steps);
    steppers.runSpeedToPosition();  
    delay(1000);
    //Mover stepper a perímetro del círculo.
    float initialAngle = radians(anguloInicial);

    moveMotorX(posXvalue + radiovalue * cos(initialAngle));
    moveMotorY(posYvalue + radiovalue * sin(initialAngle));
    steppers.moveTo(steps);
    steppers.runSpeedToPosition();   
    delay(500);
    //Activar función para subir cama para escribir.
    //moveMotorZ(posZvalue);
    //steppers.moveTo(steps);
    //steppers.runSpeedToPosition();
    delay(500);  
    PosicionInicial = 0;
    InicioCirculo = 1;
        
  }

  if (InicioCirculo == 1) {
    Serial.print("Iniciando creacion de circulo en puntos (");
    Serial.print(posXvalue);
    Serial.print(",");
    Serial.print(posYvalue);
    Serial.print(")cm. Radio: ");
    Serial.print(radiovalue);
    Serial.print(" y ");
    Serial.print(porcientovalue);
    Serial.println("%.");
    Serial.print("Angulo inicial: ");
    Serial.print(anguloInicial);
    Serial.print(" grados. Angulo Final: ");
    Serial.print(anguloFinal);
    Serial.println(" grados.");

    construirCirculo(posXvalue, posYvalue, radiovalue, anguloInicial, anguloFinal);
    delay(200);
    Reset = 0;
    InicioCirculo = 0;
  }
  //Reset
  if (Reset == 0) {
    Serial.println("==========================================================================");
    Serial.println("                           Movimiento terminado                         ");
    Serial.println("==========================================================================");
    Serial.println("Si desea reiniciar 1.");
    Serial.println("Si desea reiniciar y regresar al origen pulse 2.");
    while (Reset != 1) {
      while (Serial.available() > 0) {
        data = Serial.readStringUntil('\n');
        resetValue = data.toInt();
        if (resetValue == 2) {
          resetVars();
          // Ir al punto de origen.
          long homePos[2] = { 0, 0 };
          steppers.moveTo(homePos);
          steppers.runSpeedToPosition();
          //Salir de reset.
          Reset = 1;
        }
        else if (resetValue == 1){
          resetVars();
          //Salir de reset.
          Reset = 1;
        }     
      }
    }
  }
}