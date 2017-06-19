#include <OneWire.h>
#include <DallasTemperature.h>

#define soyArduino '2'
#define leeSensores '0'
#define sensorHumedadAguaDig '1'
#define sensorCaudalAguaDig '2'
#define sensorTemperaturaAgua '3'
#define actuadorValvula '9'

#define ordenDesactivarValvula '0'
#define ordenActivarValvula '1'
#define ordenActivarDesactivarValvula '2'

int pdMode=2;
bool modeLocal=false;

int pdTemperaturaAgua=5;
int pdHumedadAguaDig=6; 
int pdCaudalAguaDig=7;

int pdActivo=9;
int pdAviso=10;
int pdAmarillo=11;

int humedadAguaDig=0;
int humedadAguaDigValue=0;

int caudalAguaDigTmp=-9;
int caudalAguaDigAct=-9;
int caudalAguaDigAnt=-9;
int caudalAguaDigValue=0;

float temperaturaAgua=0;

int noOrden=9;
int orden=noOrden;

String inputString="";
boolean stringComplete=false;
String respuestaCodigoNoLeido="n";

int tiempoValvulaActiva=1000;

//tiempo1:tiempo 1/0 caudal
int tiempo1_max=100; //En milisegons
int tiempo1_inicial=-1;
int tiempo1_actual=-1;
bool tiempo1_activo=false;

OneWire ourWire(pdTemperaturaAgua);
DallasTemperature sensors(&ourWire);

void setup() {
  Serial.begin(9600);
  pinMode(pdActivo, OUTPUT);
  pinMode(pdAviso, OUTPUT);
  pinMode(pdAmarillo, OUTPUT);
  pinMode(pdMode, INPUT);

  pinMode(pdHumedadAguaDig, INPUT);
  pinMode(pdCaudalAguaDig, INPUT);
  sensors.begin();
  setInicio();
}

void loop() {
//  Serial.print("tiempo activo:");Serial.println(tiempo1_activo);
//  if (tiempo1_activo)
//  {
//    tiempo1_actualizar();
//  }
//  if (tiempo1_vencido())
//  {
//    tiempo1_stop(); //Parar el temps
//    tiempo1_acciones();
//  }
  if (digitalRead(pdMode))
  {
    modeLocal=true;
  }
  else
  {
    modeLocal=false;
  }
  setModeLocal(modeLocal);

  //bajarValue=map(presBajarValue,0,1023,0,255);
  humedadAguaDig=digitalRead(pdHumedadAguaDig);
  humedadAguaDigValue=!humedadAguaDig;
  
  caudalAguaDigTmp=digitalRead(pdCaudalAguaDig);
  if (caudalAguaDigAct!=-9)
  {
    caudalAguaDigAnt=caudalAguaDigAct;
  }
  else
  {
    caudalAguaDigAnt=caudalAguaDigTmp;
  }
  caudalAguaDigAct=caudalAguaDigTmp;
  if (caudalAguaDigAct!=caudalAguaDigAnt)
  {
    caudalAguaDigValue=1;
  }
  else
  {
    caudalAguaDigValue=0;
  }

  sensors.requestTemperatures();
  float temperaturaAgua= sensors.getTempCByIndex(0);
  //delay(100);
  
  //Serial.print("humedad:");Serial.println(humedadAguaDig);
  //Serial.print("caudal:");Serial.println(caudalAguaDigAct);
  //Serial.println("");
  //Serial.print("temperatura:");Serial.println(temperaturaAgua);

  orden=noOrden;

  if (modeLocal)
  {
    if ((humedadAguaDigValue) || (caudalAguaDigValue) || (temperaturaAgua>=26))
    {
      //Serial.print("    humedad:");Serial.println(humedadAguaDigValue);
      //Serial.print(" caudal act:");Serial.println(caudalAguaDigAct);
      //Serial.print(" caudal ant:");Serial.println(caudalAguaDigAnt);
      //Serial.print("temperatura:");Serial.println(temperaturaAgua);
      orden=ordenActivarValvula;
    }
    else
    {
      orden=ordenDesactivarValvula;
    }
  }//if local
  
  if ((stringComplete) && (!modeLocal))
  {
    if (inputString.charAt(0)==soyArduino)
    {
      switch(inputString.charAt(2))
      {
        case leeSensores:
        {
          String datos="";
          datos+=soyArduino;
          datos+=':';
          datos+=sensorTemperaturaAgua;
          datos+='-';
          datos+=temperaturaAgua;
          datos+=':';
          datos+=sensorCaudalAguaDig;
          datos+='-';
          datos+=caudalAguaDigValue;
          datos+=':';
          datos+=sensorHumedadAguaDig;
          datos+='-';
          datos+=humedadAguaDigValue;
          Serial.println(datos);
          break; 
        }
        case actuadorValvula:
        {
          switch(inputString.charAt(4))
          {
            case ordenActivarValvula:
            {
             Serial.println("On");
             orden=ordenActivarValvula;
             break;
            }
            case ordenDesactivarValvula:
            {
             Serial.println("Off");
             orden=ordenDesactivarValvula;
             break;
            }
            default:
            {
             orden=noOrden;
             break;
            }
          }//switch char at 4
          break;
        }//case actuador
      }//switch char at 2
    }//if soy arduino
    stringComplete=false;
    inputString="";
  }//if stringcomplete
  if (orden==ordenActivarValvula)
  {
    //Serial.println("activar");
    activaValvula(false);
    delay(tiempoValvulaActiva);
  }
  if (orden==ordenActivarDesactivarValvula)
  {
    //Serial.println("activardesactivar");
    activaValvula(true);
  }
  if(orden==ordenDesactivarValvula)
  {
    //Serial.println("desactivar");
    desactivaValvula();
  }
}//loop

void serialEvent() {
  if (!modeLocal)
  {
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
}//serialEvent
void tstLeds()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdAmarillo,HIGH);
  digitalWrite(pdAviso,HIGH);
}
void setModeLocal(bool local)
{
  if (local)
  {
    digitalWrite(pdAmarillo, HIGH);
  }
  else
  {
    digitalWrite(pdAmarillo, LOW);
  }
}
void setInicio()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdAviso, LOW);
  if (modeLocal)
  {
    digitalWrite(pdAmarillo, HIGH);
  }
  else
  {
    digitalWrite(pdAmarillo, LOW); 
  }
}
void activaValvula(bool desactivar)
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdAviso, HIGH);
  if (modeLocal)
  {
    digitalWrite(pdAmarillo, HIGH);
  }
  else
  {
    digitalWrite(pdAmarillo, LOW); 
  }
  if (desactivar)
  {
    delay(tiempoValvulaActiva);
    desactivaValvula();
  }
}
void desactivaValvula()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdAviso, LOW);
  if (modeLocal)
  {
    digitalWrite(pdAmarillo, HIGH);
  }
  else
  {
    digitalWrite(pdAmarillo, LOW); 
  }
}
void tiempo1_acciones()
{
 
}
void tiempo1_stop()
{
  tiempo1_activo=false;
  tiempo1_inicial=-1;
  tiempo1_actual=-1;
}
void tiempo1_start()
{
  if (!tiempo1_activo)
  {
    tiempo1_activo=true;
    tiempo1_inicial=millis();
    tiempo1_actual=tiempo1_inicial;
  }
}
void tiempo1_actualizar()
{
  tiempo1_actual=millis();
}
bool tiempo1_vencido()
{
  bool vencido=false;
  if ((tiempo1_actual-tiempo1_inicial)>tiempo1_max)
  {
    vencido=true;
  }
  return vencido;
}

