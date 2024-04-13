#define dirPinx 6
#define stepPinx 7
#define dirPiny 9
#define stepPiny 10
#define limitx 10
#define limity 10

float posicionActualX = 0.0;
float posicionActualY = 0.0;
double posXvalue = 0.00;
double posYvalue = 0.00;
double radiovalue = 0.00;
double y = 0.00;
double x = 0.00;
int CapturaposX = 0;
int CapturaposY = 0;
int Capturaradio = 0;
int InicioCirculo = 0;
int direccion = 0;

String data;

  // Función para mover los motores y construir un círculo
  void construirCirculo(float centroX, float centroY, float radio) {
    for (int angulo = 0; angulo <= 360; angulo++) {
      float anguloRad = radians(angulo);  // Convertir el ángulo a radianes

      // Calcular las coordenadas x e y del círculo
      float x = centroX + radio * cos(anguloRad);
      float y = centroY + radio * sin(anguloRad);

      // Mover los motores a las coordenadas calculadas
      moverMotorX(x);
      moverMotorY(y);

      delay(10);  // Ajustar según sea necesario para tu aplicación
    }
  }  
  
  // Función para mover el motor en el eje X
  void moverMotorX(float x) {
    // Calcular la dirección del movimiento
    direccion = (x > posicionActualX) ? HIGH : LOW;

    // Configurar la dirección del motor X
    digitalWrite(dirPinx, direccion);

    // Calcular la cantidad de pasos necesarios para llegar a la posición 'x'
    int pasos = calcularPasos(posicionActualX, x);

    // Realizar los pasos necesarios para llegar a la posición 'x'
    for (int i = 0; i < pasos; i++) {
      digitalWrite(stepPinx, HIGH);
      delayMicroseconds(100);
      digitalWrite(stepPinx, LOW);
      delayMicroseconds(100);
    }

    // Actualizar la posición actual del motor X
    posicionActualX = x;
  }


  // Función para mover el motor en el eje Y
  void moverMotorY(float y) {
    // Calcular la dirección del movimiento
    direccion = (y > posicionActualY) ? HIGH : LOW;

    // Configurar la dirección del motor Y
    digitalWrite(dirPiny, direccion);

    // Calcular la cantidad de pasos necesarios para llegar a la posición 'y'
    int pasos = calcularPasos(posicionActualY, y);

    // Realizar los pasos necesarios para llegar a la posición 'y'
    for (int i = 0; i < pasos; i++) {
      digitalWrite(stepPiny, HIGH);
      delayMicroseconds(100);
      digitalWrite(stepPiny, LOW);
      delayMicroseconds(100);
    }

    // Actualizar la posición actual del motor Y
    posicionActualY = y;
  }  

    // Función para calcular la cantidad de pasos necesarios para llegar a la posición deseada
  int calcularPasos(float posicionActual, float posicionDeseada) {
    // Lógica para calcular la cantidad de pasos necesarios
    // Utiliza la relación de pasos por centímetro para convertir la distancia a pasos
    return abs(posicionDeseada - posicionActual) * pasosPorCentimetro();  // Ajustar según tu lógica
  }

  // Función para obtener la relación de pasos por centímetro
  float pasosPorCentimetro() {
    // Lógica para obtener la relación de pasos por centímetro
    // Este es un ejemplo basado en tus especificaciones
    return 1587.3;  // 500 pasos corresponden a 1 cm
  }

  bool validarLimites(double x, double y, double radio){
    double limiteX = limitx;
    double limiteY = limity;

    //Calcular distancia entre el centro y los límites
    double distanciaX = x + radio;
    double distanciaY = y + radio;

    if (distanciaX > 1 && distanciaX <10 && distanciaY > 1 && distanciaY <10){
      return true;
    } else{
      return false;      
    }
  }
  // Función para validar si una porción de círculo cabe dentro del rectángulo
  bool validarPorcionCirculo(float centroX, float centroY, float radio, float anchoRectangulo, float altoRectangulo, float anguloPorcion){
    // Calcula las coordenadas del punto en el borde de la porción de círculo
    float puntoBordeX = centroX + radio * cos(anguloPorcion);
    float puntoBordeY = centroY + radio * sin(anguloPorcion);

    // Verifica si el punto del borde está dentro del rectángulo
    return (puntoBordeX >= 0 && puntoBordeX <= anchoRectangulo &&
            puntoBordeY >= 0 && puntoBordeY <= altoRectangulo);
  }  

void setup() {
   Serial.begin(9600);
   pinMode(dirPinx, OUTPUT);
   pinMode(stepPinx, OUTPUT);
   pinMode(dirPiny, OUTPUT);
   pinMode(stepPiny, OUTPUT);
   pinMode(limitx, INPUT_PULLUP);
   pinMode(limity, INPUT_PULLUP);
}
 
void loop() {

  if (CapturaposX == 0)  {
    Serial.println("Inserte la posición en X deseada en cm: (Este debe estar entre 1 y 10) ");
    while(CapturaposX != 1){
        while (Serial.available() > 0){
          data = Serial.readStringUntil('\n');
          posXvalue = data.toFloat();
          Serial.print("El punto en X elegido es de ");
          Serial.println(posXvalue);
          if (posXvalue < 1 or posXvalue > 10){
            Serial.println("Digite un numero valido");
          }
        }
      if (posXvalue >= 1 and posXvalue <= 10) {        
        CapturaposX = 1 ;
      }
    }
  }

  if (CapturaposY == 0)  {
    Serial.println("Inserte la posición en Y deseada en cm: (Este debe estar entre 1 y 10) ");
    while(CapturaposY != 1){
        while (Serial.available() > 0){
          data = Serial.readStringUntil('\n');
          posYvalue = data.toFloat();
          Serial.print("El punto en Y elegido es de ");
          Serial.println(posYvalue);
          if (posYvalue < 1 or posYvalue > 10){
            Serial.println("Digite un numero valido");
          }
        }
      if (posYvalue >= 1 and posYvalue <= 10) {        
        CapturaposY = 1 ;
      }
    }
  }
      
  if (Capturaradio == 0) {
    Serial.println("Inserte el radio deseado en cm: (Este debe estar entre 1 y 10) ");
    while(Capturaradio != 1){
      while (Serial.available() > 0){
        data = Serial.readStringUntil('\n');
        radiovalue = data.toFloat();
        Serial.print("Ha ingresado el radio ");
        Serial.println(radiovalue);
        if (radiovalue > 1 && radiovalue < 10 && validarLimites(posXvalue, posYvalue, radiovalue)) {
          Serial.println("Ha ingresado los valores correctos");
          Capturaradio = 1;
          InicioCirculo = 1;
                 
        }
        else {
          radiovalue = 0;
          Serial.println("Ingrese el radio nuevamente");
        }         
      }
    }
  }

  if (InicioCirculo == 1) {
    Serial.println("Iniciando creación de círculo en puntos x:");
    Serial.print(posXvalue);
    Serial.print(" y: ");
    Serial.print(posYvalue);
    Serial.print(" de radio: ");   
    Serial.print(radiovalue);
    Serial.print(" .");    

    construirCirculo(posXvalue, posYvalue, radiovalue); 
      
  }
    //Reset
    x = 0;
    posYvalue = 0;
    posXvalue = 0;
    radiovalue = 0;
    CapturaposX = 0;
    CapturaposY = 0;
    Capturaradio =0;
    InicioCirculo = 0;
    data = "0";
}  
