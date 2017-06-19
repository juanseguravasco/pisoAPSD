#define soyArduino '8'
#define leeSensores '0'
#define sensorPosicionMotor '2'
#define actuadorMotor '9'

#define ordenSubirTotal '0' //Abrir
#define ordenSubir1pos '1'
#define ordenBajar1pos '3' //Cerrar
#define ordenBajarTotal '4'
#define noOrden '9'

#define pinSubir '10'
#define pinBajar '11'

String inputString="";
boolean stringComplete=false;
String respuestaCodigoNoLeido="n";
int orden=noOrden;

int paSubir=0;
int paBajar=1;

int pdActivo=9;
int pdSubiendoMotor=10;
int pdBajandoMotor=11;
int pdActivaMotor=6;

int presSubirValue=0;
int presBajarValue=0;
int subirValue=0;
int bajarValue=0;
int presPulsarValue=800;
int presNoPulsarValue=100;

int posicion=1; //1=arriba/abierto,..,4=abajo
int posicionArriba=1;
int posicionAbajo=5;
bool enviaPosicion=false;

//tiempo1:tiempo movimiento motor para llegar a siguiente posición
int tiempo1_base=4000; //En milisegons
int tiempo1_max=4000; //En milisegons
int tiempo1_inicial=-1;
int tiempo1_actual=-1;
bool tiempo1_activo=false;

void setup() {
  Serial.begin(9600);
  pinMode(pdActivo, OUTPUT);
  pinMode(pdSubiendoMotor, OUTPUT);
  pinMode(pdBajandoMotor, OUTPUT);
  pinMode(pdActivaMotor, OUTPUT);
  setInicio();
}

void loop() {
  //Serial.print("tiempo activo:");Serial.println(tiempo1_activo);
  if (tiempo1_activo)
  {
    tiempo1_actualizar();
  }
  if (tiempo1_vencido())
  {
    tiempo1_stop(); //Parar el temps
    tiempo1_acciones();
  }
  
  orden=noOrden;
  enviaPosicion=false;
  if (stringComplete)
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
          datos+=sensorPosicionMotor;
          datos+='-';
          datos+=posicion;
          datos+=";";
          Serial.println(datos);
          break; 
        }
        case actuadorMotor:
        {
          switch(inputString.charAt(4))
          {
            case ordenSubirTotal:
            {
             orden=ordenSubirTotal;
             break;
            }
            case ordenBajarTotal:
            {
             orden=ordenBajarTotal;
             break;
            }
            case ordenSubir1pos:
            {
             orden=ordenSubir1pos;
             break;
            }
            case ordenBajar1pos:
            {
             orden=ordenBajar1pos;
             break;
            }
            default:
            {
             orden=noOrden;
             break;
            }
          }//switch char at 4
        }//case actuador motor
      }//switch char at 2
    }//if soy arduino
    if (orden!=noOrden)
    {
      enviaPosicion=true;
    }
    stringComplete=false;
    inputString="";
  }//stringcomplete

  if (orden==noOrden)//Tiene prioridad la orden de la Rasp
  {
    presSubirValue=analogRead(paSubir);
    //subirValue=map(presSubirValue,0,1023,0,255);
    presBajarValue=analogRead(paBajar);
    //bajarValue=map(presBajarValue,0,1023,0,255);

    //Serial.print("presSubir:");Serial.println(presSubirValue);
    //Serial.print("presBajar:");Serial.println(presBajarValue);
    //Serial.print("Subir:");Serial.println(subirValue);
    //Serial.print("Bajar:");Serial.println(bajarValue);
  
    if ((presBajarValue>presPulsarValue) and (presSubirValue>presPulsarValue))
    {
      orden=ordenSubir1pos;
    }
    if ((presSubirValue>presPulsarValue) and (presBajarValue<presNoPulsarValue))
    {
      orden=ordenSubir1pos;
    }
    if ((presBajarValue>presPulsarValue) and (presSubirValue<presNoPulsarValue))
    {
      orden=ordenBajar1pos;
    }
  } //if orden != noOrden

  if (!tiempo1_activo)
  {
    if (orden==ordenSubir1pos)
    {
      subir1pos();  
    }
    if (orden==ordenBajar1pos)
    {
      bajar1pos();
    }
    if (orden==ordenSubirTotal)
    {
      subirTotal();
    }
    if (orden==ordenBajarTotal)
    {
      bajarTotal();
    }
    if (enviaPosicion)
    {
      enviarPosicion();
    }
  }
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
void tstLeds()
{
  digitalWrite(pdActivo,HIGH);
  digitalWrite(pdSubiendoMotor,HIGH);
  digitalWrite(pdBajandoMotor,HIGH);
  digitalWrite(pdActivaMotor,HIGH);
}
void setInicio()
{
  pararMotor();
  digitalWrite(pdActivo,HIGH);
  posicion=1;
}
void pararMotor()
{
  //Serial.println("Parar motor...");
  digitalWrite(pdActivaMotor,LOW);
  digitalWrite(pdSubiendoMotor,LOW);
  digitalWrite(pdBajandoMotor,LOW);
}
void enviarPosicion()
{
  String datos="";
  datos+=soyArduino;
  datos+=':';
  datos += posicion;
  datos += ";";
  Serial.println(datos);
}
void subir1pos()
{
  if (posicion>posicionArriba)
  {
    posicion--;
    setDireccionSubir();
    arrancaMotor();
    tiempo1_start(tiempo1_base);
  }
}
void subirTotal()
{
  int tiempo_mx=0;
  if (posicion>posicionArriba)
  {
    tiempo_mx=(posicion-posicionArriba)*tiempo1_base;
    posicion=posicion-(posicion-posicionArriba);

    setDireccionSubir();
    arrancaMotor();
    tiempo1_start(tiempo_mx);
  }
}
void bajar1pos()
{
  if (posicion<posicionAbajo)
  {
    posicion++;
    setDireccionBajar();
    arrancaMotor();
    tiempo1_start(tiempo1_base);
  }
}
void bajarTotal()
{
  int tiempo_mx=0;
  if (posicion<posicionAbajo)
  {
    tiempo_mx=(posicionAbajo-posicion)*tiempo1_base;
    posicion=posicion+(posicionAbajo-posicion);

    setDireccionBajar();
    arrancaMotor();
    tiempo1_start(tiempo_mx);
  }
}
void setDireccionSubir()
{
  digitalWrite(pdSubiendoMotor,HIGH);
  digitalWrite(pdBajandoMotor,LOW);
}
void setDireccionBajar()
{
  digitalWrite(pdSubiendoMotor,LOW);
  digitalWrite(pdBajandoMotor,HIGH);
}
void arrancaMotor()
{
  //Serial.println("Arranca motor...");
  digitalWrite(pdActivaMotor,HIGH);
}
void tiempo1_acciones()
{
  pararMotor();
}
void tiempo1_stop()
{
  tiempo1_activo=false;
  tiempo1_inicial=-1;
  tiempo1_actual=-1;
}
void tiempo1_start(int tiempo_mx)
{
  if (!tiempo1_activo)
  {
    tiempo1_max=tiempo_mx;
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

