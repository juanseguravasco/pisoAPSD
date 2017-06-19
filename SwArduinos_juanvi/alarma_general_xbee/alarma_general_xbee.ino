#define soyArduino '9'
#define leeSensores '0'
#define actuadorAlarmaGeneral '9'
#define pinAlarmaGeneral '10'

int pdAct=9;
int pdAlarmaOn=10;

String inputString="";
boolean stringComplete=false;
String respuestaCodigoNoLeido="n";

void setup()
{
	Serial.begin(9600);
  //Serial.println("Iniciado...");
  pinMode(pdAct, OUTPUT);
  pinMode(pdAlarmaOn, OUTPUT);
  setInicio();
}

void loop(void)
{
  if (stringComplete)
  {
    if (inputString.charAt(0)==soyArduino)
    {
      char valor=inputString.charAt('4');
      switch(inputString.charAt(2))
      {
        case leeSensores:
        {
          break; 
        }//case leeSensores
        case actuadorAlarmaGeneral:
        {
          String datos="";
          datos += soyArduino;
          datos += ":";
          switch (inputString.charAt(4))
          {
            case '0':
            {
              datos += "Off;";
              Serial.println(datos);
              setAlarmaOff();
              break;
            }
            case '1':
            {
              datos += "On;";
              Serial.println(datos);
              setAlarmaOn();
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
      //  ºSerial.print("inputString: ");Serial.println(inputString);
      //Serial.print("0-0-n");Serial.println("0-0-n");
    }
  }
}
void setInicio()
{
  stringComplete=false;
  inputString="";
  
  digitalWrite(pdAct,HIGH);
  digitalWrite(pdAlarmaOn,LOW);
}
void setAlarmaOn()
{
  digitalWrite(pdAct,HIGH);
  digitalWrite(pdAlarmaOn,HIGH);
}
void setAlarmaOff()
{
  digitalWrite(pdAct,HIGH);
  digitalWrite(pdAlarmaOn,LOW);
}
void setError()
{
  digitalWrite(pdAct,LOW);
  digitalWrite(pdAlarmaOn,LOW);
}
