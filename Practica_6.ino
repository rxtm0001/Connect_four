  #include <AccelStepper.h>
  #include <MultiStepper.h>

  //Stepper 1 (x)
  #define dirPinx 8
  #define stepPinx 9
  //Stepper 2 (y)
  #define dirPiny 4
  #define stepPiny 5
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

  // Contador para porciento de los círculos
  int count = 0;
  int MaxCount = 0;

  // Variables de posición.
  int Reset = 0;
  int resetValue = 0;
  int direccion = 0;
  int posXvalue = -1;
  int posYvalue = -1;
  int radiovalue = -1;
  int circulovalue = -1;
  int porcientovalue = -1;
  int posibilvalue = -1;

  //Cálculo de ángulos
  int secciones;
  int anguloInicial;
  int anguloFinal;
  int anguloInicial1;
  int anguloFinal1;
  int anguloInicial2;
  int anguloFinal2;

  //Variables de flujo del programa.
  int PosicionInicial = 0;
  int CapturaposX = 0;
  int CapturaposY = 0;
  int CapturaposZ = 0;
  int Capturaradio = 0;
  int Capturacantcirc = 0;
  int Capturaporciento = 0;
  int Capturaposibilidades = -1;
  int Elegirdireccion = 0;
  int InicioCirculo = 0;
  int PrimerCirculo = 0;
  int SegundoCirculo = 0;
  int TercerCirculo = 0;

  String data;

  //Función para calcular relación en cm.
  float relationcm() {
    float relation = (1000 / 6) / step;
    return relation;
  }

  //Función para mover motores
  void moveMotorX(float MovX) {
    steps[0] = MovX * relationcm();
  }

  void moveMotorY(float MovY) {
    steps[1] = MovY * relationcm();
  }

  void moveMotorZ(float MovZ) {
    steps[2] = MovZ * relationcm();
  }


  // Función para calcular la distancia entre dos puntos
  float distancia(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
  }

  // Función para verificar si el círculo cabe en el plano
  int definirAngulos(float x_c, float y_c, float r) {

    //Variables internas de la función.
    int anguloInicialposible1 = -1;
    int anguloFinalposible1 = -1;
    int inic = -1;
    int output = -1;

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
        }
      }
    }
    //  Serial.println("Angulos");
    //  Serial.println(anguloInicialposible1);
    //  Serial.println(anguloFinalposible1);

    if (anguloInicialposible1 == 1 && anguloFinalposible1 == 361) {
      output = 1;
    }

    else {
      output = 0;
    }

    return output;
  }

  void resetVars() {
    posXvalue = -1;
    posYvalue = -1;
    radiovalue = -1;
    porcientovalue = -1;
    posibilvalue = -1;
    circulovalue = -1;
    CapturaposX = 0;
    CapturaposY = 0;
    Capturaradio = 0;
    Capturaporciento = 0;
    Capturacantcirc = 0;
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

    if (cabe < 1) {
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


      if (MaxCount >= count) {
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

    if (Capturacantcirc == 0) {
      Serial.println("Inserte la cantidad de círculos (1-3).");
      while (Capturacantcirc != 1) {
        while (Serial.available() > 0) {
          data = Serial.readStringUntil('\n');
          circulovalue = data.toInt();
          Serial.print("Ha seleccionado realizar ");
          Serial.print(circulovalue);
          Serial.println(" circulos.");

          if (circulovalue > 0 && radiovalue <= 3) {
            Capturacantcirc = 1;
          } else {
            circulovalue = -1;
            Serial.println("Ingrese la cantidad de circulos nuevamente (1-3)");
          }
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
          //Revisar todos los círculos

          //Primer círculo
          int Fst = definirAngulos(posXvalue, posYvalue, radiovalue);
          //Segundo círculo
          int Sec = definirAngulos(posXvalue + (1.61 * radiovalue * cos(radians(45))), posYvalue + (1.61 * radiovalue * sin(radians(45))), radiovalue);
          //Tercer círculo.
          int Thd = definirAngulos(posXvalue + (1.61 * radiovalue * cos(radians(135))), posYvalue + (1.61 * radiovalue * sin(radians(135))), radiovalue);
          if (circulovalue == 1) {
            if (radiovalue > 0 && radiovalue <= 12 && Fst == 1) {
              Capturaradio = 1;
            } else {
              radiovalue = -1;
              Serial.println("Ingrese el radio nuevamente");
            }
          } else if (circulovalue == 2) {
            if (radiovalue > 0 && radiovalue <= 12 && Fst == 1 && Sec == 1) {
              Capturaradio = 1;
            } else {
              radiovalue = -1;
              Serial.println("Ingrese el radio nuevamente");
            }
          } else if (circulovalue == 3) {
            if (radiovalue > 0 && radiovalue <= 12 && Fst == 1 && Sec == 1 && Thd) {
              Capturaradio = 1;
            } else {
              radiovalue = -1;
              Serial.println("Ingrese el radio nuevamente");
            }
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

          //Fórmula para calcular el porciento del desarrollo completo.
          //Se multiplica el porciento por la cantidad de círculos por 360 para saber cuántos círculos se deben realizar.
          //En donde cada círculo son 360 grados.
          if (porcientovalue > 0 && porcientovalue <= 100) {

            int cant_angulos = porcientovalue * circulovalue * 3.6;
            //Serial.println(cant_angulos);
            MaxCount = cant_angulos;

            if (MaxCount <= 360) {
              anguloInicial = 0;
              anguloFinal = MaxCount;
            }

            else if (MaxCount > 360) {
              anguloInicial = 0;
              anguloFinal = 360;
            }

            if (MaxCount > 360 && MaxCount <= 720) {
              anguloInicial1 = 360;
              anguloFinal1 = MaxCount;
            }

            else if (MaxCount > 720) {
              anguloInicial1 = 360;
              anguloFinal1 = 720;
              anguloInicial2 = 720;
              anguloFinal2 = MaxCount;
            }

            PosicionInicial = 1;
            Capturaporciento = 1;
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
      delay(2000);
      PosicionInicial = 0;
      PrimerCirculo = 1;
    }

    if (PrimerCirculo == 1) {
      Serial.print("Construyendo circulo central: ");
      Serial.print("Angulo Inicial:");
      Serial.print(anguloInicial);
      Serial.print("-- Angulo Final: ");
      Serial.print(anguloFinal);
      Serial.println(".");
      construirCirculo(posXvalue, posYvalue, radiovalue, anguloInicial, anguloFinal);
      delay(1000);
      PrimerCirculo = 0;
      SegundoCirculo = 1;
    }

    if (SegundoCirculo == 1 && anguloInicial1 > 0) {
      Serial.print("Construyendo circulo de la izquierda: ");
      Serial.print("Angulo Inicial:");
      Serial.print(anguloInicial1);
      Serial.print("-- Angulo Final:");
      Serial.print(anguloFinal1);
      Serial.println(".");
      float x1 = posXvalue + (1.61 * radiovalue * cos(radians(45)));
      float y1 = posYvalue + (1.61 * radiovalue * sin(radians(45)));
      construirCirculo(x1, y1, radiovalue, anguloInicial1, anguloFinal1);
      delay(1000);
      SegundoCirculo = 0;
      TercerCirculo = 1;
    }

    if (TercerCirculo == 1 && anguloInicial2 > 0) {
      Serial.print("Construyendo circulo de la derecha: ");
      Serial.print("Angulo Inicial: ");
      Serial.print(anguloInicial2);
      Serial.print("-- Angulo Final: ");
      Serial.print(anguloFinal2);
      Serial.println(".");
      float x2 = posXvalue + (1.61 * radiovalue * cos(radians(135)));
      float y2 = posYvalue + (1.61 * radiovalue * sin(radians(135)));
      construirCirculo(x2, y2, radiovalue, anguloInicial2, anguloFinal2);
      delay(1000);
      TercerCirculo = 0;
      Reset = 0;
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
          } else if (resetValue == 1) {
            resetVars();
            //Salir de reset.
            Reset = 1;
          }
        }
      }
    }
  }