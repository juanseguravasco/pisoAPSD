#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ   (2)
#define PN532_RESET (3)

#define soyArduino 'C'
#define leeSensores '0'
#define sensorAuthRfid '1'
#define actuadorDoor '9'
#define pinDoor '10'

int pdActivo=9;
int pdCodigoOk=10;
int pdCodigoNOk=11;
int pdLeyendo=12;

int delaySiguienteLectura=100;
bool authOk=false;

int c0=0;
int c1=0;
int c2=0;
int c3=0;

int delayCondicionError=1000;
int tiempoMantenerEstado_Auth_Ok_NOk=3000;

byte tLeidos=0;

int tiempo1_Max=8000; //Milisegons màx. de espera hasta petición de código desde la Rasp
int tiempo1_Actual=-1;
int tiempo1_Inic=-1;
bool tiempo1_Activo=false;

String inputString="";
boolean stringComplete=false;
String respuestaCodigoNoLeido="n";

//Configuración por shield con conexión I2C
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {
  Serial.begin(9600);
  //Serial.println("Iniciado...");
  
  pinMode(pdActivo, OUTPUT);
  pinMode(pdLeyendo, OUTPUT);
  pinMode(pdCodigoOk, OUTPUT);
  pinMode(pdCodigoNOk, OUTPUT);
  
  nfc.begin();
  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    setError();
    //Serial.print("Tarjeta PN53x no encontrada");
    while (1); // halt
  }
  else
  {
    //Serial.print("Tarjeta PN53x encontrada");
  }
  //Configurar para leer etiquetas RFID
  nfc.SAMConfig();
  setInicial();
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Almacena el UID leido
  uint8_t uidLength;                        // Longitud del UID: 4 o 7 bytes dependiendo del tipo de tarjeta ISO14443A

  if ((tLeidos == 1) && (!tiempo1_Activo))
  {
    tiempo1_Inic=millis();
    tiempo1_Activo=true;
  }
  if (tLeidos > 0)
  {
    tiempo1_Actual=millis();
  }
  if ((tiempo1_Actual-tiempo1_Inic)>tiempo1_Max)
  {
    setInicial();
  }

  if (!tiempo1_Activo)
  {
    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    if (success) {
      if (uidLength == 4)
      {
        setLeyendo();
        delay(50);
      
        c0=uid[0];
        c1=uid[1];
        c2=uid[2];
        c3=uid[3];
        authOk=chkCodigo(c0,c1,c2,c3);
        if (authOk)
        {
         //Serial.println("Auth Ok");
         setAuthOk();
         delay(tiempoMantenerEstado_Auth_Ok_NOk);
         setInicial();
        }
        else
        {
         tLeidos=1;
         //Serial.println("Auth NOk");
         //setAuthNOk();
         //delay(tiempoMantenerEstado_Auth_Ok_NOk);
         //setInicial();
        }
        //Serial.println(c0);
        //Serial.println(c1);
        //Serial.println(c2);
        //Serial.println(c3);
        delay(delaySiguienteLectura);
      }
      else
      {
        setError();
        delay(delayCondicionError);
        setInicial();
      }
    } //if success
  }//if no tiempo1_Activo

  if (stringComplete)
  {
    //Serial.print("Peticion: ");Serial.println(inputString);
    //Serial.print("char(0): ");Serial.println(inputString.charAt(0));
    //Serial.print("char(1): ");Serial.println(inputString.charAt(1));
    //Serial.print("char(2): ");Serial.println(inputString.charAt(2));
    //Serial.print("char(3): ");Serial.println(inputString.charAt(3));
    //Serial.print("char(4): ");Serial.println(inputString.charAt(4));
    if (inputString.charAt(0)==soyArduino)
    {
      String datos="";
      //Serial.print("Peticion aceptada: ");Serial.println(inputString);
      char valor=inputString.charAt('4');
      switch(inputString.charAt(2))
      {
        case leeSensores:
        {
          datos="";
          datos+=soyArduino;
          datos+=':';
          datos+=sensorAuthRfid;
          datos+='-';
          if (!chkCodigoNoLeido(c0,c1,c2,c3))
          {
            datos+=c0;
            datos+='*';
            datos+=c1;
            datos+='*';
            datos+=c2;
            datos+='*';
            datos+=c3;
          }
          else
          {
            datos+=respuestaCodigoNoLeido;
          }
          Serial.println(datos);
          break; 
        }//case leeSensores
        case actuadorDoor:
        {
          //Serial.print("Peticion actuador: ");Serial.println(inputString);
          switch (inputString.charAt(4))
          {
            datos="";
            datos+=soyArduino;
            datos+=':';
            case '0':
            {
              datos += "Off;";
              Serial.println(datos);
              setAuthNOk();
              delay(tiempoMantenerEstado_Auth_Ok_NOk);
              setInicial();
              break;
            }
            case '1':
            {
              datos += "On;";
              Serial.println(datos);
              setAuthOk();
              delay(tiempoMantenerEstado_Auth_Ok_NOk);
              setInicial();
              break;
            }
            default:
            {
              //Serial.println("Orden de actuador desconocida: "+inputString);
              break;
            }
          }
          break;
        }//case actuadorDoor
        default:
        {
          //Serial.println("Actuador desconocido: "+inputString);
          break;
        }
      }//switch charAt 2
    }//if soyArduino
    //setInicial();
    stringComplete=false;
    inputString="";
  }//if stringComplete
}
void serialEvent() {
  //Serial.println("serial event");
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
      //Serial.print("inputString: ");Serial.println(inputString);
    }
  }
}
bool chkCodigo(int c0, int c1, int c2, int c3)
{
  authOk=false;
  if ((c0==252) && (c1==69) && (c2==53) &&(c3==109)) //Llavero L1 Rojo 
  {
    authOk=true;
  }
  else if ((c0==174) && (c1==149) && (c2==247) &&(c3==12)) //
  {
    //authOk=true;
  }
  else if ((c0==76) && (c1==231) && (c2==53) &&(c3==109)) //
  {
    //authOk=true;
  }
  else if ((c0==206) && (c1==121) && (c2==97) &&(c3==241)) //
  {
    //authOk=true;
  }
  return authOk;
}
bool chkCodigoNoLeido(int c0, int c1, int c2, int c3)
{
  bool resultado=false;
  if ((c0==0) && (c1==0) && (c2==0) && (c3==0))
  {
    resultado=true;
  }
  return resultado;
}
void setInicial()
{
  tLeidos=0;
  tiempo1_Inic=-1;
  tiempo1_Actual=-1;
  tiempo1_Activo=false;

  authOk=false;
  
  c0=0;
  c1=0;
  c2=0;
  c3=0;

  stringComplete=false;
  inputString="";
  
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdLeyendo,LOW);
  digitalWrite(pdCodigoNOk,LOW);
  digitalWrite(pdCodigoOk,LOW);
}
void setLeyendo()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdLeyendo,HIGH);
  digitalWrite(pdCodigoNOk,LOW);
  digitalWrite(pdCodigoOk,LOW);
}
void setAuthNOk()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdLeyendo,LOW);
  digitalWrite(pdCodigoNOk,HIGH);
  digitalWrite(pdCodigoOk,LOW);
}
void setAuthOk()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdLeyendo,LOW);
  digitalWrite(pdCodigoNOk,LOW);
  digitalWrite(pdCodigoOk,HIGH);
}
void setError()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdLeyendo,HIGH);
  digitalWrite(pdCodigoNOk,HIGH);
  digitalWrite(pdCodigoOk,HIGH);
}

