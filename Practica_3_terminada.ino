#include <Servo.h>

//Sensor ultrasónico
const int TriggerPin = 2;
const int EchoPin = 3;

//Creación del motor
Servo myServo;

//Límites de trabajo
String Valorinferior;
String ValorSuperior;


//Etapas

//Valores de límites
int LimInferiorValueMotor = 0;
int LimSuperiorValueMotor = 0;

//Límites del sensor
int LimInferiorSensor = 0;
int LimSuperiorSensor = 0;

//Valores de límites
int LimInferiorValueSensor = 0;
int LimSuperiorValueSensor = 0;

float Pendiente = 0;
float MovMotor = 0;

//Variables del ultrasónico
long t = 0;
long z = 0;

//Eliminación de picos ultrasónico
long y = 0;
int cont = 0;

//Cálculo de la media
float lecturas[75];
int posicion = 0;
float totalValues = 0;
float valormedio = 0; 
bool mediaCalculada = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  digitalWrite(TriggerPin, LOW);
   myServo.attach(6);
}

void loop(){

  LimSuperiorValueMotor = 180;
  LimInferiorValueMotor = 1;
  

 
  if (LimInferiorSensor == 0){
    Serial.println("Inserte limite inferior del sensor");
    while(LimInferiorSensor != 1){
       while (Serial.available() > 0){
         String Valorinferior = Serial.readStringUntil('\n');
         LimInferiorValueSensor = Valorinferior.toInt();
         Serial.println(LimInferiorValueSensor);
         if (LimInferiorValueSensor < 10 or LimInferiorValueSensor > 150){
           Serial.println("Digite un numero válido");
          }
        }
        if (LimInferiorValueSensor >= 10 and LimInferiorValueSensor <= 150) {
        LimInferiorSensor = 1;
        }
    }
  }
    
 
  if (LimSuperiorSensor == 0){
    Serial.println("Inserte limite superior del sensor");
    while(LimSuperiorSensor != 1){
        while (Serial.available() > 0){
          String ValorSuperior = Serial.readStringUntil('\n');
          LimSuperiorValueSensor = ValorSuperior.toInt();
          Serial.println(LimSuperiorValueSensor);
          if (LimSuperiorValueSensor <= LimInferiorSensor or LimSuperiorValueSensor > 200) {
            Serial.println("Digite un numero válido");
          }
        }
      if (LimSuperiorValueSensor > LimInferiorSensor and LimSuperiorValueSensor <= 200) {
        LimSuperiorSensor = 1 ;
      }
    }
  }


  //Data del Sensor ultrasónico

  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);         
  digitalWrite(TriggerPin, LOW);
  
  t = pulseIn(EchoPin, HIGH);
  z = t/59; 
      

  //Eliminación de picos del ultrasónico
  if (y==0)
  {y=z;}

  if (z>=y+5 and z<=y-5)
  {y=z;
  cont=0;
  }
  else 
  {cont++;}

  if(cont==5)
  {y=z;
  cont=0;
  }

  //Para sacar la media se actualiza un valor del array en una posición x.
  //Cuando acaba de empezar, se debe esperar que estén los 75 valores para iniciar el movimiento.

  totalValues = totalValues - lecturas[posicion];
  lecturas[posicion] = y;
  totalValues = totalValues + lecturas[posicion]; 
  posicion = posicion + 1;

  if (posicion >= 75){
    mediaCalculada = 1;
    posicion = 0;
  }

  //Se saca el valor medio y se imprime
  valormedio = totalValues / 75;
  if (mediaCalculada){
    Serial.print("Distancia: ");
    Serial.print(valormedio);
    Serial.println("cm");
    
  }

 

  if (mediaCalculada){
    Pendiente= (LimSuperiorValueMotor-LimInferiorValueMotor)/(LimSuperiorValueSensor-LimInferiorValueSensor);

  
    if (valormedio>=LimInferiorValueSensor and valormedio<=LimSuperiorValueSensor){ 
      //Si el valor de la media es mayor que el valor mínimo del sensor, se resta la media menos el límite inferior.
      if (valormedio>LimInferiorValueSensor){
        MovMotor= (valormedio-LimInferiorValueSensor);
      }
      //Si el valor de la media es menor que el valor mínimo del sensor, se resta el límite inferior a la media.
      if (valormedio<LimInferiorSensor){
        MovMotor= (LimInferiorValueSensor-valormedio);
      }
      //Si el valor medio es igual que el límite inferior, el motor se queda en el estado inicial.
      if (valormedio==LimInferiorValueSensor){
        MovMotor= 0;
      }
      //Con la pendiente y el movimiento del motor se tiene la posición del motor, empezando desde el límite inferior que es sumado
      myServo.write((Pendiente*MovMotor)+LimInferiorValueMotor);
    }
  }

}
