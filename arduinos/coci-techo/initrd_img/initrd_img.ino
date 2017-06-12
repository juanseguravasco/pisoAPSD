// En primer lugar definimos el arduino que es, los actuadores y sensores que tiene
// que tiene y los pin del arduino que usa cada sensor/actuador
//
// Arduino: 3 (Cocina-techo)
// Sensores/Actuadores:
//  - 0 leeSensores
//  - actuadorBombilla - es la luz de la cocina, se enciende si detecta movimiento.
//    - 12 pinBombilla - Bombilla cocina
//  - actuadorventilador - es el actuador que anchufa el ventilador
//    - 13 pinVentilador - ventilador por deteccion de gas.
//  - sensorTemp - sensor de Temperatura 
//    - A0 pinSensorTH - sensor de Tª
//  - sensorPir : sensor pir octopus
//    - pin 2 : pinSensorPir
//  - sensorGas: detecta el nivel de gas de la estancia.
//    - pin A4 : pinSensorGas
//  - sensorLlama: sensor de llama
//    - pin 9 : pinSensorLlama


#define soyArduino '3'
// Sensores/Actuadores:
#define leeSensores '0'
#define actuadorBombilla '9'
#define actuadorVentilador '8'
#define sensorTemp '1'
#define sensorPir '2'
#define sensorGas '4'
#define sensorLlama '3'

// Pins del Arduino usados
#define pinBombilla 12
#define pinVentilador 13
#define pinAigua 11
#define pinSensorTH A0
#define pinSensorPir 2
#define pinSensorGas A4
#define pinSensorLlama 9

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  Serial.begin(9600);
//  Serial.println("Listo");

  pinMode(pinBombilla, OUTPUT);
  pinMode(pinVentilador, OUTPUT);
  pinMode(pinAigua, OUTPUT);
  pinMode(pinSensorTH, INPUT);
  pinMode(pinSensorPir, INPUT);
  pinMode(pinSensorGas, INPUT);
  pinMode(pinSensorLlama, INPUT); 
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
//      int segundoGuion=inputString.indexOf('-', primerGuion+1);
  //    int actuador=inputString.substring(primerGuion+1, segundoGuion-1).toInt();
      // Y el valor que será el tercero
  //    int valor=inputString.substring(segundoGuion+1).toInt();
//        delay(1000);
      char valor=inputString.charAt('4');
      switch(inputString.charAt(2)) {
          case leeSensores: {

            // Leer sensor temperatura
            int a = analogRead(pinSensorTH);
            float R = (1023.0/((float)a)-1.0)*100000.0;
            float t=1.0/(log(R/100000.0)/4275+1/298.15)-273.15;//convert to temperature via datasheet ;
            int temp = (int) t;
            
            // leer sensor movimiento
            int moviment = digitalRead(pinSensorPir);
            char datoMov;
            if (moviment == 1) {
              digitalWrite(pinBombilla,HIGH);
              datoMov='1';
            } else {
              digitalWrite(pinBombilla, LOW);
              datoMov='X';
            }
            
            //leer sensor gas
            int gas = analogRead(pinSensorGas); //Lemos la salida analógica del MQ
            if (gas > 650) digitalWrite(pinVentilador,HIGH);
            else digitalWrite(pinVentilador, LOW);
            
            
            //leer sensor llama
            int llama = digitalRead(pinSensorLlama);
            char datoLlama;
            if (llama ==1){
              digitalWrite(pinAigua,llama);
              datoLlama = '1';
            }else{
              datoLlama ='X';
            }
                      
            String datos="";
            datos += soyArduino;
            datos += ':';
            datos += sensorTemp;
            datos += '-';
            datos += temp;
            datos += ':';
            datos += sensorPir;
            datos += '-';
            datos += datoMov;
            datos += ':';
            datos += sensorGas;
            datos += '-';
            datos += gas;
            datos += ':';
            datos += sensorLlama;
            datos += '-';
            datos += datoLlama;         
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
          default:
            Serial.println("Sensor desconocido "+inputString);
            break;
      }

ç    }
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

