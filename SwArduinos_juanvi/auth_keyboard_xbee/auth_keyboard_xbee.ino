#include <Keypad.h>

#define soyArduino 'B'
#define leeSensores '0'
#define sensorAuthTeclado '1'
#define actuadorDoor '9'
#define pinDoor '10'

const byte tRows=4;
const byte tCols=3;
const byte lenCodigo=4;

char keymap[tRows][tCols]=
{
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};

byte rowPins[tRows]={8,7,6,5}; //Rows 0 to 3
byte colPins[tCols]={4,3,2}; //Cols 0 to 2
byte tLeidos=0;
int pdActivo=9;
int pdCodigoOk=10;
int pdCodigoNok=11;
int pdLeyendo=12;

char codigoLeido[lenCodigo+1]="";
char codigoValido1[lenCodigo+1]="1235";
char codigoValido2[lenCodigo+1]="7890";
char codigoLocalValido1[lenCodigo+1]="1379";
char codigoNoLeido[lenCodigo+1]="nnnn";
bool authOk=false;
int tiempoMantenerEstado_Auth_Ok_NOk=4000;
int tiempo1_Max=8000; //Milisegons màx. entre pulsacions de tecles.
int tiempo1_Actual=-1;
int tiempo1_Inic=-1;
bool tiempo1_Activo=false;

String inputString="";
boolean stringComplete=false;
String respuestaCodigoNoLeido="n";

Keypad myKeypad=Keypad(makeKeymap(keymap),rowPins,colPins,tRows,tCols);

char keyBorrar='*';

void setup() {
  Serial.begin(9600);
  //Serial.println("Iniciado...");
  pinMode(pdActivo, OUTPUT);
  pinMode(pdLeyendo, OUTPUT);
  pinMode(pdCodigoNok, OUTPUT);
  pinMode(pdCodigoOk, OUTPUT);
  setInicial();
}

void loop() {
  char keyPressed=NO_KEY;
  
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
    keyPressed=keyBorrar;
  }
  else
  {
    keyPressed=myKeypad.getKey();  
  }
  
  if (keyPressed==keyBorrar)
  {
    setInicial();
  }
  else if (keyPressed!=NO_KEY)
  {
    tiempo1_Inic=tiempo1_Actual;
    if ((tLeidos>=0) && (tLeidos<lenCodigo))
    {
      codigoLeido[tLeidos]=keyPressed;
      tLeidos++;
      if (tLeidos==1)
      {
        setLeyendo();
      }
    }
    if (tLeidos==lenCodigo)
    {
      authOk=chkCodigo(codigoLeido);
      if (authOk)
      {
        setAuthOk();
        delay(tiempoMantenerEstado_Auth_Ok_NOk);
        setInicial();
      }
      else
      {
        //setAuthNOk();
        //delay(tiempoMantenerEstado_Auth_Ok_NOk);
        //setInicial();
      }
    }
  }

  //id - id - id
  // 0 1  2 3  4  
  //Rasp a Arduino
  //idArduino-leeSensores-
  //idArduino-idActuador-Orden
  //Arduino a Rasp
  //idArduino:idSensor1-valorSensor1:idSensor2-valorSensor2...
  
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
      //Serial.print("Peticion aceptada: ");Serial.println(inputString);
      char valor=inputString.charAt('4');
      switch(inputString.charAt(2))
      {
        case leeSensores:
        {
          //Serial.print("Peticion leer sensores: ");Serial.println(inputString);
          
          String datos="";
          datos+=soyArduino;
          datos+=':';
          datos+=sensorAuthTeclado;
          datos+='-';
          if ((tLeidos==lenCodigo) && (!chkCodigoNoLeido(codigoLeido)))
          {
            datos+=codigoLeido[0];
            datos+=codigoLeido[1];
            datos+=codigoLeido[2];
            datos+=codigoLeido[3];
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
            case '0':
            {
              Serial.println("Off");
              setAuthNOk();
              delay(tiempoMantenerEstado_Auth_Ok_NOk);
              setInicial();
              break;
            }
            case '1':
            {
              Serial.println("On");
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
      }//switch inputString char at 2
    }//if inputString soyArduino
    //setInicial();
    stringComplete=false;
    inputString="";
  }//if stringComplete
}
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
      //Serial.print("inputString: ");Serial.println(inputString);
    }
  }
}
bool chkCodigo(char codigoLeido[])
{
  bool resultado=false;
  if (strcmp(codigoLeido,codigoLocalValido1)==0)
  {
    resultado=true;
  }
  
  //if (strcmp(codigoLeido,codigoValido1)==0)
  //{
  //  resultado=true;
  //}
  //if (strcmp(codigoLeido,codigoValido2)==0)
  //{
  //  resultado=true;
  //}
  
  return resultado;
}
bool chkCodigoNoLeido(char codigoLeido[])
{
  bool resultado=false;
  if (strcmp(codigoLeido,codigoNoLeido)==0)
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

  codigoLeido[0]='n';
  codigoLeido[1]='n';
  codigoLeido[2]='n';
  codigoLeido[3]='n';

  stringComplete=false;
  inputString="";
  
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdLeyendo,LOW);
  digitalWrite(pdCodigoNok,LOW);
  digitalWrite(pdCodigoOk,LOW);
}
void setLeyendo()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdLeyendo,HIGH);
  digitalWrite(pdCodigoNok,LOW);
  digitalWrite(pdCodigoOk,LOW);
}
void setAuthNOk()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdLeyendo,LOW);
  digitalWrite(pdCodigoNok,HIGH);
  digitalWrite(pdCodigoOk,LOW);
}
void setAuthOk()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdLeyendo,LOW);
  digitalWrite(pdCodigoNok,LOW);
  digitalWrite(pdCodigoOk,HIGH);
}
void setError()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdLeyendo,HIGH);
  digitalWrite(pdCodigoNok,HIGH);
  digitalWrite(pdCodigoOk,HIGH);
}
