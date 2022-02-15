#include <EEPROM.h>

String id="11001100";
int eepromLen;
unsigned long periods=0;
unsigned long sendPeriods=300;

void setup() 
{
  Serial.begin(9600);
  pinMode(A1,INPUT);//receiver

  pinMode(7,OUTPUT);//buzzer
  pinMode(13,OUTPUT);//transmitter
  pinMode(12,OUTPUT);//led
  eepromLen = EEPROM.read(0);
  Serial.println(eepromLen);
  digitalWrite(12,HIGH);
}

void loop() 
{ 

  while(!(decodeBit(sendPeriods)))
  {
    Serial.println("waiting");
    dataStream(id);
  }

  readBit();

  //save to eeprom
}
//Half Duplex
void readBit() //reads the byte of information and transmit its own byte
{
  bool sync = false;
  int count =0;
  while(!sync && count <=5)
  {
    sync = syncBit();
  }
  if(count >=5)
  {
    for(int x = 0; x<2;x++)
    {
      Serial.println("writing");
      dataStream(id);
    }
    return;
  }
  
  int arr[] = {0,0,0,0,0,0,0,0};
  Serial.println("reading");
  for(int x = 0; x < 8; x++)
  {
    arr[x] = decodeBit(periods);
  }

  digitalWrite(12,LOW);
  tone(7,600);
  delay(50);
  noTone(7);
  digitalWrite(12,HIGH);
  delay(50);
  tone(7,1000);
  digitalWrite(12,LOW);
  delay(100);
  noTone(7);
  digitalWrite(12,HIGH);
  
  for(int x  = 0; x < 8;x++)
  {
    EEPROM.write(eepromLen, arr[x]);
    Serial.print(arr[x]);
    eepromLen+=1;
  }
  EEPROM.write(0,eepromLen);
  Serial.println("");

  for(int x = 0; x<2;x++)
    {
      Serial.println("writing");
      dataStream(id);
    }
}
//READ
bool decodeBit(unsigned long period) //function to get a bit with sample time of period
{
  unsigned long now = millis();
  int count = 0;
  
  while(millis()<now+period)
  {
    int val = analogRead(A1);
    //Serial.println(val);
    if (val > 0)
    {
      count = 0;
    }
    else
    {
      count++;
    }
  }

  if (count >= 40)
  {
    return false;
  }
  return true;
}

bool syncBit() //syncs the transmitter and reciever
{
  int count = 0;

  unsigned long now = millis();
  while(count < 40)
  {
    int val = analogRead(A1);
    //Serial.println(val);
    if (val > 0)
    {
      count = 0;
    }
    else
    {
      count++;
    }
  }
  
  periods = (millis() - now)/3.1;
  if(periods <= 200)
  {
    return false;
  }
  now = millis();
  Serial.println(periods);
  
  if (decodeBit(periods))
  {
    return false;
  }
  if (!(decodeBit(periods*2)))
  {
    return false;
  }
  if (decodeBit(periods))
  {
    return false;
  }
  if (!(decodeBit(periods)))
  {
    return false;
  }
  return true;
}

//WRITE
void startBit()
{
  highBit(sendPeriods);
  highBit(sendPeriods);
  highBit(sendPeriods);
  lowBit(sendPeriods);
  highBit(sendPeriods);  
  highBit(sendPeriods);
  lowBit(sendPeriods);
  highBit(sendPeriods);
}

void endBit()
{
  lowBit(sendPeriods);
  lowBit(sendPeriods);
  lowBit(sendPeriods);
  lowBit(sendPeriods);
}


void dataStream(String id)
{
  startBit();
  int len = id.length();

  for(int x = 0; x < len; x++)
  {
    if(id.charAt(x) == '0')
    {
      lowBit(sendPeriods);
    }
    else
    {
      highBit(sendPeriods);
    }
  }
  endBit();
}

void highBit(unsigned long period)
{
  unsigned long now = millis();
  while(millis() < now + period)
  {
    tone(13,40000);
  }
  noTone(13);
}

void lowBit(unsigned long period)
{
  unsigned long now = millis();
  while(millis() < now + period)
  {
  }
}
