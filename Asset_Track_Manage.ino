#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS mygps;
SoftwareSerial gps(4, 5);
SoftwareSerial gsm(2, 3);

int sensor = A1;
float gpslat, gpslon;
float temp, temp_alert_val, Temp_shut_val;
int sms_count = 0, fire;


void setup()
{
  pinMode(sensor,INPUT);
  
  gsm.begin(9600);
  gps.begin(9600); 
    
  Serial.begin(9600);      
  delay(500);
}

void loop()
{
  CheckFire();
  CheckShutDown();
  SendLocation();
}

void CheckFire()
{
  Temp_alert_val = CheckTemp();
  if(Temp_alert_val>45)
  {
    SetAlert();
  }
}

float CheckTemp()
{
  temp = analogRead(sensor);
  temp = temp * 5;   
  temp = temp / 10;
  return temp;
}

void SetAlert()
{
  while(sms_count<3) 
  {  
    SendTextMessage();
  }
  fire = 1; 
}

void CheckShutDown()
{
  if(fire == 1)
  {
    Temp_shut_val = CheckTemp();
    if(Temp_shut_val < 28)
    {
      sms_count=0;
      fire = 0;
    }
   }
}

void SendTextMessage()
{
  gsm.println("AT+CMGF=1");
  delay(2000);
  gsm.println("AT+CMGS=\"+911234567890\"\r");
  delay(2000);
  gsm.println("High Temperature(Fire Alert)");
  delay(200);
  gsm.println((char)26);
  delay(5000);
  gsm.print("Latitude :");
  gsm.println(gpslat, 6);
  gsm.print("Longitude:");
  gsm.println(gpslon, 6);
  delay(1000);
  gsm.write(0x1A);
  sms_count++;
}

void SendLocation()
{
  gps.listen();
  while (gps.available())
  {
    int num = gps.read();
    if (mygps.encode(num))
    {
      mygps.f_get_position(&gpslat, &gpslon);
    }
  }
  gsm.listen();
    if (gsm.available() > 0) 
    {
      String in = gsm.readString();
      in.trim();
      if (in.indexOf("Track Location") >= 0) 
      {
        gsm.print("\r");
        delay(1000);
        gsm.print("AT+CMGF=1\r");
        delay(1000);

        gsm.print("AT+CMGS=\"+911234567890\"\r");
        delay(1000);

        gsm.print("Latitude :");
        gsm.println(gpslat, 6);
        gsm.print("Longitude:");
        gsm.println(gpslon, 6);
        delay(1000);
        gsm.write(0x1A);
        delay(1000);
      }
  delay(100);
}
