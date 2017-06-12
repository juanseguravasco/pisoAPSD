// En primer lugar definimos el arduino que es, los actuadores y sensores que tiene
// que tiene y los pin del arduino que usa cada sensor/actuador
//
// Arduino: 2 (Cocina)
// Sensores/Actuadores:
//  - 0 leeSensores
//  - actuadorBombilla - es la luz de la cocina. Usa los pins de arduino:
//    - 12 pinBombilla - Bombilla cocina
//  - actuadorGrifo - es el grifo de la cocina
//    - 4 pinEvGrifo - electrovalvula que abre/cierra el grifo
//  - sensorTemp - sensor de Temperatura del grifo
//    - 6 pinSensorTH - sensor de Tª y humedad del grifo
//  - sensorHum - sensor de humedad del grifo
//    - 6 pinSensorTH - sensor de Tª y humedad del grifo
//  - sensorCaudal - sensor de caudal del grifo
//    - 9 pinSensorCaudal - sensor de caudal del grifo

#define soyArduino '2'
// Sensores/Actuadores:
#define leeSensores '0'
#define actuadorBombilla '1'
#define actuadorGrifo '2'
#define sensorTemp '3'
#define sensorHum '4'
#define sensorCaudal '5'
// Pins del Arduino usados
#define pinBombilla 12
#define pinEvGrifo 4
#define pinSensorTH 6
#define pinSensorCaudal 9

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  Serial.begin(9600);
  Serial.println("Listo");

  pinMode(pinBombilla, OUTPUT);
  pinMode(pinEvGrifo, OUTPUT);
  pinMode(pinSensorTH, INPUT);
  pinMode(pinSensorCaudal, INPUT);
  inputString.reserve(200);   // reserve 200 bytes for the inputString
}

void loop() {
  // procesa la orden si está completa
  if (stringComplete) {
    // Obtenemos el primer elemento pasado que será el código del Arduino 
    // a quien se ha enviado la orden
    // Sólo procesamos la orden si somos su destinatario    
    if (inputString.charAt(0) == soyArduino) {
      // Obtenemos el actuador del segundo elemento pasado
      switch(inputString.charAt(2)) {
          case leeSensores: {
            // Leer sensores, ej., 
            int temp=25;
            int humedad=56;
            int caudal=15;
            String datos="";
            datos += soyArduino;
            datos += ':';
            datos += sensorTemp;
            datos += '-';
            datos += temp;
            datos += ':';
            datos += sensorHum;
            datos += '-';
            datos += humedad;
            datos += ':';
            datos += sensorCaudal;
            datos += '-';
            datos += caudal;
            Serial.println(datos);
            break;
          }
          case actuadorBombilla:
            switch (inputString.charAt(4)) {
              case '0':   // Apagar
                digitalWrite(pinBombilla, LOW);
                Serial.println("Off");
                break;
              case '1':   // Encender
                digitalWrite(pinBombilla, HIGH);
              Serial.println("On");
                break;
              default:
                Serial.println("Orden desconocida "+inputString);
            }
            break;
          case actuadorGrifo:
            switch (inputString.charAt(4)) {
              case '0':   // Cerrar
                // orden al pin evGrifo para cerrarlo
                Serial.println("Off");
                break;
              case '1':   // Abrir
                // orden al pin evGrifo para abrirlo
              Serial.println("On");
                break;
              default:
                Serial.println("Orden desconocida "+inputString);
            }
            break;
          default:
            Serial.println("Sensor desconocido "+inputString);
            break;
      }
    } else {
      Serial.println("Ard desc: "+inputString);
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }      
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n' || inChar == ';') {
      stringComplete = true;
    } else {
      // add it to the inputString:
      inputString += inChar;
    }
  }
}

