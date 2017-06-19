#include "FPS_GT511C3.h"

#define soyArduino 'A'
#define leeSensores '0'
#define sensorAuthDactilar '1'
#define actuadorDoor '9'
#define pinDoor '10'

int pdAct=9;
int pdOk=10;
int pdNOk=11;
int pdLey=12;

int pdModoLocalTotal=7;

int delaySiguienteLectura=100;
int mnCodigoLeido=0;
int mxCodigoLeido=199;
int codigoLeido=-1;
bool authOk=false;

bool modoLocalTotal=false;
int modoLocalTotalPinValue=0;

int codigoValido1=0; //juanvi índice derecho
int codigoValido2=1; //juanvi corazón derecho
int codigoLocalValido1=2; //juanvi índice izquierdo
int codigoLocalValido2=3; //juanvi corazón izquierdo
int codigoNoLeido=-1;

int tiempoMantenerEstado_Auth_Ok_NOk=2000;

byte tLeidos=0;

int tiempo1_Max=8000; //Milisegons màx. que se guarda el código leido para enviar a Rasp.
int tiempo1_Actual=-1;
int tiempo1_Inic=-1;
bool tiempo1_Activo=false;

String inputString="";
boolean stringComplete=false;
String respuestaCodigoNoLeido="n";

FPS_GT511C3 fps(2, 3);

void setup()
{
	Serial.begin(9600);
  //Serial.println("Iniciado...");
	delay(100);
	fps.Open();
	fps.SetLED(true);
  pinMode(pdAct, OUTPUT);
  pinMode(pdLey, OUTPUT);
  pinMode(pdNOk, OUTPUT);
  pinMode(pdOk, OUTPUT);
  pinMode(pdModoLocalTotal, INPUT);
  setInicio();
}

void loop(void)
{
 	delay(delaySiguienteLectura);
  modoLocalTotalPinValue=digitalRead(pdModoLocalTotal);
  if (modoLocalTotalPinValue)
  {
    modoLocalTotal=true;
  }
  else
  {
    modoLocalTotal=false;
  }
  //Serial.println(modoLocalTotalPinValue);

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
    setInicio();
  }
  
	if (fps.IsPressFinger())
	{
    setLeyendo();
		fps.CaptureFinger(false);
		codigoLeido=fps.Identify1_N();
		if (codigoLeido<=mxCodigoLeido)
		{
			//Serial.print("Encontrado. Codigo Leido = ");
			//Serial.println(codigoLeido);
      authOk=chkCodigo(codigoLeido);
      if (authOk)
      {
        setAuthOk();
        delay(tiempoMantenerEstado_Auth_Ok_NOk);
        setInicio();
      }
      else
      {
        tLeidos=1;
        //setAuthNOk();
        //delay(tiempoMantenerEstado_Auth_Ok_NOk);
        //setInicio();
      }
		}
		else
		{
      tLeidos=1;
			//Serial.print("No encontrado. Codigo Leido = ");
      //Serial.println(codigoLeido);
      //setAuthNOk();
		}
	}//if fps_IsPressFinger
	else
	{
		//Serial.println("Poner dedo");
    //setInicio();
	}

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
          //Serial.print("Petición leer sensores: ");Serial.println(inputString);
          String datos="";
          datos+=soyArduino;
          datos+=':';
          datos+=sensorAuthDactilar;
          datos+='-';
          if (!chkCodigoNoLeido(codigoLeido))
          {
            datos+=codigoLeido;
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
              setInicio();
              break;
            }
            case '1':
            {
              Serial.println("On");
              setAuthOk();
              delay(tiempoMantenerEstado_Auth_Ok_NOk);
              setInicio();
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
    //setInicio();
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
bool chkCodigo(int codigo)
{
  bool resultado=false;
  if (modoLocalTotal)
  {
    if ((codigo>=mnCodigoLeido) && (codigo<=mxCodigoLeido))
    {
      resultado=true;
    }
  }
  if (codigo==codigoLocalValido1) //Juanvi Índice izquierdo: 2
  {
    resultado=true;
  }
  if (codigo==codigoLocalValido2) //Juanvi corazón izquierdo: 3
  {
    resultado=true;
  }
  if (codigo==codigoValido1) //Juanvi Índice derecho: 0
  {
    //resultado=true;
  }
  if (codigo==codigoValido2) //Juanvi Corazón derecho: 1
  {
    //resultado=true;
  }
  return resultado;
}
bool chkCodigoNoLeido(int codigoLeido)
{
  bool resultado=false;
  if (codigoLeido==codigoNoLeido)
  {
    resultado=true;
  }
  return resultado;
}
void setInicio()
{
  tLeidos=0;
  tiempo1_Inic=-1;
  tiempo1_Actual=-1;
  tiempo1_Activo=false;

  authOk=false;
  
  codigoLeido=codigoNoLeido;
  
  stringComplete=false;
  inputString="";
  
  digitalWrite(pdAct,HIGH);
  digitalWrite(pdLey,LOW);
  digitalWrite(pdOk,LOW);
  digitalWrite(pdNOk,LOW);
}
void setLeyendo()
{
  digitalWrite(pdAct,HIGH);
  digitalWrite(pdLey,HIGH);
  digitalWrite(pdOk,LOW);
  digitalWrite(pdNOk,LOW);
}
void setAuthOk()
{
  digitalWrite(pdAct,HIGH);
  digitalWrite(pdLey,LOW);
  digitalWrite(pdOk,HIGH);
  digitalWrite(pdNOk,LOW);
}
void setAuthNOk()
{
  digitalWrite(pdAct,HIGH);
  digitalWrite(pdLey,LOW);
  digitalWrite(pdOk,LOW);
  digitalWrite(pdNOk,HIGH);
}
void setError()
{
  digitalWrite(pdAct,HIGH);
  digitalWrite(pdLey,HIGH);
  digitalWrite(pdOk,HIGH);
  digitalWrite(pdNOk,HIGH);
}
