
/*
 * Created On    : 01/07/2021
 * Developed by  : Muhammed Nabeel
 * Last Update   : 11/07/2021
 * Revision      : 011
 * Contact       : nabeelin1994@gmail.com
 * 
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <elapsedMillis.h>

int signalPin = 11;
String str;
//int oldmos=-1;
LiquidCrystal_I2C lcd(0x27,16,2);  // if your LCD I2C address is not 0x27 , you should replace  0x27  with correct I2C address(i.e 0x3f)
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int flag_hr, flag_min, flag_sec ,flag_rot,flag_blink;


String inputString;
long inputInt;
int Rotation_Value;

//Variable initialisation to update time 
int SecsCurrent, MinutesCurrent, HoursCurrent;

//Remaining time
int rem_hr, rem_min;

//Variable initialisation to store first feeder time  
int SecsFeeder_1, MinutesFeeder_1, HoursFeeder_1;

//Variable initialisation to store secound feeder time 
int SecsFeeder_2, MinutesFeeder_2, HoursFeeder_2;

//Variable initialisation to store 3rd feeder time 
int SecsFeeder_3, MinutesFeeder_3, HoursFeeder_3;

//static unsigned long time_store;
 elapsedMillis TotalRuntime;
 elapsedMillis MotorRuntime;
void setup()
{ 
   Serial.begin(9600);
   pinMode(signalPin, OUTPUT);
   lcd.init();    // initialize the lcd
   lcd.backlight();
   
   flag_blink=0;
   
   InputFail: //Goto Loop
   
   lcd.clear();
   lcd.setCursor(2,0);
   lcd.print("Current Time");
   lcd.setCursor(3,1);
   Time_input(HoursCurrent,MinutesCurrent,SecsCurrent);
   
   if(HoursCurrent>23 || MinutesCurrent>60 || SecsCurrent>60 )
   {
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Wrong Input.. !");
   lcd.setCursor(0,1);
   lcd.print("INPUT AGAIN !");
   delay(3000);
   goto InputFail;
   }
   

   lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("Feeder Time-1");
   lcd.setCursor(3,1);
   Time_input(HoursFeeder_1, MinutesFeeder_1, SecsFeeder_1);
   
   if(HoursFeeder_1>23 || MinutesFeeder_1>60 || SecsFeeder_1>60 )
   {
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Wrong Input.. !");
   lcd.setCursor(0,1);
   lcd.print("INPUT AGAIN !");
   delay(3000);
   goto InputFail;

   }
   
   
   lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("Feeder Time-2");
   lcd.setCursor(3,1);
   Time_input(HoursFeeder_2, MinutesFeeder_2, SecsFeeder_2);
   
   if(HoursFeeder_2>23 || MinutesFeeder_2>60 || SecsFeeder_2>60 )
   {
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Wrong Input.. !");
   lcd.setCursor(0,1);
   lcd.print("INPUT AGAIN !");
   delay(3000);
   goto InputFail;

   }
   

   lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("Feeder Time-3");
   lcd.setCursor(3,1);
   Time_input(HoursFeeder_3, MinutesFeeder_3, SecsFeeder_3);
   
   if(HoursFeeder_3>23 || MinutesFeeder_3>60 || SecsFeeder_3>60 )
   {
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Wrong Input.. !");
   lcd.setCursor(0,1);
   lcd.print("INPUT AGAIN !");
   delay(3000);
   goto InputFail;

   }

   lcd.clear(); 
   lcd.setCursor(0,0);
   lcd.print(" Rotation:0-360");
   lcd.setCursor(4,1);
   GetRotation(Rotation_Value);
  
   if(Rotation_Value>360 )
   {
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Wrong Input.. !");
   lcd.setCursor(0,1);
   lcd.print("INPUT AGAIN !");
   delay(3000);
   goto InputFail;
   }




   Serial.println(TotalRuntime);

}
void loop()
{
if(TotalRuntime>=1000)
{ 

  update_clock(HoursCurrent, MinutesCurrent,SecsCurrent);
  Serial.print(HoursCurrent);
  Serial.print(MinutesCurrent);
  Serial.print(SecsCurrent);
  TotalRuntime=0;
  remainingTime(rem_hr, rem_min);
  if(rem_hr==0&&rem_min==0 && MotorRuntime>120000)
    {
//      Serial.print("Motor Is running");
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("Feeder Active");
        lcd.setCursor(0,1);
        lcd.print("Rotation :");
        lcd.print(Rotation_Value );
        lcd.print("Deg");
   
       analogWrite(signalPin, 255); 
       delay((Rotation_Value*2.5));
       analogWrite(signalPin, 0); 
      MotorRuntime=0;
    }
  else{
    DisplayRemainingTime();
    }
   
}

}



void DisplayRemainingTime()
{ 
  lcd.clear();
  lcd.setCursor(1,0);
   lcd.print("Time Remainig");
   lcd.setCursor(5,1);
   if(rem_hr < 10)
        lcd.print('0');
   lcd.print(rem_hr);
   sep();
   if(rem_min < 10)
   lcd.print('0');
   lcd.print(rem_min);
  }


char sep()
{    
    if(flag_blink==0)
    {
        lcd.print(":");
        flag_blink=1;
    }
    else {
        lcd.print(" ");
        flag_blink=0;
    }  
}


void remainingTime(int &hour, int &minute)
{ int time_1, time_2, time_3, time_mini;
  time_1= (((HoursFeeder_1*60)+MinutesFeeder_1)-(( HoursCurrent*60)+MinutesCurrent));
  if(time_1<0)
  { time_1+=1440;}
  
  time_2= (((HoursFeeder_2*60)+MinutesFeeder_2)-(( HoursCurrent*60)+MinutesCurrent));
  if(time_2<0)
  { time_2+=1440;}

  time_3= (((HoursFeeder_3*60)+MinutesFeeder_3)-(( HoursCurrent*60)+MinutesCurrent));
  if(time_3<0)
  { time_3+=1440;}
  
if(time_1<time_2)
 {
  time_mini=time_1;
  }else {time_mini=time_2;}
  
if(time_3<time_mini)
  {time_mini=time_3;
  }
//Serial.print("time_mini:");
//Serial.println(time_mini);
//Serial.print("time_1:");
//Serial.println(time_1);
//Serial.print("time_2:");
//Serial.println(time_2);
//Serial.print("time_3:");
//Serial.println(time_3);
hour=time_mini/60;
minute=time_mini%60;
  }

    

void Time_input(int &input_hr, int &input_min, int &input_sec)
{   flag_hr=0;
    flag_min=0;
    flag_sec=0;

    Serial.println("Hi I am here");
while(flag_hr==0 || flag_min ==0||flag_sec==0)
{   
//  Serial.println("Inside while");
//     Serial.println(key);
    char key = customKeypad.getKey();
   
    if (key >= '0' && key <= '9') 
    {     // only act on numeric keys
       lcd.print(key); //realtime print
      inputString += key;               // append new character to input string
      if(inputString.length()==2 && flag_hr == 0)
      { input_hr= inputString.toInt();
        Serial.println(input_hr);
        Serial.println("input_hr");
        inputString = "";                 // clear input
        flag_hr=1;
        lcd.print(":"); //Colon for realtime print
        }
        if(inputString.length()==2 && flag_min == 0)
      { input_min= inputString.toInt();
        Serial.println(input_min);
        Serial.println("input_min");
        inputString = "";                 // clear input
        flag_min=1;
        lcd.print(":"); //Colon for realtime print
        }
        if(inputString.length()==2 && flag_sec == 0)
      { input_sec= inputString.toInt();
        Serial.println(input_sec);
        Serial.println("input_sec");
        inputString = "";                 // clear input
        flag_sec=1;
        lcd.print(":"); //Colon for realtime print
        }
     
    } 
    if (key == '#' || (flag_sec==0&&flag_min&&flag_hr==0)) 
    {
      return(0);
      }

   }
}
    
  
void GetRotation(int &data)
 {
   flag_rot=0;
            while(flag_rot==0)
          { 
              char key = customKeypad.getKey();
              if (key >= '0' && key <= '9') 
              {     // only act on numeric keys
                    lcd.print(key); //realtime print
                    inputString += key;
                    
               
               }else if (key == '#' || inputString.length()==3)
               {
                   
                     data = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
                    
                     flag_rot=1;
                  
               }

          }
 }

void update_clock(int &hours, int &minutes, int &seconds)
{
    seconds++;
    if (seconds >= 60)
    {
        seconds = 0;
        minutes++;
    }
    if(minutes>=60)
    {
      minutes=0;
      hours++;
    }
    if(hours>23)
    {
      hours=0;
    }
}
