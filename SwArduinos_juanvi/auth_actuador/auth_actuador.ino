const int pWhite=3;
const int pRed=4;
const int pGreen=5;

const int pTeclado=6;
const int pRfid=7;
const int pDactilar=8;

int auth_Teclado_Ok=0;
int auth_Rfid_Ok=0;
int auth_Dactilar_Ok=0;

int tiempo1_Max=4000; //Milisegons màx. entre pulsacions de tecles.
int tiempo1_Actual=-1;
int tiempo1_Inic=-1;
bool tiempo1_Activo=false;

bool estadoAuthOk=false;

void setup() {
  pinMode(pWhite, OUTPUT);
  pinMode(pRed, OUTPUT);
  pinMode(pGreen, OUTPUT);

  pinMode(pTeclado, INPUT);
  pinMode(pRfid, INPUT);
  pinMode(pDactilar, INPUT);

  digitalWrite(pWhite, HIGH);
  setEstadoAuthNOk();
}

void loop() {
  if ((estadoAuthOk) && (!tiempo1_Activo))
  {
    tiempo1_Inic=millis();
    tiempo1_Activo=true;
  }
  if ((estadoAuthOk) && (tiempo1_Activo))
  {
    tiempo1_Actual=millis();
  }
  if ((estadoAuthOk) && tiempo1_Activo && ((tiempo1_Actual-tiempo1_Inic)>tiempo1_Max))
  {
    setEstadoAuthNOk();
    tiempo1_Activo=false;
  }

  if (!tiempo1_Activo)
  {
    auth_Teclado_Ok=digitalRead(pTeclado);
    auth_Rfid_Ok=digitalRead(pRfid);
    auth_Dactilar_Ok=digitalRead(pDactilar);

    if ((auth_Teclado_Ok) || (auth_Rfid_Ok) || (auth_Dactilar_Ok))
    {
      setEstadoAuthOk();    
    }
    else
    {
      setEstadoAuthNOk();
    }
  } 
}

void setEstadoAuthOk()
{
  digitalWrite(pRed,LOW);
  digitalWrite(pGreen,HIGH);
  estadoAuthOk=true;
}

void setEstadoAuthNOk()
{
  digitalWrite(pRed,HIGH);
  digitalWrite(pGreen,LOW);
  estadoAuthOk=false;
}

