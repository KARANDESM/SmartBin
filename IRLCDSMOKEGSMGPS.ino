#include <SoftwareSerial.h>
#include "TinyGPS.h"
TinyGPS gps;
long startMillis;
SoftwareSerial mySerial(9, 8);// rx of gsm ==>8  tx of gsm ==>9
//define flags which desides whether to send sms or not
//l0   ==> level below 60
//l1   ==> 60
//l2   ==> 70
//l3   ==> 80
//f    ==> for fire
//done ==> send status
bool fdone = true;


//sms contents are
//WT:   ==> for weight
//L:    ==> for level
//F:    ==> for fire
//LAT:  ==> for latitude
//LON:  ==> for longitude
float lat = 0.0, lon=0.0;
String finalsms = "SMK";
char name[] = "SMART BIN ALERT";
char name1[] = "SYSTEM !!";
//pin 22 for 60%
//pin 23 for 70%
//pin 24 for 80%
//pin 41 for buzzer
int l1, l2,l3,sm,buzz;
char level[]="LEVEL:";
char smoke[]="FIRE:";
char wt[]="Wt:";
char wtt[]="1";
char level1[] = "60";
char level2[] = "70";
char level3[] = "80";
char level0[]=  "<60";
char pr[] = "Y";
char ab[] = "N";
void setup()
{
  mySerial.begin(9600);
  Serial1.begin(9600);
  Serial.begin(9600);

  pinMode(41, OUTPUT);
  pinMode(9, INPUT);
  pinMode(8, OUTPUT);
  pinMode(53, OUTPUT);
  pinMode(52, OUTPUT);
  pinMode(22, INPUT);
  pinMode(23, INPUT);
  pinMode(24, INPUT);  
  pinMode(25, INPUT);
  DDRL=B11111111;
  PORTL=B00000000;
  init_lcd();
  delay(20);
  lcd_write_string(name);
  lcd_cmd(0xC0);
  lcd_write_string(name1);
  //digitalWrite(41, HIGH);
  delay(1500);
  readLoc();
  delay(500);/*
  lcd_cmd(0x01);
  lcd_data(lat);
  lcd_cmd(0xC0);
  lcd_data(lon);
  delay(2000);*/
  //digitalWrite(41, LOW);
 }
  void loop()
  {
    
    digitalWrite(41, LOW);
    delay(3000);
  
    l1=digitalRead(22);
    l2=digitalRead(23);
    l3=digitalRead(24);
    sm=digitalRead(25);
    
     if(!l3)
    {
            if(!sm)
                
                {
                      lcd(level3,wtt,pr);
                      stat(3, wtt, 1,lat, lon);
                }
            else
         
                {
                      lcd(level3,wtt,ab);
                      stat(3, wtt, 0,lat, lon);
                }
          
           delay(20);
    }
    
    else if(((!l2) && (!l1))== HIGH)
    {
      
                      if(!sm)
                      {
                        lcd(level2,wtt,pr);
                        stat(2, wtt, 1,lat, lon);
                      }
                      else
                      {
                        lcd(level2,wtt,ab);
                        stat(3, wtt, 0,lat, lon);
                        
                      }
            
            delay(20);
      }

    
    else if(!l1)
    {
                        
                        if(!sm)
                        {
                          lcd(level1,wtt,pr);
                          stat(1, wtt, 1,lat, lon);
                          
                        }
                        else
                        {
                          lcd(level1,wtt,ab);
                        }
          delay(200);
    }
    
    else /*if((l3 && l2 && l1))*/
    {
                              
                              if(!sm)
                              {
                                lcd(level0,wtt,pr);
                                stat(0, wtt, 1,lat, lon);
                              }
                              else
                              {
                                lcd(level0,wtt,ab);
                              }
     
      delay(200);
    }
  //  delay(5000); 
  }
  void lcd_cmd(char c)
  {
    PORTL=c;
    digitalWrite(53, LOW);
    digitalWrite(52, HIGH);
    delay(25);
    digitalWrite(52, LOW);
    delay(20);
  }
  void init_lcd()
  {
    lcd_cmd(0x38);
    delay(20);
    lcd_cmd(0x0E);
    delay(20);
    lcd_cmd(0x01);
    delay(20);
    lcd_cmd(0x80);
    delay(20);  
  }
  void lcd_data(char d)
  {
    PORTL=d;
    digitalWrite(53, HIGH);
    digitalWrite(52, HIGH);
    delay(25);
    digitalWrite(52, LOW);
    delay(20);
  }

  void lcd_write_string(char * str)
  {
    int i=0;
    while(str[i]!='\0')
    {
      lcd_data(str[i]);
      delay(20);
      i++;
    }
  }
  void lcd(char * lv, char * w, char * smk)
  {
    char *p = lv;
    char *q = w;
    char *r = smk;

    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_write_string(level);
    lcd_cmd(0x86);
    lcd_write_string(p);
    lcd_cmd(0x8B);
    lcd_write_string(wt);
    lcd_cmd(0x8E);
    lcd_write_string(q);
    lcd_cmd(0xC0);
    lcd_write_string(smoke);    
    lcd_cmd(0xC7);
    lcd_write_string(r);
    delay(1000);
  }

void stat(int lvl, int wgh, int fr, float lt, float ln)
{
    int f = fr;
    int l=lvl;
    int wg=wgh;
    float latt=lt;
    float longt=ln;
    
    
    String finalsms = String("L:" + String(l)+ "WT:" + String( wg) + "LAT:" + String(latt) + "LON:" + String(longt) + "F:" + String(f));
    /*Serial.println(lat);
    Serial.println(lon);*/
    
    delay(1000);
        
    if(l>=2 || f==1)
    {
      if(f == 1){ 
    digitalWrite(41, HIGH);
      }
      else
      {}
      if(fdone)
      {
      SendMessage(finalsms);
      fdone=false;
      }
      else
      {
        
      }
    }
 }
  
 void SendMessage(String msg)
{
  String s = String(msg);
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+917620176104\"\r"); 
  delay(1000);
  mySerial.println(msg);
  delay(100);
   mySerial.println((char)26);
  delay(1000);
}
void readLoc()
{
  bool newData = false;
  unsigned long chars = 0;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial1.available())
    {
      int c = Serial1.read();
      ++chars;
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
  
  if (newData)
  {
       
    unsigned long age;
    gps.f_get_position(&lat, &lon, &age);
    
    lat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lat;
    lon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lon;
Serial.println(lat, 6);
Serial.println(lon, 6);    
  }
  }


